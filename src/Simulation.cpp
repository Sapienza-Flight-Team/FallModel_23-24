#include "../include/Simulation.h"

#include <span>

namespace odeint = boost::numeric::odeint;

void Simulation::change_settings(ODESettings settings) {
    /**
     * @brief Changes the settings of the simulation
     *
     * @param settings The new settings to be applied
     *
     * @note Deletes results if pointer is not null
     */
    results.clear();

    time_step = settings.dt;
    time_interval = settings.T;
    method = settings.method;
}

std::span<State> Simulation::run(Model* h, State S0,
                                 std::span<State> res_span) {
    /**
     * Runs the dynamical model simulation using the provided Model object and
     * initial state. If a pointer to a State object is provided, the results
     * will be stored in it. Otherwise, memory will be allocated for the
     * results.
     * .
     *
     * @param h Pointer to the Model object to use for the simulation.
     * @param S0 The initial state of the simulation.
     * @param res_span Pointer to a State object to store the results in.
     * Default is nullptr.
     * @note If res_span is given, memory must be managed by the caller
     */

    using rk4 = odeint::runge_kutta4<State, double, State, double,
                                     odeint::vector_space_algebra>;
    rk4 stepper;
    State S0_step = S0;
    State out;
    std::reference_wrapper<Model> h_ref = *h;

    int n_steps = ceil(time_interval / time_step);

    if (res_span.empty()) {
        // res_span not provided, allocate
        results.clear();
        results.resize(n_steps + 1);
        res_span = results;
    }

    res_span[0] = S0;
    double t = 0;
    if (h->conditionFunc) {
        /*
            Might be nice to add a check of multiple conditions, modifyng the
            internal Model.operator() to another ode law
            Can try to enable 2 or more functions objects, returning the sum of
           ode laws But in this way i need to figure how to stop integration
            where it doesnt make sense

            For now it just stops integration and gg
        */

        // Condition function is defined, run until condition is met
        for (int i = 0; i < n_steps; i++) {
            t = i * time_step;
            stepper.do_step(h_ref, S0_step, t, out, time_step);

            if (h->conditionFunc(S0_step, out, t + time_step)) {
                // Would be nice to do a weighted average of the last two states
                // until last.z > 0
                return res_span.subspan(0, i + 1);
            }
            res_span[i + 1] = out(t + time_step);
            S0_step = out;
        }
    } else {
        // Condition function is not defined, run for n_steps
        for (int i = 0; i < n_steps; i++) {
            t = i * time_step;
            stepper.do_step(h_ref, S0_step, t, out, time_step);
            res_span[i + 1] = out(t + time_step);
            S0_step = out;
        }
    }
    return res_span;  // If the condition is never met, return the last
                      // state
}

std::vector<std::span<State>> Simulation::run_parallel_ic(
    Model* h, std::span<State> v_S0) {
    // Runs the simulation for each initial condition in v_S0

    std::reference_wrapper<Model> h_ref = *h;

    size_t const& n_ic = v_S0.size();
    size_t n_steps = ceil(time_interval / time_step);

    results.clear();
    results.resize(n_ic * (n_steps + 1));

    // Create threads
    std::mutex mtx;

    std::vector<std::span<State>> res_spans(
        n_ic);  // Reserve space for return spans

    std::vector<std::thread> threads(N_THREADS);  // Reserve space for threads

    for (size_t i = 0; i < N_THREADS; i++) {
        threads[i] = std::thread{[&, i]() {
            size_t thread_id{i};

            for (auto j = thread_id; j < n_ic; j += N_THREADS) {
                State const& S0 = v_S0[j];
                std::span<State> r_sp = run(
                    h, S0, {results.begin() + j * (n_steps + 1), n_steps + 1});
                // Lock res_spans for write
                mtx.lock();
                res_spans[j] = r_sp;
                mtx.unlock();
            }
        }};
    }

    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }
    return res_spans;
}

// // Need to find a better way to get out the last pointers, might be dangerous
// // like this
// std::vector<std::span<State>> Simulation::run_parallel_ic_cond(
//     Model* h, std::span<State> v_S0, simulation_predicate cond_func) {
//     // Runs the simulation for each initial condition in v_S0
//     std::atomic<size_t> counter = 0;
//     size_t n_ic = v_S0.size();
//     size_t n_steps = ceil(time_interval / time_step);
//     std::mutex mtx;
//     std::vector<std::span<State>> map_states(n_ic);

//     results.clear();
//     results.reserve(n_ic * (n_steps + 1));

//     // Create threads
//     std::vector<std::thread> threads(N_THREADS);

//     for (int i = 0; i < N_THREADS; i++) {
//         threads.emplace_back([&]() {
//             int thread_id{i};

//             for (auto j = thread_id; j += N_THREADS; j < v_S0.size()) {
//                 State const& S0 = v_S0[j];
//                 size_t res_size = run_cond(
//                     h, S0, cond_func,
//                     {results.begin() + j * (n_steps + 1), n_steps + 1});
//                 std::span res_span{results.begin() + j * (n_steps + 1),
//                                    res_size};
//                 // Lock res_span for write
//                 mtx.lock();
//                 map_states.push_back(res_span);
//                 mtx.unlock();
//             }
//         });
//     }

//     // Wait for all threads to finish
//     for (auto& thread : threads) {
//         thread.join();
//     }
//     return map_states;
// }

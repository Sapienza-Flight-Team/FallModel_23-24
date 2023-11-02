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

void Simulation::run(Model* h, State S0, std::span<State> res_span) {
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
        results.reserve(n_steps + 1);
        res_span = results;
    }

    res_span[0] = S0;
    double t = 0;

    for (int i = 0; i < n_steps; i++) {
        t = i * time_step;
        stepper.do_step(h_ref, S0_step, t, out, time_step);
        res_span[i + 1] = out(t);
        S0_step = out;
    }
}

size_t Simulation::run_cond(Model* h, State S0, simulation_predicate cond_func,
                            std::span<State> res) {
    /**
     * Runs a simulation of a model until a given condition is met.
     *
     *
     * @param h Pointer to the model to simulate.
     * @param S0 Initial state of the simulation.
     * @param cond_func Function that returns true if the simulation should
     * stop.
     * @param res Pointer to the array where the results will be stored. If
     * null, memory will be allocated.
     * @note If res is given, memory must be managed by the caller
     * @return Number of steps executed until cond.
     */

    typedef odeint::runge_kutta4<State, double, State, double,
                                 odeint::vector_space_algebra>
        rk4;
    rk4 stepper;
    State S0_step = S0;
    State out;
    std::reference_wrapper<Model> h_ref = *h;
    int n_steps = ceil(time_interval / time_step);

    if (res.empty()) {
        // res_span not provided, allocate
        results.clear();
        results.reserve(n_steps + 1);
        res = results;
    }

    res[0] = S0;
    double t = 0;

    for (int i = 0; i < n_steps; i++) {
        t = i * time_step;
        stepper.do_step(h_ref, S0_step, t, out, time_step);

        if (cond_func(S0_step, out, t)) {
            // Would be nice to do a weighted average of the last two states
            // until last.z > 0

            return i;
        }
        res[i + 1] = out(t);
        S0_step = out;
    }
    return n_steps;  // If the condition is never met, return the last
                     // state
}

// void Simulation::run_parallel_ic(Model* h, std::span<State> v_S0) {
//     // Runs the simulation for each initial condition in v_S0
//     size_t const& n_ic = v_S0.size();
//     size_t n_steps = ceil(time_interval / time_step);

//     results.clear();
//     results.reserve(n_ic * (n_steps + 1));

//     // Create threads
//     std::vector<std::thread> threads(N_THREADS);
//     std::reference_wrapper<Model> h_ref = *h;

//     for (int i = 0; i < N_THREADS; i++) {
//         threads.emplace_back(
//             [this](int i, std::span<State> v_S0, int n_steps,
//                    std::span<State> results, Model* h) {
//                 std::cout << "Spawned thread " << i << std::endl;
//                 size_t thread_id{i};

//                 for (auto j = thread_id; j += N_THREADS; j < v_S0.size()) {
//                     State const& S0 = v_S0[j];
//                     run(h, S0,
//                         {results.begin() + j * (n_steps + 1), n_steps + 1});
//                 }
//             },
//             i, v_S0, n_steps, results, h);
//     }

//     for (auto& t : threads) {
//         t.join();
//     }
//     threads.clear();
// }

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

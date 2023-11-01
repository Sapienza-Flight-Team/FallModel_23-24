#include "../include/Simulation.h"

#include "Simulation.h"

namespace odeint = boost::numeric::odeint;

void Simulation::change_settings(ODESettings settings) {
    /**
     * @brief Changes the settings of the simulation
     *
     * @param settings The new settings to be applied
     *
     * @note Deletes results if pointer is not null
     */
    if (results != nullptr) {
        results.reset();
    }
    time_step = settings.dt;
    time_interval = settings.T;
    method = settings.method;
}

void Simulation::run(Model* h, State S0, State* res_ptr) {
    /**
     * Runs the dynamical model simulation using the provided Model object and
     * initial state. If a pointer to a State object is provided, the results
     * will be stored in it. Otherwise, memory will be allocated for the
     * results.
     * .
     *
     * @param h Pointer to the Model object to use for the simulation.
     * @param S0 The initial state of the simulation.
     * @param res_ptr Pointer to a State object to store the results in. Default
     * is nullptr.
     * @note If res_ptr is given, memory must be managed by the caller
     */

    typedef odeint::runge_kutta4<State, double, State, double,
                                 odeint::vector_space_algebra>
        rk4;
    rk4 stepper;
    State S0_step = S0;
    State out;
    std::reference_wrapper<Model> h_ref = *h;

    int n_steps = ceil(time_interval / time_step);

    // _allocate memory for results
    if (res_ptr == nullptr) {
        // If no pointer is passed, _allocate memory
        if (results != nullptr) {
            // If results is not null, delete it
            Simulation::_deallocate();
        }
        Simulation::_allocate(n_steps + 1);  // +1 to include initial condition
        res_ptr = results.get();
    }

    res_ptr[0] = S0;
    double t = 0;

    for (int i = 0; i < n_steps; i++) {
        t = i * time_step;
        stepper.do_step(h_ref, S0_step, t, out, time_step);
        res_ptr[i + 1] = out(t);
        S0_step = out;
    }
}

size_t Simulation::run_cond(Model* h, State S0,
                            bool (*cond_func)(State S0, State S0_dot, double t),
                            State* res_ptr) {
    /**
     * Runs a simulation of a model until a given condition is met.
     *
     *
     * @param h Pointer to the model to simulate.
     * @param S0 Initial state of the simulation.
     * @param cond_func Function that returns true if the simulation should
     * stop.
     * @param res_ptr Pointer to the array where the results will be stored. If
     * null, memory will be allocated.
     * @note If res_ptr is given, memory must be managed by the caller
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

    // _allocate memory for results
    if (res_ptr == nullptr) {
        // If no pointer is passed, _allocate memory
        if (results != nullptr) {
            // If results is not null, delete it
            Simulation::_deallocate();
        }
        Simulation::_allocate(n_steps + 1);  // +1 to include initial condition
        res_ptr = results.get();
    }

    res_ptr[0] = S0;
    double t = 0;

    for (int i = 0; i < n_steps; i++) {
        t = i * time_step;
        stepper.do_step(h_ref, S0_step, t, out, time_step);

        if (cond_func(S0_step, out, t)) {
            // Would be nice to do a weighted average of the last two states
            // until last.z > 0

            return i;
        }
        res_ptr[i + 1] = out(t);
        S0_step = out;
    }
    return n_steps;  // If the condition is never met, return the last
                     // state
}

void Simulation::run_parallel_ic(Model* h, const std::vector<State>& v_S0) {
    // Runs the simulation for each initial condition in v_S0
    std::atomic<size_t> counter(0);
    size_t n_ic = v_S0.size();
    size_t n_steps = ceil(time_interval / time_step);
    // _allocate memory for results
    if (results != nullptr) {
        // If results is not null, delete it
        _deallocate();
    }
    Simulation::_allocate(n_ic *
                          (n_steps + 1));  // +1 to include initial condition

    // Create threads
    std::vector<std::thread> threads(N_THREADS);

    for (int i = 0; i < N_THREADS; i++) {
        threads.emplace_back([&]() {
            size_t index;
            while ((index = counter.fetch_add(1)) < n_ic) {
                // Run simulation for the current initial condition
                State S0 = v_S0[index];
                State* res_ptr = results.get() + index * (n_steps + 1);
                run(h, S0, res_ptr);
            }
        });
        threads[i].join();
    }
}
// Need to find a better way to get out the last pointers, might be dangerous
// like this
std::vector<size_t> Simulation::run_parallel_ic_cond(
    Model* h, const std::vector<State>& v_S0,
    bool (*cond_func)(State S0, State S0_dot, double t)) {
    // Runs the simulation for each initial condition in v_S0
    std::atomic<size_t> counter = 0;
    size_t n_ic = v_S0.size();
    size_t n_steps = ceil(time_interval / time_step);
    std::mutex mtx;
    std::vector<size_t> final_states(n_ic);
    // _allocate memory for results
    if (results != nullptr) {
        // If results is not null, delete it
        _deallocate();
    }
    Simulation::_allocate(n_ic *
                          (n_steps + 1));  // +1 to include initial condition

    // Create threads
    std::vector<std::thread> threads;

    for (int i = 0; i < N_THREADS; i++) {
        threads.emplace_back([&]() {
            size_t index;
            while ((index = counter.fetch_add(1)) < n_ic) {
                // Run simulation for the current initial condition
                State S0 = v_S0[index];
                State* res_ptr = results.get() + index * (n_steps + 1);
                size_t last_step = run_cond(h, S0, cond_func, res_ptr);

                // Protect final_states with mutex
                mtx.lock();
                final_states[index] = last_step;
                mtx.unlock();
            }
        });
    }

    // Wait for all threads to finish
    for (auto& thread : threads) {
        thread.join();
    }
    return final_states;
}

void Simulation::_allocate(size_t size) {
    /**
     * @brief Allocates memory for the simulation results.
     *
     * @param size The size of the memory to be allocated.
     */

    if (results != nullptr) {
        results.reset();
    }
    try {
        results = std::make_unique<State>(size);
        res_size = size;
    } catch (const std::bad_alloc& e) {
        std::cerr << "Simulation::_allocate: " << e.what() << '\n';
    }
}

void Simulation::_deallocate() {
    /**
     * @brief Deallocates the results of the simulation.
     *
     * If the results are not null, it deallocates them and sets the pointer to
     * null. Also sets the size of the results to 0.
     */
    if (results != nullptr) {
        results.reset();  // Deallocates memory and sets pointer to null
        res_size = 0;
    }
}
/**
 * @brief Takes ownership of the results unique_ptr and assigns it to the in_ptr
 * variable.
 *
 * @param in_ptr The unique_ptr to be assigned to the results member variable.
 */
void Simulation::own_res(std::unique_ptr<State>& in_ptr) {
    in_ptr = std::move(results);
    results = nullptr;  // Should be covered by the move op
    res_size = 0;
}

std::vector<std::vector<State>> Simulation::vRes(std::vector<size_t> v_ptr) {
    /**
     * TODO: FINISH THIS - Not working properly
     * @brief Converts a vector of pointers to State objects to a vector of
     * vectors of State objects.
     *
     * @param v_ptr The vector of pointers to be converted.
     * @return std::vector<std::vector<State>> The vector of vectors of State
     * objects.
     */
    // if (results != nullptr) {
    //     std::vector<std::vector<State>> v_res;
    //     size_t idx = 0;
    //     size_t n_state_off = ((time_interval / time_step) + 1);
    //     for (auto& ptr : v_ptr) {
    //         std::vector<State> v;
    //         State* begin = results.get() + idx * n_state_off;
    //         // State* end = begin + ptr;
    //         State* end = begin + n_state_off;
    //         v.reserve(end - begin);

    //         for (int i = 0; i < res_size; i++) {
    //             v.push_back(ptr[i]);
    //         }
    //         v_res.push_back(v);
    //     }
    //     return v_res;
    // }
    return std::vector<std::vector<State>>();
}

#pragma once

#include <boost/numeric/odeint.hpp>
#include <cmath>
#include <functional>
#include <memory>
#include <span>
#include <string>
#include <thread>
#include <vector>

#include "BaseStepper.h"
#include "Model.h"
#include "State.h"

constexpr inline size_t N_THREADS = 6;

/**
 * @brief Struct containing settings for an ODE simulation.
 *
 */
typedef struct {
    // Methods parameters
    double dt;          /**< Time step */
    double T;           /**< Total simulation time */
    std::string method; /**< ODE solving method */
} ODESettings;

/**
 * @brief Class for simulating the model.
 *
 */
template <size_t N>
class Simulation {
   private:
    // Methods parameters
    std::string method;            /**< Method to be used for simulation. */
    double time_step;              /**< Time step for simulation. */
    double time_interval;          /**< Time interval for simulation. */
    std::vector<State<N>> results; /**< Vector containing the results of the
                                     simulation. */
    BaseStepper<N>* stepper;       /**< Pointer to the stepper object. */

   public:
    /**
     * @brief Constructor for Simulation class.
     *
     * @param dt Time step for simulation.
     * @param T Time interval for simulation.
     * @param method Method to be used for simulation.
     */
    Simulation(double dt, double T, std::string method = "")
        : method(method), time_step(dt), time_interval(T) {
        if (method == "rk4" || method == "") {
            // Default method
            stepper = new RK4Stepper<N>();
        }
        // else if (method == "rk45") {
        //     stepper = new RK4Stepper();
        // }
        // // else if (method == "ode113")
        // // {
        // //     /* code */
        // // }
        // else if (method == "euler") {
        //     stepper = new EulerStepper();
        // }
        else {
            throw std::invalid_argument("Invalid method");
        }
    }

    /**
     * @brief Constructor for Simulation class.
     *
     * @param settings ODESettings object containing simulation settings.
     */
    Simulation(ODESettings settings)
        : method(settings.method),
          time_step(settings.dt),
          time_interval(settings.T) {}

    /**
     * @brief Changes the simulation settings.
     *
     * @param settings ODESettings object containing new simulation settings.
     */
    void change_settings(ODESettings settings);

    /**
     * @brief Runs the simulation.
     *
     * @param h Pointer to the Model object.
     * @param S0 Initial State<N> of the system.
     * @param res_ptr Pointer to the result array.
     */

    std::span<State<N>> run(Model<N>* h, State<N> S0,
                            std::span<State<N>> res_ptr = {});

    /**
     * @brief Runs the simulation in parallel for multiple initial conditions.
     *
     * @param h Pointer to the Model object.
     * @param v_S0 Vector of initial states of the system.
     */
    std::vector<std::span<State<N>>> run_parallel_ic(Model<N>* h,
                                                     std::span<State<N>> v_S0);

    void own_res(std::vector<State<N>>& v_res) {
        /**
         * @brief Returns the results of the simulation.
         * Transfers ownership from the private member to the user vector.
         * User should handle runtime_exception if results is empty.
         *
         * @return std::vector<State<N>> Vector containing the results of the
         * simulation.
         *
         */

        if (results.empty()) {
            throw std::runtime_error("No results to return");
        } else {
            v_res = std::move(results);
            results.clear();
        }
    }
};

// Implementations

namespace odeint = boost::numeric::odeint;

template <size_t N>
void Simulation<N>::change_settings(ODESettings settings) {
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

template <size_t N>
std::span<State<N>> Simulation<N>::run(Model<N>* h, State<N> S0,
                                       std::span<State<N>> res_span) {
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

    // Create stepper
    RK4Stepper<N> stepper;
    State<N> S0_step = S0;
    State<N> out;
    std::reference_wrapper<Model<N>> h_ref = *h;

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

template <size_t N>
std::vector<std::span<State<N>>> Simulation<N>::run_parallel_ic(
    Model<N>* h, std::span<State<N>> v_S0) {
    // Runs the simulation for each initial condition in v_S0

    size_t const& n_ic = v_S0.size();
    size_t n_steps = ceil(time_interval / time_step);

    results.clear();
    try {
        results.resize(n_ic * (n_steps + 1));
    } catch (std::bad_alloc& ba) {
        std::cerr << "bad_alloc caught: " << ba.what() << '\n';
        std::cerr << "Trying to allocate "
                  << n_ic * (n_steps + 1) * sizeof(State<N>) / std::pow(10, 9)
                  << " GB\n";
        std::cerr << "Not enough memory to allocate results vector\n";
        std::cerr << "Try reducing the number of initial conditions\n";
        std::cerr << "or decreasing the time interval\n";
        std::cerr << "or increasing the time step\n";

        std::cerr << "or buying more RAM\n";
        std::exit(EXIT_FAILURE);
    }

    // Create threads

    std::vector<std::span<State<N>>> res_spans(
        n_ic);  // Reserve space for return spans

    std::vector<std::thread> threads(N_THREADS);  // Reserve space for threads

    for (size_t i = 0; i < N_THREADS; i++) {
        threads[i] = std::thread{[&, i]() {
            size_t thread_id{i};

            for (auto j = thread_id; j < n_ic; j += N_THREADS) {
                State<N> const& S0 = v_S0[j];
                std::span<State<N>> r_sp = run(
                    h, S0, {results.begin() + j * (n_steps + 1), n_steps + 1});
                // No need to lock res_spans for write
                res_spans[j] = r_sp;
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

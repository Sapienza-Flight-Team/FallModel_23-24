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

enum Metod { rk4, rk45, euler };

extern const unsigned N_THREADS{[&]() {
    unsigned n_threads{std::thread::hardware_concurrency()};
    if (n_threads == 0) {
        n_threads = 1;
    }
    return n_threads;
}()};

TODO:   
template <size_t N>
struct Results {
    std::vector<State<N>> results;
    std::vector<std::span<State<N>>> results_spans;

    Results() : results(), results_spans() {}
    Results(size_t n, size_t m = 1) : results(n), results_spans(m) {}
    void set_span(size_t m) {
        // Allowed only if results is empty
        if (results.empty()) {
            results_spans.resize(m);
        } else {
            throw std::runtime_error("Results is not empty");
        }
    }
};

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
class Simulation {
   private:
    // Methods parameters
    std::string method;   /**< Method to be used for simulation. */
    double time_step;     /**< Time step for simulation. */
    double time_interval; /**< Time interval for simulation. */
    Metod m;

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
            // stepper = new RK4Stepper<N>();
            Metod m = rk4;
        } else if (method == "rk45") {
            // stepper = new RK45Stepper<N>();
            Metod m = rk45;
        }
        // else if (method == "ode113")
        // {
        //     /* code */
        // }
        else if (method == "euler") {
            // stepper = new EulerStepper<N>();
            Metod m = euler;
        } else {
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
     * @param settings ODESettings object containing new simulation
     * settings.
     */
    void change_settings(ODESettings settings);

    /**
     * @brief Runs the simulation.
     *
     * @param h Pointer to the Model object.
     * @param S0 Initial State<N> of the system.
     * @param res_ptr Pointer to the result array.
     */
    // TODO:
    template <size_t N>
    Results<N> run(Model<N>* h, State<N> S0, std::span<State<N>> res_ptr = {});

    /**
     * @brief Runs the simulation in parallel for multiple initial
     * conditions.
     *
     * @param h Pointer to the Model object.
     * @param v_S0 Vector of initial states of the system.
     *
     */
    template <size_t N>
    TODO:
    Results<N> run_parallel_ic(Model<N>* h, std::span<State<N>> v_S0);

    // void own_res(std::vector<State<N>>& v_res) {
    //     /**
    //      * @brief Returns the results of the simulation.
    //      * Transfers ownership from the private member to the user
    //      vector.
    //      * User should handle runtime_exception if results is empty.
    //      *
    //      * @return std::vector<State<N>> Vector containing the results of
    //      the
    //      * simulation.
    //      *
    //      */

    //     if (results.empty()) {
    //         throw std::runtime_error("No results to return");
    //     } else {
    //         v_res = std::move(results);
    //         results.clear();
    //     }
    // }
};

// Implementations

namespace odeint = boost::numeric::odeint;

void Simulation::change_settings(ODESettings settings) {
    /**
     * @brief Changes the settings of the simulation
     *
     * @param settings The new settings to be applied
     *
     * @note Deletes results if pointer is not null
     */
    // results.clear();

    time_step = settings.dt;
    time_interval = settings.T;
    method = settings.method;
}

template <size_t N>
// TODO: Decidere come tornare Results se run viene chiamato con run_parallel_ic
Results<N> Simulation::run(Model<N>* h, State<N> S0,
                           std::span<State<N>> res_span) {
    /**
     * Runs the dynamical model simulation using the provided Model object
     * and initial state. If a pointer to a State object is provided, the
     * results will be stored in it. Otherwise, memory will be allocated for
     * the results.
     * .
     *
     * @param h Pointer to the Model object to use for the simulation.
     * @param S0 The initial state of the simulation.
     * @param res_span Pointer to a State object to store the results in.
     * Default is nullptr.
     * @note If res_span is given, memory must be managed by the caller
     */

    constexpr size_t N = h->getN();
    // Create stepper

    if (m == rk4) {
        odeint::runge_kutta4<State<N>> stepper;
    } else if (m == rk45) {
        odeint::runge_kutta_dopri5<State<N>> stepper;
    } else if (m == euler) {
        odeint::euler<State<N>> stepper;
    }

    // Create a copy of the Model object
    // This is done to avoid race conditions if an Event modifies the Model
    Model<N> h_cl = h->clone();

    int n_steps = ceil(time_interval / time_step);

    if (res_span.empty()) {
        // res_span not provided, create results and allocate
        // TODO: run should allocate only if is not called by run_parallel_ic.
        // How to check this with Result struct? ->
    }

    res_span[0] = S0;
    State<N> out;
    State<N> S0_step = S0;
    double t = 0;

    if (h_cl.conditionFunc) {
        /*
            Might be nice to add a check of multiple conditions, modifyng
           the internal Model.operator() to another ode law Can try to
           enable 2 or more functions objects, returning the sum of ode laws
           But in this way i need to figure how to stop integration where it
           doesnt make sense

            For now it just stops integration and gg
        */

        // Condition function is defined, run until condition is met
        for (int i = 0; i < n_steps; i++) {
            t = i * time_step;
            stepper.do_step(h_cl, S0_step, t, out, time_step);

            if (h_cl->conditionFunc(S0_step, out, t + time_step)) {
                // Would be nice to do a weighted average of the last two
                // states until last.z > 0
                return res_span.subspan(0, i + 1);
            }
            res_span[i + 1] = out(t + time_step);
            S0_step = out;
        }
    } else {
        // Condition function is not defined, run for n_steps witthout checking
        // conditions
        for (int i = 0; i < n_steps; i++) {
            t = i * time_step;
            stepper.do_step(h_cl, S0_step, t, out, time_step);
            res_span[i + 1] = out(t + time_step);
            S0_step = out;
        }
    }
    return res_span;  // If the condition is never met, return the last
                      // state
}

template <size_t N>
Results<N> Simulation::run_parallel_ic(Model<N>* h, std::span<State<N>> v_S0) {
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

        std::cerr << "or downloading more RAM\n";
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

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

enum Format { B = 1,
    KB = 3,
    MB = 6,
    GB = 9 };

std::string getFormatString(Format format)
{
    std::string formatString = 0;
    switch (format) {
    case B:
        formatString = "B";
        break;
    case KB:
        formatString = "KB";
        break;
    case MB:
        formatString = "MB";
        break;
    case GB:
        formatString = "GB";
        break;
    default:
        formatString = "";
        break;
    }
    return formatString;
}
void handle_bad_alloc [[noreturn]] (std::bad_alloc& ba, size_t size, Format format = B)
{
    std::cerr << "bad_alloc caught: " << ba.what() << '\n';
    std::cerr << "Trying to allocate " << size / std::pow(10, int(format))
              << getFormatString(format) << "\n";
    std::cerr << "Not enough memory to allocate results vector\n";
    std::cerr << "Try reducing the number of initial conditions\n";
    std::cerr << "or decreasing the time interval\n";
    std::cerr << "or increasing the time step\n";

    std::cerr << "or downloading more RAM\n";
    std::exit(EXIT_FAILURE);
}

enum Method { rk4,
    rk45,
    euler };

extern const unsigned N_THREADS { []() {
    unsigned n_threads { std::thread::hardware_concurrency() };
    if (n_threads == 0) {
        n_threads = 1;
    }
    return n_threads;
}() };

template <size_t N>
struct Results {
private:
    std::unique_ptr<State<N>[]> results {};
    std::vector<std::span<State<N>>> results_view;

public:
    Results() = default;
    Results(State<N>* res, std::span<State<N>> res_span)
        : results(res)
    {
        results_view.push_back(res_span);
    }

    Results(State<N>* res, std::vector<std::span<State<N>>> res_spans)
        : results(res)
        , results_view(res_spans)
    {
    }
    ~Results() = default;

    // View only operator, doenst allow to modify the results
    const std::span<State<N>>& operator[](size_t i) const
    {
        if (results) {
            if (i >= results_view.size() || results_view[i].empty()) {
                throw std::out_of_range("Index out of range");
            }

            return results_view[i]; // Sintax results[i][j] to access results

        } else {
            throw std::runtime_error("Results not initialized");
        }
    }

    auto getLast(size_t idx = 0) -> State<N>
    {
        if (results) {
            if (results_view.size() >= idx) {
                return results_view[idx].back();
            } else {
                throw std::runtime_error("Index out of range");
            }
        } else {
            throw std::runtime_error("Results not initialized");
        }
    }
    auto sanityCheck() -> bool { return results; }
    auto getSSize() -> size_t { return results_view.size(); }
};

/**
 * @brief Struct containing settings for an ODE simulation.
 *
 */
using ODESettings = struct {
    // Methods parameters
    double dt {}; /**< Time step */
    double T {}; /**< Total simulation time */
    std::string method {}; /**< ODE solving method */
};

/**
 * @brief Class for simulating the model.
 *
 */
class Simulation {
private:
    // Methods parameters
    Method m; /**< ODE solving method */
    double time_step; /**< Time step for simulation. */
    double time_interval; /**< Time interval for simulation. */

    template <typename ModelType, size_t N = ModelType::getN()>
    std::span<State<N>> _run(ModelType h, State<N> S0,
        std::span<State<N>> res_span,
        std::unique_ptr<BaseStepper<ModelType>>&& stepper);

public:
    /**
     * @brief Constructor for Simulation class.
     *
     * @param dt Time step for simulation.
     * @param T Time interval for simulation.
     * @param method Method to be used for simulation.
     */
    Simulation(double dt, double T, std::string method = "")
    {
        // dt and T sanity check
        if (dt <= 0) {
            throw std::invalid_argument("dt must be nonzero positive");
        } else if (T <= 0) {
            throw std::invalid_argument("T must be nonzero positive");
        }
        this->time_step = dt;
        this->time_interval = T;

        // Method selection

        if (method == "rk4" || method == "") {
            // Default method
            this->m = rk4;
        } else if (method == "rk45") {
            this->m = rk45;
        }
        // else if (method == "ode113")
        // {
        //     /* code */
        // }
        else if (method == "euler") {
            this->m = euler;
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
        : Simulation(settings.dt, settings.T, settings.method)
    {
    }

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

    template <typename ModelType, size_t N = ModelType::getN()>
    auto run(ModelType h, State<N> S0) -> Results<N>;

    /**
     * @brief Runs the simulation in parallel for multiple initial
     * conditions.
     *
     * @param h ModelType object, has to inherit from Model<N>.
     * @param v_S0 Vector of initial states of the system.
     *
     */
    template <typename ModelType, size_t N = ModelType::getN()>
    auto run_parallel_ic(ModelType h, std::span<State<N>> v_S0) -> Results<N>;
};

// Implementations

template <typename ModelType, size_t N>
std::span<State<N>> Simulation::_run(ModelType h, State<N> S0,
    std::span<State<N>> w_span,
    std::unique_ptr<BaseStepper<ModelType>>&& stepper)
{
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

    // --- Compile time instructions ---

    static_assert(std::is_base_of<Model<N>, ModelType>::value,
        "ModelType must be derived from Model");

    int n_steps = w_span.size() - 1;

    w_span[0] = S0;
    State<N> out;
    State<N> S0_step = S0;
    double t = 0;

    if (h.conditionFunc) {
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
            stepper->do_step(h, S0_step, t, out, time_step);

            if (h.conditionFunc(S0_step, out,
                    t + time_step)) { // TODO: Da toccare
                // Would be nice to do a weighted average of the last two
                // states until last.z > 0
                return w_span.subspan(0, i + 1);
            }
            w_span[i + 1] = out(t + time_step);
            S0_step = out;
        }
    } else {
        // Condition function is not defined, run for n_steps witthout checking
        // conditions
        for (int i = 0; i < n_steps; i++) {
            t = i * time_step;
            stepper->do_step(h, S0_step, t, out, time_step);
            w_span[i + 1] = out(t + time_step);
            S0_step = out;
        }
    }

    return w_span; // If the condition is never met, return the last
                   // state
}

template <typename ModelType, size_t N>
auto Simulation::run(ModelType h, State<N> S0) -> Results<N>
{
    // ...
    const size_t n_steps = ceil(this->time_interval / this->time_step);
    std::unique_ptr<BaseStepper<ModelType>> stepper;
    try {
        if (m == rk4) {
            stepper = std::make_unique<RK4Stepper<ModelType>>();
        } else if (m == rk45) {
            stepper = std::make_unique<RK45Stepper<ModelType>>();
        } else if (m == euler) {
            stepper = std::make_unique<EulerStepper<ModelType>>();
        } else {
            throw std::invalid_argument("Invalid method");
        }
    } catch (std::bad_alloc& ba) {
        // Handle bad_alloc
        handle_bad_alloc(ba, sizeof(BaseStepper<ModelType>), B);
    }

    // Allocate memory for results
    State<N>* res = nullptr;
    try {
        res = new State<N>[n_steps + 1];
    } catch (std::bad_alloc& ba) {
        // Handle bad_alloc
        handle_bad_alloc(ba, (n_steps + 1) * sizeof(State<N>), KB);
    }
    // Run simulation
    std::span<State<N>> r_sp = _run(h, S0, { res, n_steps + 1 }, std::move(stepper));

    return Results(res, r_sp);
}

template <typename ModelType, size_t N>
auto Simulation::run_parallel_ic(ModelType h, std::span<State<N>> v_S0) -> Results<N>
{
    // Runs the simulation for each initial condition in v_S0

    // Check if ModelType is derived from Model
    static_assert(std::is_base_of<Model<N>, ModelType>::value,
        "ModelType must be derived from Model");

    // This needs to be modyfied for multistep methods, as each method needs its
    // own stepper
    // Create an array of BaseStepper[N_THREADS] pointers and instantiate each
    // pointer with the correct stepper
    BaseStepper<ModelType>** stepper = nullptr = nullptr;
    try {
        stepper = new BaseStepper<ModelType>*[N_THREADS];
        for (size_t i = 0; i < N_THREADS; i++) {
            if (m == rk4) {
                stepper[i] = new RK4Stepper<ModelType>();
            } else if (m == rk45) {
                stepper[i] = new RK45Stepper<ModelType>();
            } else if (m == euler) {
                stepper[i] = new EulerStepper<ModelType>();
            } else {
                throw std::invalid_argument("Invalid method");
            }
        }
    } catch (std::bad_alloc& ba) {
        // Handle bad_alloc
        handle_bad_alloc(ba, N_THREADS * sizeof(BaseStepper<ModelType>*), KB);
    }

    size_t const& n_ic = v_S0.size();
    const size_t n_steps = ceil(this->time_interval / this->time_step);

    // Allocate memory for results
    State<N>* res = nullptr;
    try {
        res = new State<N>[n_ic * (n_steps + 1)];
    } catch (std::bad_alloc& ba) {
        // Handle bad_alloc
        handle_bad_alloc(ba, (n_steps + 1) * sizeof(State<N>), MB);
    }

    std::vector<std::span<State<N>>> v_res_spans(
        n_ic); // Reserve space for return spans
    // Create threads
    std::vector<std::thread> threads(N_THREADS);

    for (size_t i = 0; i < N_THREADS; i++) {
        threads[i] = std::thread([&, i]() {
            size_t thread_id = i;

            for (auto j = thread_id; j < n_ic; j += N_THREADS) {
                State<N> const& S0 = v_S0[j];
                std::span<State<N>> r_sp = _run(
                    h, S0, { res + j * (n_steps + 1), n_steps + 1 }, stepper[i]);
                // No need to lock res_spans for write
                v_res_spans[j] = r_sp;
                // if stepper is multi step, it needs to be cleaned here
            }
        });
    }

    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }
    for (size_t i = 0; i < N_THREADS; i++) {
        delete stepper[i]; // Clean steppers
    }
    delete[] stepper; // Clean stepper
    return Results(res, v_res_spans);
}

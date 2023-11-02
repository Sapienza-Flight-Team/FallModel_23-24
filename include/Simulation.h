#pragma once

#include <boost/numeric/odeint.hpp>
#include <cmath>
#include <functional>
#include <memory>
#include <mutex>
#include <span>
#include <string>
#include <thread>
#include <vector>

#include "Model.h"
#include "State.h"

constexpr inline uint N_THREADS = 6;

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
    using simulation_predicate =
        std::function<bool((State S0, State S0_dot, double t))>;

   private:
    // Methods parameters
    std::string method;   /**< Method to be used for simulation. */
    double time_step;     /**< Time step for simulation. */
    double time_interval; /**< Time interval for simulation. */

    std::vector<State> results;

    void _run_thread(Model* h, State S0, std::span<State> res_span,
                     std::atomic<int>& id, std::mutex& mtx);
    void _run_thread_cond(Model* h, State S0, simulation_predicate cond_func,
                          std::span<State> res_span, std::atomic<int>& id,
                          std::mutex& mtx);

   public:
    /**
     * @brief Constructor for Simulation class.
     *
     * @param dt Time step for simulation.
     * @param T Time interval for simulation.
     * @param method Method to be used for simulation.
     */
    Simulation(double dt, double T, std::string method)
        : method(method), time_step(dt), time_interval(T) {}

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
     * @param S0 Initial state of the system.
     * @param res_ptr Pointer to the result array.
     */
    void run(Model* h, State S0, std::span<State> res_ptr = {});

    /**
     * @brief Runs the simulation until a certain condition is met.
     *
     * @param h Pointer to the Model object.
     * @param S0 Initial state of the system.
     * @param cond_func Function pointer to the condition function.
     * @param res_ptr Pointer to the result array.
     * @return State* Pointer to the final state of the system.
     */
    size_t run_cond(Model* h, State S0, simulation_predicate cond_func,
                    std::span<State> res_ptr = {});

    /**
     * @brief Runs the simulation in parallel for multiple initial conditions.
     *
     * @param h Pointer to the Model object.
     * @param v_S0 Vector of initial states of the system.
     */
    void run_parallel_ic(Model* h, std::span<State> v_S0);

    /**
     * @brief Runs the simulation in parallel for multiple initial conditions
     * until a certain condition is met.
     *
     * @param h Pointer to the Model object.
     * @param v_S0 Vector of initial states of the system.
     * @param cond_func Function pointer to the condition function.
     */
    std::vector<std::span<State>> run_parallel_ic_cond(
        Model* h, std::span<State> v_S0, simulation_predicate cond_func);

    std::vector<State> ret_res() { return this->results; }
};

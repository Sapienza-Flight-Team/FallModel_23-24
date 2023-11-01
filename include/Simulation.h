#pragma once

#include <boost/numeric/odeint.hpp>
#include <cmath>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "Model.h"
#include "State.h"

#define N_THREADS 6

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

typedef struct {
    State* begin;
    State* end;
} StateRange;
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
    std::unique_ptr<State> results =
        nullptr;         /**< Pointer to result array of the simulation. */
    size_t res_size = 0; /**< Size of result array. */

    /**
     * @brief Allocates memory for the result array.
     *
     * @param size Size of the result array.
     */
    void _allocate(size_t size);

    /**
     * @brief Deallocates memory for the result array.
     *
     */
    void _deallocate();

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
     * @brief Destructor for Simulation class.
     *
     */
    ~Simulation() { _deallocate(); }

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
    void run(Model* h, State S0, State* res_ptr = nullptr);

    /**
     * @brief Runs the simulation until a certain condition is met.
     *
     * @param h Pointer to the Model object.
     * @param S0 Initial state of the system.
     * @param cond_func Function pointer to the condition function.
     * @param res_ptr Pointer to the result array.
     * @return State* Pointer to the final state of the system.
     */
    size_t run_cond(Model* h, State S0,
                    bool (*cond_func)(State S0, State S0_dot, double t),
                    State* res_ptr = nullptr);

    /**
     * @brief Runs the simulation in parallel for multiple initial conditions.
     *
     * @param h Pointer to the Model object.
     * @param v_S0 Vector of initial states of the system.
     */
    void run_parallel_ic(Model* h, const std::vector<State>& v_S0);

    /**
     * @brief Runs the simulation in parallel for multiple initial conditions
     * until a certain condition is met.
     *
     * @param h Pointer to the Model object.
     * @param v_S0 Vector of initial states of the system.
     * @param cond_func Function pointer to the condition function.
     */
    std::vector<size_t> run_parallel_ic_cond(
        Model* h, const std::vector<State>& v_S0,
        bool (*cond_func)(State S0, State S0_dot, double t));
    /**
     * @brief Takes ownership of the results unique_ptr and assigns it to the
     * in_ptr variable.
     *
     * @param in_ptr The unique_ptr to be assigned to the results member
     * variable.
     */
    void own_res(std::unique_ptr<State>& in_ptr);
    std::vector<std::vector<State>> vRes(std::vector<size_t> v_ptr);
};

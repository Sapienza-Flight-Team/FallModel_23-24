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
class Simulation {
   private:
    // Methods parameters
    std::string method;         /**< Method to be used for simulation. */
    double time_step;           /**< Time step for simulation. */
    double time_interval;       /**< Time interval for simulation. */
    std::vector<State> results; /**< Vector containing the results of the
                                     simulation. */
    std::unique_ptr<BaseStepper> stepper; /**< Pointer to the stepper object. */

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
            stepper = std::make_unique<RK4Stepper>();
        } else if (method == "rk45") {
            stepper = std::make_unique<RK45Stepper>();
        }
        // else if (method == "ode113")
        // {
        //     /* code */
        // }
        else if (method == "euler") {
            stepper = std::make_unique<EulerStepper>();
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
    std::span<State> run(Model* h, State S0, std::span<State> res_ptr = {});

    /**
     * @brief Runs the simulation in parallel for multiple initial conditions.
     *
     * @param h Pointer to the Model object.
     * @param v_S0 Vector of initial states of the system.
     */
    std::vector<std::span<State>> run_parallel_ic(Model* h,
                                                  std::span<State> v_S0);

    void own_res(std::vector<State>& v_res) {
        /**
         * @brief Returns the results of the simulation.
         * Transfers ownership from the private member to the user vector.
         * User should handle runtime_exception if results is empty.
         *
         * @return std::vector<State> Vector containing the results of the
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

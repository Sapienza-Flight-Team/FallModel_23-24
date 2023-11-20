/**

 *  TODO: traslazione
 *  TODO: 3 waypoint
 *  TODO: costruire settings
 */
// 2.72, -0.26, 2.83
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>

#include "../include/pch.h"
#include "Waypoint.h"
double t_ap = 1.10;
double dt = 0.6;

double cd_load(State s, Real3 Vr, double t) {
    if (t < t_ap) {
        return 0;
    } else {
        return 0.825;
    }
}

double sur_load(State s, Real3 Vr, double t) {
    if (t < t_ap) {
        return 0;
    } else {
        return 0.825;
    }
}

constexpr double load_mass = 0.5;

Real3 wind_law(State state, Real3 pos, double t) { return Real3(0, 0, 0); }

ConFun f_stop = [](State state, State S0_dot, double t) {
    return (state.pos().z > 0);
};

int main() {
    PayChute pc(cd_load, sur_load, load_mass);
    Wind Vw(wind_law);
    BallisticModel bm(pc, Vw, f_stop);
    Simulation sim(0.01, 10, "");

    std::vector<State> res;
    State last_state;
    // Lancio 1

    GPS gps_1 = GPS(41.8317465, 12.3334762);
    GPS gps_2 = GPS(41.8313572, 12.3335520);
    GPS gps_3 = GPS(41.8312717, 12.3335901);
    double heading1 = 2.72;
    double heading2 = -0.26;
    double heading3 = 2.83;
    State S0(0, 0, -33.9, 22.2541, 1.05673, 0);
    sim.run(&bm, S0);
    res = sim.ret_res();
    last_state = res.back();

    double d1 = sqrt(last_state.pos().x * last_state.pos().x +
                     last_state.pos().y * last_state.pos().y);
    GPS gps_1_drop = translate_gps(gps_1, d1, heading1);

    // Lancio 2

    State S02(0, 0, -38.1, 29.1274, -0.13172, 0);
    sim.run(&bm, S02);
    res = sim.ret_res();
    last_state = res.back();

    double d2 = sqrt(last_state.pos().x * last_state.pos().x +
                     last_state.pos().y * last_state.pos().y);
    GPS gps_2_drop = translate_gps(gps_2, d2, heading2);

    // Lancio 3
    State S03(0, 0, -45.8, 20.5295, 1.01421, 0);
    sim.run(&bm, S03);
    res = sim.ret_res();
    last_state = res.back();

    double d3 = sqrt(last_state.pos().x * last_state.pos().x +
                     last_state.pos().y * last_state.pos().y);
    GPS gps_3_drop = translate_gps(gps_3, d3, heading3);

    std::cout << "d1: " << d1 << std::endl;
    std::cout << "GPS1: " << gps_1_drop.lat << " " << gps_1_drop.lon
              << std::endl;

    std::cout << "d2: " << d2 << std::endl;
    std::cout << "GPS2: " << gps_2_drop.lat << " " << gps_2_drop.lon
              << std::endl;
    std::cout << "d3: " << d3 << std::endl;
    std::cout << "GPS3: " << gps_3_drop.lat << " " << gps_3_drop.lon
              << std::endl;
}
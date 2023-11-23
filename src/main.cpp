#include "../include/pch.h"
















/**

 *  TODO: traslazione
 *  TODO: 3 waypoint
 *  TODO: costruire settings
 */
// 2.72, -0.26, 2.83


const double Cd = 0.825;
const double S = M_PI * pow(0.3, 2);  // 0.3 m radius
double cd_load(State s, Real3 Vr, double t) {
    if (t < 1) {
        // add cosine function of time
        return 0.5 * (1 - cos(3 * t)) * Cd;

    } else {
        return Cd;
    }
}

double sur_load(State s, Real3 Vr, double t) { return S; }

constexpr double load_mass = 0.5;
const double wind_mod = 1.38889;       // 4 km-> 1.02889 m/s
const double wind_dir = 300 * M_PI / 180;  // 300° in rads
const Real3 wind_speed =
    Real3(wind_mod * cos(wind_dir), wind_mod* sin((wind_dir)), 0);

// Wind 2knots@300 -> 1,02889 m/s,
Real3 wind_law(State state, Real3 pos, double t) { return wind_speed; }

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

    GPS gps_1_m = GPS(41.8318462, 12.3334944);
    GPS gps_2_m = GPS(41.8315882, 12.3337060);
    GPS gps_3_m = GPS(41.8312507, 12.3338636);

    double heading1 = 2.72;
    double heading2 = -0.26;
    double heading3 = 2.83;

    double dist1 = 11;
    double dist2 = 29;
    double dist3 = 23;

    State S0(0, 0, -33.9, 22.2541, 1.05673, 0);
    std::span<State> res_span = sim.run(&bm, S0);
    res = sim.ret_res();

    last_state = res_span.back();

    double d1 = sqrt(last_state.pos().x * last_state.pos().x +
                     last_state.pos().y * last_state.pos().y);
    GPS gps_1_drop = translate_gps(gps_1, d1, heading1);

    // Lancio 2

    State S02(0, 0, -38.1, 29.1274, -0.13172, 0);
    sim.run(&bm, S02);
    res = sim.ret_res();

    last_state = res_span.back();

    double d2 = sqrt(last_state.pos().x * last_state.pos().x +
                     last_state.pos().y * last_state.pos().y);
    GPS gps_2_drop = translate_gps(gps_2, d2, heading2);

    // Lancio 3
    State S03(0, 0, -45.8, 20.5295, 1.01421, 0);
    sim.run(&bm, S03);
    res = sim.ret_res();

    last_state = res_span.back();

    double d3 = sqrt(last_state.pos().x * last_state.pos().x +
                     last_state.pos().y * last_state.pos().y);
    GPS gps_3_drop = translate_gps(gps_3, d3, heading3);
    std::cout << "---------------------------------------------------"
              << std::endl;
    std::cout << "d1: " << d1
              << " absolute deviation from measure: " << abs(d1 - dist1)
              << std::endl;
    std::cout << "GPS1: " << gps_1_drop.lat << " " << gps_1_drop.lon
              << " absoulute deviation from measure "
              << abs(gps_1_drop.lat - gps_1_m.lat) << " "
              << abs(gps_1_drop.lon - gps_1_m.lon) << std::endl;
    std::cout << "---------------------------------------------------"
              << std::endl;

    std::cout << "d2: " << d2
              << " absolute deviation from measure: " << abs(d2 - dist2)
              << std::endl;
    std::cout << "GPS2: " << gps_2_drop.lat << " " << gps_2_drop.lon
              << " absoulute deviation from measure "
              << abs(gps_2_drop.lat - gps_2_m.lat) << " "
              << abs(gps_2_drop.lon - gps_2_m.lon) << std::endl;

    std::cout << "---------------------------------------------------"
              << std::endl;
    std::cout << "d3: " << d3
              << " absolute deviation from measure: " << abs(d3 - dist3)
              << std::endl;
    std::cout << "GPS3: " << gps_3_drop.lat << " " << gps_3_drop.lon
              << " absoulute deviation from measure "
              << abs(gps_3_drop.lat - gps_3_m.lat) << " "
              << abs(gps_3_drop.lon - gps_3_m.lon) << std::endl;
}
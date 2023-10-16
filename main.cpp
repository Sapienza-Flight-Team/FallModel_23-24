/**
 *  TODO: implementare RK
 *  TODO: traslazione
 *  TODO: 3 waypoint
 *  TODO: costruire settings
 */

#include "inc/pch.h"
#include "fstream"

double cd_payload(State s, Real3 Vr, double t) { return 0.47; }
double cd_parachute(State s, Real3 Vr, double t) { return 1.75; }
double sur_payload(State s, Real3 Vr, double t) { return 0.1257; }
double sur_parachute(State s, Real3 Vr, double t) { return 0.3491; }
double chute_mass = 0.116;
double load_mass = 1.15;

Real3 wind_law(State state, Real3 pos, double t) { return Real3(0, 0, 0); }

int main() {
    PayChute pc(cd_payload, sur_payload, load_mass, cd_parachute, sur_parachute,
                chute_mass);
    Wind Vw(wind_law);
    Model m(pc, Vw);
    State S0(0, 0, -40, 22, 0, 0);
    Simulation s(0.01, 10, "", m, S0);
    s.run();
    std::vector<State> res = s.retRes();

    std::ofstream ofile{"./res.csv", std::ios::trunc};
    for (auto it = res.begin(); it < res.end(); it++) {
        ofile << (*it) << std::endl;
    }
}
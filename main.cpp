/**
 *  TODO: implementare RK
 *  TODO: traslazione
 *  TODO: 3 waypoint
 *  TODO: costruire settings
 */

#include "inc/pch.h"
#include "fstream"

double Cd_const(State S, Real3 v, double t) { return 2.22; }
double zeroFun(State S, Real3 v, double t) { return 0; }
double Surface_const(State S, Real3 v, double t) { return 3.14/2; }

Real3 wind_law(State state, Real3 pos, double t) { return Real3(0, 0, 0); }

int main() {
    PayChute pc(Cd_const, zeroFun, 0.116, zeroFun, Surface_const, 0.5);
    Wind Vw(wind_law);
    Model m(pc, Vw);
    State S0 (0,0,-40,-7.1824, 19.7335, 0);
    Simulation s(0.01, 10, "", m, S0);
    s.run();
    std::vector<State> res = s.retRes();

    std::ofstream ofile{"./res.csv"};
    for (auto it = res.begin(); it < res.end(); it++) {
        ofile << (*it)  << std::endl;
    }
}
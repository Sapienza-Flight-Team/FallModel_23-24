/**

 *  TODO: traslazione
 *  TODO: 3 waypoint
 *  TODO: costruire settings
 */

#include <filesystem>
#include <fstream>

#include "../include/pch.h"

double cd_payload(State s, Real3 Vr, double t) { return 0.47; }
double cd_parachute(State s, Real3 Vr, double t) { return 1.75; }
double sur_payload(State s, Real3 Vr, double t) { return 0.1257; }
double sur_parachute(State s, Real3 Vr, double t) { return 0.3491; }
constexpr double chute_mass = 0.116;
constexpr double load_mass = 1.15;

Real3 wind_law(State state, Real3 pos, double t) { return Real3(0, 0, 0); }

std::vector<State> createVS0(size_t elements, State S0, double variation) {
    std::vector<State> v_S0;
    v_S0.reserve(elements);
    for (size_t i = 0; i < elements; i++) {
        State S;
        if (i % 2 == 0) {
            S = {S0.pos().x,
                 S0.pos().y,
                 S0.pos().z,
                 S0.vel().x + i * variation,
                 S0.vel().y + i * variation,
                 S0.vel().z + i * variation};
        } else {
            S = {S0.pos().x,
                 S0.pos().y,
                 S0.pos().z,
                 S0.vel().x - i * variation,
                 S0.vel().y - i * variation,
                 S0.vel().z - i * variation};
        }
        v_S0.push_back(S);
    }
    return v_S0;
}
bool cond_func(State S0, State S1, double t) { return S1.pos().z > 0; }

std::string fPath = {"../test/"};
std::string pPath = {"../test/para/"};

int main() {
    PayChute pc(cd_payload, sur_payload, load_mass, cd_parachute, sur_parachute,
                chute_mass);
    Wind Vw(wind_law);
    BallisticModel bm(pc, Vw);
    State S0(0, 0, -40, 22, 0, 0);
    Simulation s(0.1, 1, "");
    size_t n_steps = ceil(10 / 0.01) + 1;
    // {
    //     // Test auto allocation
    //     std::cout << "Auto allocation\n";
    //     s.run(&bm, S0);
    //     std::unique_ptr<State> res;
    //     s.own_res(res);
    //     // std::ofstream file("../test/test_auto.csv");
    //     // for (size_t i = 0; i < n_steps; i++) {
    //     //     file << res.get()[i] << "\n";
    //     // }
    //     std::cout << "End of auto allocation\n";
    // }
    // {
    //     // Test manual allocation
    //     std::cout << "Manual allocation\n";
    //     size_t n_steps = ceil(10 / 0.01) + 1;
    //     State* res = new State[n_steps];
    //     s.run(&bm, S0, res);
    //     // std::ofstream file("../test/test_manual.csv");

    //     // for (size_t i = 0; i < n_steps; i++) {
    //     //     file << res[i] << "\n";
    //     // }
    //     // std::cout << "End of manual allocation\n";
    // }
    // {
    //     // Test conditional propagation

    //     std::cout << "Conditional propagation\n";
    //     size_t n_taken = s.run_cond(&bm, S0, cond_func);
    //     std::unique_ptr<State> res;
    //     s.own_res(res);
    //     // std::ofstream file(fPath + "test_cond.csv");

    //     // for (size_t i = 0; i < n_taken; i++) {
    //     //     file << res.get()[i] << "\n";
    //     // }
    //     std::cout << "End of conditional propagation\n";
    // }
    {
        std::cout << "Parallel propagation\n";
        Simulation ps(0.1, 1, "");
        std::vector vS0 = createVS0(10, S0, 1);
        ps.run_parallel_ic(&bm, vS0);
        std::unique_ptr<State> res;
        ps.own_res(res);
        // Test parallel propagation
        size_t idx = 0;
        // for (auto& S : vS0) {
        //     std::string path =
        //         pPath + "test_parallel" + std::to_string(idx) + ".csv";
        //     std::ofstream ofile(path);

        //     for (size_t j = idx * n_steps; j < (idx + 1) * n_steps; j++) {
        //         ofile << res.get()[j] << "\n";
        //     }
        //     idx++;
        // }
        std::cout << "End of parallel propagation\n";
    }
}
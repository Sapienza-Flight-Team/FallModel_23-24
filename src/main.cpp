/**
 * TODO: Vectorization of state - Depends on model
 *
 */

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>




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

ConFun f_stop = [](State S0, State S0_dot, double t) {
    return S0_dot.pos().z > 0;
};

std::string fPath = {"../test/"};
std::string pPath = {"../test/para/"};

using namespace std::filesystem;
path p = path("../test/para/");

int main() {
    PayChute pc(cd_payload, sur_payload, load_mass, cd_parachute, sur_parachute,
                chute_mass);
    Wind Vw(wind_law);
    BallisticModel bm(pc, Vw, f_stop);
    State S0(0, 0, -40, 22, 0, 0);

    {
        size_t n_ic = 10000;  // 10k ic
        std::vector vS0 = createVS0(n_ic, S0, 0.05);

        std::cout << "--- Serial code --- n_iterations: " << n_ic << "\n";

        Simulation s(0.01, 10, "rk4");
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < n_ic; i++) {
            s.run(&bm, vS0[i]);
        }
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration =
            std::chrono::duration_cast<std::chrono::seconds>(stop - start);
        std::cout << "Elapsed time: " << duration.count() << " s\n";
        std::cout << "Time per run: " << std::fixed << std::setprecision(10)
                  << std::chrono::duration_cast<std::chrono::milliseconds>(
                         duration)
                             .count() /
                         n_ic
                  << "ms\n";

        std::vector<State> res;
        s.own_res(res);

        // std::ofstream file("../test/test_auto.csv");
        // for (size_t i = 0; i < res.size(); i++) {
        //     file << res[i] << "\n";
        // }
        std::cout << "--- End of serial code --- \n\n";
    }

    {
        size_t n_ic = 10000;  // 10k ic
        std::vector vS0 = createVS0(n_ic, S0, 0.05);
        std::cout << "--- Parallel propagation --- n_iterations: " << n_ic
                  << "\n";

        Simulation ps(0.01, 10, "");
        auto start = std::chrono::high_resolution_clock::now();
        std::vector<std::span<State>> res_span =
            ps.run_parallel_ic(&bm, vS0);
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration =
            std::chrono::duration_cast<std::chrono::seconds>(stop - start);
        std::cout << "Elapsed time: " << duration.count() << " s\n";

        std::cout << "Time per run: " << std::fixed << std::setprecision(10)
                  << std::chrono::duration_cast<std::chrono::milliseconds>(
                         duration)
                             .count() /
                         n_ic
                  << "ms\n";

        // // Clean result folder
        // for (auto& entry : directory_iterator(p)) {
        //     remove(entry.path());
        // }
        // Write results to file
        // size_t idx = 0;
        // for (auto& r_sp : res_span) {
        //     std::string path =
        //         pPath + "test_parallel" + std::to_string(idx) + ".csv";
        //     std::ofstream ofile(path);

        //     for (auto& state : r_sp) {
        //         ofile << state << "\n";
        //     }
        //     idx++;
        // }

        std::cout << "--- End of parallel propagation--- \n";
    }
    return 0;
}
/**
 * TODO: Vectorization of state - Depends on model
 *
 */

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>

#include "../include/pch.h"

const size_t dim3 = 3;

double cds_payload(const State<dim3>& s, double t) {
    return 0.47 * 0.1257 + 1.75 * 0.3491;
}

constexpr double load_mass = 1.15;

VReal3 wind_law(const State<dim3>& state, const VReal<dim3>& pos, double t) {
    return VReal<dim3>();
}

std::vector<State<dim3>> createVS0(size_t elements, State<dim3> S0,
                                   double variation) {
    std::vector<State<dim3>> v_S0;
    v_S0.reserve(elements);
    for (size_t i = 0; i < elements; i++) {
        State<dim3> S;
        if (i % 2 == 0) {
            S = S0 + State<dim3>{
                         0, 0, 0, i * variation, i * variation, i * variation};
        } else {
            S = S0 + State<dim3>{
                         0, 0, 0, i * variation, i * variation, i * variation};
        }
        v_S0.push_back(S);
    }
    return v_S0;
}

std::string fPath = {"../test/"};
std::string pPath = {"../test/para/"};

using namespace std::filesystem;
path p = path("../test/para/");

int main() {
    PayChute<dim3> pc(cds_payload, load_mass);
    Wind<dim3> Vw(wind_law);
    BallisticModel bm(pc, Vw);
    State<dim3> S0 = {0, 0, -40, 22, 0, 0};

    {
        size_t n_ic = 10000;  // 10k ic
        std::vector vS0 = createVS0(n_ic, S0, 0.05);

        std::cout << "--- Serial code --- n_iterations: " << n_ic << "\n";

        Simulation s(0.01, 10, "rk4");
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < n_ic; i++) {
            Results res = s.run(bm, vS0[i]);
          
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

        std::vector<State<dim3>> res;

        std::ofstream file("../test/test_auto.csv");
        for (size_t i = 0; i < res.size(); i++) {
            file << res[i] << "\n";
        }
        std::cout << "--- End of serial code --- \n\n";
    }

    {
        size_t n_ic = 10000;  // 10k ic
        std::vector vS0 = createVS0(n_ic, S0, 0.05);
        std::cout << "--- Parallel propagation --- n_iterations: " << n_ic
                  << "\n";

        Simulation ps(0.01, 10, "");
        auto start = std::chrono::high_resolution_clock::now();
        Results s_par = ps.run_parallel_ic(bm, {vS0});
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
        // // Write results to file
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
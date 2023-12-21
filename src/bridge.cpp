#include <pybind11/pybind11.h>
#include "pch.h"

namespace py = pybind11;

PYBIND11_MODULE(AirDrop, m) {
    py::class_<Simulation>(m, "Simulation")
        .def(py::init<double, double, std::string>())
        .def("run", [](Simulation& self, BallisticModel h, State<BallisticModel::getN()> S0) {
            return self.run<BallisticModel, BallisticModel::getN()>(h, S0);
        });
}
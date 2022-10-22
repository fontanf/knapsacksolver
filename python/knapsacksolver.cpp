#include "knapsacksolver/algorithms/algorithms.hpp"

#include <pybind11/pybind11.h>

namespace py = pybind11;

knapsacksolver::Solution solve(
        knapsacksolver::Instance& instance,
        std::string algorithm,
        bool verbose)
{
    std::mt19937_64 gen(0);
    optimizationtools::Info info = optimizationtools::Info()
        .set_verbosity_level(verbose)
        ;
    return run(algorithm, instance, gen, info).solution;
}

PYBIND11_MODULE(knapsacksolver, m)
{
    py::class_<knapsacksolver::Instance>(m, "Instance")
        .def(py::init<>())
        .def(py::init<std::string, std::string>(),
            py::arg("instance_path"),
            py::arg("format") = "standard")
        .def("set_capacity", &knapsacksolver::Instance::set_capacity)
        .def("add_item", &knapsacksolver::Instance::add_item);
    py::class_<knapsacksolver::Solution>(m, "Solution")
        .def("contains", &knapsacksolver::Solution::contains_idx)
        .def("number_of_items", &knapsacksolver::Solution::number_of_items)
        .def("profit", &knapsacksolver::Solution::profit)
        .def("weight", &knapsacksolver::Solution::weight);
    m.def("solve", &solve,
            py::arg("instance"),
            py::arg("algorithm") = "minknap",
            py::arg("verbose") = true);
}


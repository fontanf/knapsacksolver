#include "knapsacksolver/knapsack/algorithms/algorithms.hpp"

#include <pybind11/pybind11.h>

namespace py = pybind11;

knapsacksolver::knapsack::Solution solve(
        knapsacksolver::knapsack::Instance& instance,
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
    py::class_<knapsacksolver::knapsack::Instance>(m, "Instance")
        .def(py::init<>())
        .def(py::init<std::string, std::string>(),
            py::arg("instance_path"),
            py::arg("format") = "standard")
        .def("set_capacity", &knapsacksolver::knapsack::Instance::set_capacity)
        .def("add_item", &knapsacksolver::knapsack::Instance::add_item);
    py::class_<knapsacksolver::knapsack::Solution>(m, "Solution")
        .def("contains", &knapsacksolver::knapsack::Solution::contains_idx)
        .def("number_of_items", &knapsacksolver::knapsack::Solution::number_of_items)
        .def("profit", &knapsacksolver::knapsack::Solution::profit)
        .def("weight", &knapsacksolver::knapsack::Solution::weight);
    m.def("solve", &solve,
            py::arg("instance"),
            py::arg("algorithm") = "minknap",
            py::arg("verbose") = true);
}


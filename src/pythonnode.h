#ifndef PYTHONNODE_H
#define PYTHONNODE_H

#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/pybind11.h>
#include <behavior_tree_core/action_node.h>


namespace py = pybind11;
using BT::NodeParameters;
using BT::NodeStatus;
// clang-format off



PYBIND11_MODULE(BehaviorTreePython, m)
{
  py::enum_<NodeStatus>(m, "ScopedEnum", py::arithmetic())
          .value("SUCCESS", NodeStatus::SUCCESS)
          .value("FAILURE", NodeStatus::FAILURE)
          .value("RUNNING", NodeStatus::RUNNING)
          .value("IDLE",    NodeStatus::IDLE);
  //------------------------------------------
  pybind11::class_<BT::NodeParameters>(m, "NodeParameters")
      .def(py::init<>())
      .def("__contains__",
          [](const BT::NodeParameters &params, const std::string &key)
      {
              return params.find(key) != params.end();
      } )
      .def("__getitem__",
              [](NodeParameters &m, const std::string &k) -> const std::string & {
                  auto it = m.find(k);
                  if (it == m.end())
                    throw py::key_error();
                 return it->second;
              },
              py::return_value_policy::copy // ref + keepalive
          )
      .def("__len__", [](const NodeParameters &par) { return par.size(); })
      .def("__iter__",
             [](NodeParameters &par) { return pybind11::make_key_iterator(par.begin(), par.end()); },
             pybind11::keep_alive<0, 1>() /* Essential: keep list alive while iterator exists */
      );
  //------------------------------------------
}


// clang-format on


#endif // PYTHONNODE_H

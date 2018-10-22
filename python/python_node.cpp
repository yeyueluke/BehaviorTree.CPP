#include "python_node.h"

PYBIND11_EMBEDDED_MODULE(BehaviorTree, m)
{
  py::enum_<NodeStatus>(m, "NodeStatus", py::arithmetic())
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
              [](NodeParameters &params, const std::string &key) -> const std::string &
              {
                  auto it = params.find(key);
                  if (it == params.end()){
                    throw py::key_error();
                  }
                 return it->second;
              },
              py::return_value_policy::copy // ref + keepalive
          )
      .def("__len__", [](const NodeParameters &par) { return par.size(); })
      .def("__iter__",
             [](NodeParameters &par) { return pybind11::make_key_iterator(par.begin(), par.end()); },
             pybind11::keep_alive<0, 1>() /* Essential: keep list alive while iterator exists */
      );
}


#include "python_node.h"
#include  <list>

const std::string script_text = R"(

import BehaviorTree as BT

class HelloPythonAction:

    def __init__(self, name):
        self.name = name

    def tick(self):
        print("tick called from {}".format(self.name) )
        return BT.NodeStatus.SUCCESS

    def halt(self):
        print("halt called.")

    @staticmethod
    def requiredNodeParameters():
        return [( "paramA", "1" ), ( "paramB", "2" )]
 )";

int main()
{
    using namespace py::literals;
    py::scoped_interpreter guard{};

    py::exec( script_text );
    auto locals = py::dict("_instance_name"_a="myAction");

    py::exec( "_node_instance = HelloPythonAction(\"{_instance_name}\".format(**locals())) ",
              py::globals(), locals);

    py::exec( "_tick_result = _node_instance.tick()", py::globals(), locals );
    std::cout << locals["_tick_result"].cast<BT::NodeStatus>() << std::endl;

    py::exec( "_required_params = HelloPythonAction.requiredNodeParameters()", py::globals(), locals );
    auto params = locals["_required_params"].cast<std::list<std::pair<std::string, std::string>>>();

    for (const auto& p: params)
    {
        std::cout << p.first << " / " << p.second << std::endl;
    }

    return 0;
}

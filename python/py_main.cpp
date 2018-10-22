#include "python_node.h"

const std::string script_text = R"(

# define Action HelloPythonAction
import BehaviorTree as BT

class HelloPythonAction:

    def __init__(self, name):
        self.name = name

    def tick(self):
        print("tick called.")
        return BT.NodeStatus.SUCCESS

    def halt(self):
        print("halt called.")

    def requiredNodeParameters():
        return [( "paramA", "1" ), ( "paramB", "2" )]

myAction = HelloPythonAction("myAction")
print(myAction.tick())

 )";

int main()
{
  py::scoped_interpreter guard{};

  py::exec( script_text );

  return 0;
}

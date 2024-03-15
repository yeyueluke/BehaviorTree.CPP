// g++ main.cpp -I/home/hoo/github_src/BehaviorTree.CPP/include --std=c++17 -L/home/hoo/github_src/BehaviorTree.CPP/build -lbehaviortree_cpp
// run : export LD_LIBRARY_PATH=/home/hoo/github_src/BehaviorTree.CPP/build
#include "behaviortree_cpp/bt_factory.h"
#include "dummy_nodes.h"

static const char* xml_text = R"(
  <root BTCPP_format="4">
    <BehaviorTree>
        <Sequence>
        <Script code=" msg:='hello world' " />
        <Script code=" A:=THE_ANSWER; B:=3.14; color:=RED " />
            <Precondition if="A>B && color != BLUE" else="FAILURE">
            <Sequence>
                <SaySomething message="{A}"/>
                <SaySomething message="{B}"/>
                <SaySomething message="{msg}"/>
                <SaySomething message="{color}"/>
            </Sequence>
        </Precondition>
        </Sequence>
    </BehaviorTree>
  </root>
)";

enum Color
{
  RED = 1,
  BLUE = 2,
  GREEN = 3
};
int main()
{
  BehaviorTreeFactory factory;
  factory.registerNodeType<SaySomething>("SaySomething");

  // We can add these enums to the scripting language.
  // Check the limits of magic_enum
  factory.registerScriptingEnums<Color>();

  // Or we can manually assign a number to the label "THE_ANSWER".
  // This is not affected by any range limitation
  factory.registerScriptingEnum("THE_ANSWER", 42);

  auto tree = factory.createTreeFromText(xml_text);
  tree.tickWhileRunning();

  return 0;
}
/* output:
Robot says: 42.000000
Robot says: 3.140000
Robot says: hello world
Robot says: 1.000000
*/
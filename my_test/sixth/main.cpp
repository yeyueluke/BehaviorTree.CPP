// g++ main.cpp -I/home/hoo/github_src/BehaviorTree.CPP/include --std=c++17 -L/home/hoo/github_src/BehaviorTree.CPP/build -lbehaviortree_cpp
// run : export LD_LIBRARY_PATH=/home/hoo/github_src/BehaviorTree.CPP/build
#include "behaviortree_cpp/bt_factory.h"
#include "dummy_nodes.h"

static const char* xml_text = R"(
  <root BTCPP_format="4">
      <BehaviorTree ID="MainTree">
          <Sequence>
              <Script code=" move_goal='1;2;3' " />
              <SubTree ID="MoveRobot" target="{move_goal}" 
                                      result="{move_result}" />
              <SaySomething message="{move_result}"/>
          </Sequence>
      </BehaviorTree>

      <BehaviorTree ID="MoveRobot">
          <Fallback>
              <Sequence>
                  <MoveBase  goal="{target}"/>
                  <Script code=" result:='goal reached' " />
              </Sequence>
              <ForceFailure>
                  <Script code=" result:='error' " />
              </ForceFailure>
          </Fallback>
      </BehaviorTree>

  </root>
)";


int main()
{
  BT::BehaviorTreeFactory factory;

  factory.registerNodeType<SaySomething>("SaySomething");
  factory.registerNodeType<MoveBaseAction>("MoveBase");

  factory.registerBehaviorTreeFromText(xml_text);
  auto tree = factory.createTree("MainTree");

  // Keep ticking until the end
  tree.tickWhileRunning();

  // let's visualize some information about the current state of the blackboards.
  std::cout << "\n------ First BB ------" << std::endl;
  tree.subtrees[0]->blackboard->debugMessage();
  std::cout << "\n------ Second BB------" << std::endl;
  tree.subtrees[1]->blackboard->debugMessage();

  return 0;
}

/* Expected output:

------ First BB ------
move_result (std::string)
move_goal (Pose2D)

------ Second BB------
[result] remapped to port of parent tree [move_result]
[target] remapped to port of parent tree [move_goal]

*/

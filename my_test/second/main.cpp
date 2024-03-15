// g++ main.cpp -I/home/hoo/github_src/BehaviorTree.CPP/include --std=c++17 -L/home/hoo/github_src/BehaviorTree.CPP/build -lbehaviortree_cpp
// run : export LD_LIBRARY_PATH=/home/hoo/github_src/BehaviorTree.CPP/build
#include "behaviortree_cpp/bt_factory.h"

// file that contains the custom nodes definitions
#include "dummy_nodes.h"
//using namespace DummyNodes;

int main()
{
    // We use the BehaviorTreeFactory to register our custom nodes 
  BehaviorTreeFactory factory;
  factory.registerNodeType<SaySomething>("SaySomething");
  factory.registerNodeType<ThinkWhatToSay>("ThinkWhatToSay");

  auto tree = factory.createTreeFromFile("./my_tree.xml");
  tree.tickWhileRunning();
  
  return 0;
}

/* Expected output:
*
   Robot says: hello
  Robot says: The answer is 42
*/


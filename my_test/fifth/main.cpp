// g++ main.cpp -I/home/hoo/github_src/BehaviorTree.CPP/include --std=c++17 -L/home/hoo/github_src/BehaviorTree.CPP/build -lbehaviortree_cpp
// run : export LD_LIBRARY_PATH=/home/hoo/github_src/BehaviorTree.CPP/build
#include "behaviortree_cpp/bt_factory.h"
// #include "dummy_nodes.h"

static const char* xml_text = R"(
  <root BTCPP_format="4">

    <BehaviorTree ID="MainTree">
        <Sequence>
            <Fallback>
                <Inverter>
                    <IsDoorClosed/>
                </Inverter>
                <SubTree ID="DoorClosed"/>
            </Fallback>
            <PassThroughDoor/>
        </Sequence>
    </BehaviorTree>

    <BehaviorTree ID="DoorClosed">
        <Fallback>
            <OpenDoor/>
            <RetryUntilSuccessful num_attempts="5">
                <PickLock/>
            </RetryUntilSuccessful>
            <SmashDoor/>
        </Fallback>
    </BehaviorTree>
    
  </root>
)";
inline void SleepMS(int ms)
{
  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

using namespace BT;

class CrossDoor {
 public:
    void registerNodes(BT::BehaviorTreeFactory& factory);

    // SUCCESS if _door_open != true
    BT::NodeStatus isDoorClosed() {
        SleepMS(200);
        return !_door_open ? NodeStatus::SUCCESS : NodeStatus::FAILURE;
    }

    // SUCCESS if _door_open == true
    BT::NodeStatus passThroughDoor() {
        SleepMS(200);
        return !_door_open ? NodeStatus::SUCCESS : NodeStatus::FAILURE;
    }


    // After 3 attempts, will open a locked door
    BT::NodeStatus pickLock() {
        SleepMS(500);
        // succeed at 3rd attempt
        if (_pick_attempts++ > 3) {
            _door_locked = false;
            _door_open = true;
        }
        return _door_open ? NodeStatus::SUCCESS : NodeStatus::FAILURE;
    }

    // FAILURE if door locked
    BT::NodeStatus openDoor() {
        SleepMS(500);
        if (_door_locked) {
            return NodeStatus::FAILURE;
        } else {
            _door_open = true;
            return NodeStatus::SUCCESS;
        }
    }

    // WILL always open a door
    BT::NodeStatus smashDoor() {
        _door_locked = false;
        _door_open = true;
        // smash always works
        return NodeStatus::SUCCESS;
    }

private:
    bool _door_open   = false;
    bool _door_locked = true;
    int _pick_attempts = 0;
};

// Helper method to make registering less painful for the user
void CrossDoor::registerNodes(BT::BehaviorTreeFactory &factory)
{
  factory.registerSimpleCondition(
      "IsDoorClosed", std::bind(&CrossDoor::isDoorClosed, this));

  factory.registerSimpleAction(
      "PassThroughDoor", std::bind(&CrossDoor::passThroughDoor, this));

  factory.registerSimpleAction(
      "OpenDoor", std::bind(&CrossDoor::openDoor, this));

  factory.registerSimpleAction(
      "PickLock", std::bind(&CrossDoor::pickLock, this));

  factory.registerSimpleCondition(
      "SmashDoor", std::bind(&CrossDoor::smashDoor, this));
}

int main()
{
  BehaviorTreeFactory factory;

  CrossDoor cross_door;
  cross_door.registerNodes(factory);

  // In this example a single XML contains multiple <BehaviorTree>
  // To determine which one is the "main one", we should first register
  // the XML and then allocate a specific tree, using its ID

  factory.registerBehaviorTreeFromText(xml_text);
  auto tree = factory.createTree("MainTree");

  // helper function to print the tree
  printTreeRecursively(tree.rootNode());

  tree.tickWhileRunning();

  return 0;
}

// Copyright 2024 Intelligent Robotics Lab
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "cs4home_core/Core.hpp"
#include "cs4home_core/macros.hpp"
#include <memory>
#include <string>

#include "ament_index_cpp/get_package_share_directory.hpp"
#include "behaviortree_cpp_v3/behavior_tree.h"
#include "behaviortree_cpp_v3/bt_factory.h"
#include "behaviortree_cpp_v3/loggers/bt_zmq_publisher.h"
#include "behaviortree_cpp_v3/utils/shared_library.h"
#include "rclcpp/macros.hpp"
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_cascade_lifecycle/rclcpp_cascade_lifecycle.hpp"
#include "rclcpp_lifecycle/lifecycle_node.hpp"

using std::placeholders::_1;
using namespace std::chrono_literals;

/**
 * @class HumanAssistanceBT
 * @brief Core component execute the audio supervision behaviour.
 */
class HumanAssistanceBT : public cs4home_core::Core {
public:
  RCLCPP_SMART_PTR_DEFINITIONS(HumanAssistanceBT)

  /**
   * @brief Constructs an HumanAssistanceBT object and initializes the parent
   * lifecycle node.
   * @param parent Shared pointer to the lifecycle node managing this
   * HumanAssistanceBT instance.
   */

  explicit HumanAssistanceBT(rclcpp_lifecycle::LifecycleNode::SharedPtr parent)
      : Core("human_assistance_bt", parent) {
    RCLCPP_DEBUG(parent_->get_logger(), "Core created: [HumanAssistanceBT]");
  }

  /**
   * @brief Configures the HumanAssistanceBT component.
   * @return True if configuration is successful.
   */
  bool configure() override {
    RCLCPP_DEBUG(parent_->get_logger(), "[HumanAssistanceBT] configured");

    std::string pkgpath =
        ament_index_cpp::get_package_share_directory("bt_test");
    std::string xml_file = pkgpath + "/bt_xml/human_assistance.xml";

    factory_.registerFromPlugin(loader_.getOSName("attend_bt_node"));

    auto cascade_node =
        std::make_shared<rclcpp_cascade_lifecycle::CascadeLifecycleNode>(
            "human_assistance_cn");

    blackboard_ = BT::Blackboard::create();
    blackboard_->set("node", cascade_node);

    tree_ = factory_.createTreeFromFile(xml_file, blackboard_);

    publisher_zmq_ = std::make_shared<BT::PublisherZMQ>(tree_, 10, 1668, 1669);

    return true;
  }

  /**
   * @brief Activates the HumanAssistanceBT component by initializing the
   * behavior tree thread.
   *
   * @return True if activation is successful.
   */
  bool activate() override {
    bt_thread_ = std::thread(&HumanAssistanceBT::runBehaviorTree, this);
    return true;
  }

  /**
   * @brief Deactivates the node and ensures proper shutdown of the behavior
   * tree thread.
   *
   * It ensures that any running behavior tree thread is
   * properly joined to avoid leaving background tasks running or causing race
   * conditions.
   *
   * @return True to indicate successful deactivation.
   */
  bool deactivate() override {
    if (bt_thread_.joinable()) {
      bt_thread_.join();
    }
    return true;
  }

private:
  BT::BehaviorTreeFactory factory_;
  BT::SharedLibrary loader_;
  BT::Tree tree_;
  BT::Blackboard::Ptr blackboard_;
  std::shared_ptr<BT::PublisherZMQ> publisher_zmq_;
  std::thread bt_thread_;

  void runBehaviorTree() {
    rclcpp::Rate rate(10);
    bool finish = false;

    while (!finish && rclcpp::ok()) {
      finish = tree_.rootNode()->executeTick() != BT::NodeStatus::RUNNING;
      rate.sleep();
    }

    RCLCPP_INFO(parent_->get_logger(),
                "Behavior Tree finished, deactivating HumanAssistanceBT.");
    parent_->trigger_transition(
        lifecycle_msgs::msg::Transition::TRANSITION_DEACTIVATE);
  }
};

/// Registers the HumanAssistanceBT component with the ROS 2 class loader
CS_REGISTER_COMPONENT(HumanAssistanceBT)

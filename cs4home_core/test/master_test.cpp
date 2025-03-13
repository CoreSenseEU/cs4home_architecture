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

#include "ament_index_cpp/get_package_share_directory.hpp"

#include "lifecycle_msgs/msg/state.hpp"
#include "sensor_msgs/msg/image.hpp"

#include "cs4home_core/Flow.hpp"
#include "cs4home_core/Master.hpp"
#include "cs4home_core/CognitiveModule.hpp"
#include "gtest/gtest.h"


using namespace std::chrono_literals;

TEST(master_test, flow_creation)
{
  auto flow1 = cs4home_core::Flow::make_shared(
    "flow1", std::vector<std::string>({"A", "B", "C", "D"}));
  auto flow2 = cs4home_core::Flow::make_shared(
    "flow2", std::vector<std::string>({"A", "B", "C", "E"}));

  ASSERT_EQ(flow1->get_flow(), std::vector<std::string>({"A", "B", "C", "D"}));
  ASSERT_EQ(flow2->get_flow(), std::vector<std::string>({"A", "B", "C", "E"}));
}

TEST(master_test, master_creation)
{
 std::string pkgpath = ament_index_cpp::get_package_share_directory("cs4home_core");
  std::string config_file = pkgpath + "/config/startup_simple_1.yaml";

  rclcpp::NodeOptions options;
  options.arguments(
    {"--ros-args", "--params-file", config_file});

  auto master = cs4home_core::Master::make_shared("master_1", options);
  auto cm1 = cs4home_core::CognitiveModule::make_shared("cognitive_module_1", options);
  auto cm2 = cs4home_core::CognitiveModule::make_shared("cognitive_module_2", options);

  auto pub_node = rclcpp::Node::make_shared("pub_node");
  auto sub_node = rclcpp::Node::make_shared("sub_node");
  auto pub = pub_node->create_publisher<sensor_msgs::msg::Image>("/image", 100);
  std::vector<sensor_msgs::msg::Image> images;
  auto sub = sub_node->create_subscription<sensor_msgs::msg::Image>(
    "/detections2", 100, [&images](sensor_msgs::msg::Image msg) {
      images.push_back(msg);
    });

  rclcpp::executors::SingleThreadedExecutor exe;
  exe.add_node(master->get_node_base_interface());
  exe.add_node(cm1->get_node_base_interface());
  exe.add_node(cm2->get_node_base_interface());
  exe.add_node(pub_node);
  exe.add_node(sub_node);


  master->trigger_transition(lifecycle_msgs::msg::Transition::TRANSITION_CONFIGURE);
  cm1->trigger_transition(lifecycle_msgs::msg::Transition::TRANSITION_CONFIGURE);
  cm2->trigger_transition(lifecycle_msgs::msg::Transition::TRANSITION_CONFIGURE);
  ASSERT_EQ(master->get_current_state().id(), lifecycle_msgs::msg::State::PRIMARY_STATE_INACTIVE);
  ASSERT_EQ(cm1->get_current_state().id(), lifecycle_msgs::msg::State::PRIMARY_STATE_INACTIVE);
  ASSERT_EQ(cm2->get_current_state().id(), lifecycle_msgs::msg::State::PRIMARY_STATE_INACTIVE);

  const auto & flows = master->get_flows();
  ASSERT_EQ(flows.size(), 1u);

  ASSERT_NE(flows.find("flow_1"), flows.end());
  const auto & flow_1 = flows.at("flow_1");
  const auto & flow_1_cms = flow_1->get_flow();

  ASSERT_EQ(flow_1_cms, std::vector<std::string>({"cognitive_module_1", "cognitive_module_2"}));


  master->trigger_transition(lifecycle_msgs::msg::Transition::TRANSITION_ACTIVATE);
  ASSERT_EQ(master->get_current_state().id(), lifecycle_msgs::msg::State::PRIMARY_STATE_ACTIVE);

  {
    auto start = master->now();
    while (cm1->now() - start < 1s) {
      exe.spin_some();
    }
  }

  // They should be activated by master, as it is part of a flow
  ASSERT_EQ(cm1->get_current_state().id(), lifecycle_msgs::msg::State::PRIMARY_STATE_ACTIVE);
  ASSERT_EQ(cm2->get_current_state().id(), lifecycle_msgs::msg::State::PRIMARY_STATE_ACTIVE);

  sensor_msgs::msg::Image msg;
  for (int i = 0; i < 10; i++) {
    msg.header.frame_id = std::to_string(i);
    pub->publish(msg);
    exe.spin_some();
  }

  auto start = cm1->now();
  while (cm1->now() - start < 1s) {
    exe.spin_some();
  }

  master->trigger_transition(lifecycle_msgs::msg::Transition::TRANSITION_DEACTIVATE);
  ASSERT_EQ(master->get_current_state().id(), lifecycle_msgs::msg::State::PRIMARY_STATE_INACTIVE);
  ASSERT_EQ(cm1->get_current_state().id(), lifecycle_msgs::msg::State::PRIMARY_STATE_INACTIVE);
  ASSERT_EQ(cm2->get_current_state().id(), lifecycle_msgs::msg::State::PRIMARY_STATE_INACTIVE);

  ASSERT_EQ(images.size(), 10);
  for (int i = 0; i < 10; i++) {
    ASSERT_EQ(i * 4, std::atoi(images[i].header.frame_id.c_str()));
  }
}

/**
 * @brief Unit test for verifying the creation and structure of Flow instances.
 *
 * This test checks that `Flow` instances are created correctly with the expected
 * sequence of nodes, ensuring that the get_nodes() function returns the correct
 * vector of node names.
 */
TEST(flow_test, flow_creation)
{
  cs4home_core::Flow flow1("flow1", {"A", "B", "C", "D"});
  cs4home_core::Flow flow2("flow2", {"A", "B", "C", "E"});

  // Check that the nodes in flow1 match the expected sequence
  ASSERT_EQ(flow1.get_flow(), std::vector<std::string>({"A", "B", "C", "D"}));
  // Check that the nodes in flow2 match the expected sequence
  ASSERT_EQ(flow2.get_flow(), std::vector<std::string>({"A", "B", "C", "E"}));
}

/**
 * @brief Main function for running all GoogleTest unit tests.
 *
 * Initializes GoogleTest and ROS 2, then runs all tests defined in the executable.
 *
 * @param argc Argument count
 * @param argv Argument vector
 * @return int Test run result (0 if all tests passed, non-zero otherwise)
 */
int main(int argc, char ** argv)
{
  testing::InitGoogleTest(&argc, argv);
  rclcpp::init(argc, argv);

  return RUN_ALL_TESTS();
}

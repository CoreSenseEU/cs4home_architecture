// Copyright 2024 Intelligent Robotics Lab - Gentlebots
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

#include "bt_nodes/Attend.hpp"

#include <cstdint>
#include <string>
#include <utility>

#include "audio_common_msgs/action/tts.hpp"
#include "behaviortree_cpp_v3/behavior_tree.h"
#include "behaviortree_cpp_v3/bt_factory.h"
#include "std_msgs/msg/int8.hpp"
#include "std_msgs/msg/string.hpp"

using namespace std::chrono_literals;
using namespace std::placeholders;

Attend::Attend(const std::string &xml_tag_name,
               const BT::NodeConfiguration &conf)
    : BT::ActionNodeBase(xml_tag_name, conf) {
  config().blackboard->get("node", node_);

  this->publisher_ =
      node_->create_publisher<std_msgs::msg::String>("say_text", 10);

  this->publisher_->on_activate();

  this->client_ = rclcpp_action::create_client<audio_common_msgs::action::TTS>(
      node_, "/say");
}

void Attend::halt() { RCLCPP_INFO(node_->get_logger(), "Attend halted"); }

BT::NodeStatus Attend::tick() {
  RCLCPP_DEBUG(node_->get_logger(), "Attend ticked");
  if (status() == BT::NodeStatus::IDLE || !is_goal_sent_) {
    return on_idle();
  }

  return BT::NodeStatus::RUNNING;
}

BT::NodeStatus Attend::on_idle() {
  auto goal = audio_common_msgs::action::TTS::Goal();

  getInput("interaction_type", interaction_type_);

  if (interaction_type_.empty()) {
    return BT::NodeStatus::SUCCESS;
  }

  if (interaction_type_ != "emergency") {
    goal.text = "can I help you with that?";
  } else {
    getInput("event", event_);

    goal.text = "Quick, you have to leave!";
  }

  auto msg = std_msgs::msg::String();
  auto msg_dialog_action = std_msgs::msg::Int8();

  msg.data = goal.text;
  this->publisher_->publish(msg);

  RCLCPP_INFO(node_->get_logger(), "Sending goal");

  auto future_goal_handle = client_->async_send_goal(goal);
  if (rclcpp::spin_until_future_complete(node_->get_node_base_interface(),
                                         future_goal_handle) !=
      rclcpp::FutureReturnCode::SUCCESS) {

    RCLCPP_ERROR(node_->get_logger(), "send_goal failed");
    is_goal_sent_ = false;
    return BT::NodeStatus::FAILURE;
  }

  auto goal_handle = future_goal_handle.get();
  if (!goal_handle) {
    RCLCPP_ERROR(node_->get_logger(), "Goal was rejected by server");
    return BT::NodeStatus::FAILURE;
  }

  // Wait for the server to be done with the goal
  auto result_future = client_->async_get_result(goal_handle);

  RCLCPP_INFO(node_->get_logger(), "Waiting for result");
  if (rclcpp::spin_until_future_complete(node_->get_node_base_interface(),
                                         result_future) !=
      rclcpp::FutureReturnCode::SUCCESS) {
    RCLCPP_ERROR(node_->get_logger(), "get result call failed :(");
    return BT::NodeStatus::FAILURE;
  }

  auto wrapped_result = result_future.get();

  if (wrapped_result.code != rclcpp_action::ResultCode::SUCCEEDED) {
    RCLCPP_ERROR(node_->get_logger(), "Goal was rejected");
    return BT::NodeStatus::FAILURE;
  }

  is_goal_sent_ = true;

  return BT::NodeStatus::SUCCESS;
}

BT_REGISTER_NODES(factory) { factory.registerNodeType<Attend>("Attend"); }
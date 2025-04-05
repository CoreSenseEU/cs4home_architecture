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

#ifndef ATTEND_HPP_
#define ATTEND_HPP_

#include "BTActionNode.hpp"
#include "audio_common_msgs/action/tts.hpp"
#include "behaviortree_cpp_v3/behavior_tree.h"
#include "behaviortree_cpp_v3/bt_factory.h"
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_cascade_lifecycle/rclcpp_cascade_lifecycle.hpp"
#include "std_msgs/msg/string.hpp"
#include <algorithm>
#include <map>
#include <string>

class Attend : public BT::ActionNodeBase {
public:
  explicit Attend(const std::string &xml_tag_name,
                  const BT::NodeConfiguration &conf);

  void halt();
  BT::NodeStatus tick();

  static BT::PortsList providedPorts() {
    return BT::PortsList({BT::InputPort<std::string>("interaction_type"),
                          BT::InputPort<std::string>("event")});
  }

private:
  BT::NodeStatus on_idle();
  std::shared_ptr<rclcpp_cascade_lifecycle::CascadeLifecycleNode> node_;
  std::string interaction_type_ = "";
  std::string event_ = "";
  rclcpp_lifecycle::LifecyclePublisher<std_msgs::msg::String>::SharedPtr
      publisher_;
  std::shared_ptr<rclcpp_action::Client<audio_common_msgs::action::TTS>>
      client_;
  bool is_goal_sent_ = false;
};

#endif // ATTEND_HPP_

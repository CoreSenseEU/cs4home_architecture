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

#include "bt_nodes/DefineInteraction.hpp"

#include <limits>
#include <string>
#include <utility>

#include "behaviortree_cpp_v3/behavior_tree.h"

using namespace std::chrono_literals;
using namespace std::placeholders;

const std::unordered_set<std::string> DefineInteraction::emergency_events = {
    "Alarm_bell_ringing", "Running_water", "Frying"};

DefineInteraction::DefineInteraction(const std::string &xml_tag_name,
                                     const BT::NodeConfiguration &conf)
    : BT::ActionNodeBase(xml_tag_name, conf) {
  config().blackboard->get("node", node_);
}

BT::NodeStatus DefineInteraction::tick() {

  if (status() == BT::NodeStatus::IDLE) {
    RCLCPP_DEBUG(node_->get_logger(), "DefineInteraction idle");
  }

  RCLCPP_DEBUG(node_->get_logger(), "DefineInteraction ticked");

  getInput("event", event_);
  getInput("human_detected", human_detected_);

  if (!human_detected_ && emergency_events.count(event_)) {
    RCLCPP_DEBUG(node_->get_logger(), "no human detected y evento emergencia");
    interaction_type_ = "emergency";
  } else if (human_detected_) {
    interaction_type_ = "assist";
  } else {
    interaction_type_ = "";
  }

  setOutput("interaction_type", interaction_type_);

  RCLCPP_INFO(node_->get_logger(), "[DefineInteraction] Interaction: %s",
              interaction_type_.c_str());
  return BT::NodeStatus::SUCCESS;
}

void DefineInteraction::halt() {}

BT_REGISTER_NODES(factory) {
  factory.registerNodeType<DefineInteraction>("DefineInteraction");
}

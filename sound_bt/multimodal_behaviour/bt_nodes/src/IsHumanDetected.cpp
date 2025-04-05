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

#include "bt_nodes/IsHumanDetected.hpp"

#include <limits>
#include <string>
#include <utility>

#include "behaviortree_cpp_v3/behavior_tree.h"

using namespace std::chrono_literals;
using namespace std::placeholders;

IsHumanDetected::IsHumanDetected(const std::string &xml_tag_name,
                                 const BT::NodeConfiguration &conf)
    : BT::ActionNodeBase(xml_tag_name, conf) {
  config().blackboard->get("node", node_);

  detections_sub_ = node_->create_subscription<yolo_msgs::msg::DetectionArray>(
      "/yolo/detections", 20, std::bind(&IsHumanDetected::callback, this, _1));
}

BT::NodeStatus IsHumanDetected::tick() {

  rclcpp::spin_some(node_->get_node_base_interface());

  if (detection_buffer_.empty()) {
    RCLCPP_ERROR(node_->get_logger(),
                 "[IsHumanDetected] No detecciones en el buffer");
    return BT::NodeStatus::RUNNING;
  }

  // Recorrer las detecciones recientes
  for (const auto &detections : detection_buffer_) {
    for (const auto &detection : detections->detections) {
      if (detection.class_name == "person") {
        RCLCPP_INFO(node_->get_logger(), "[IsHumanDetected] Human detected");
        return BT::NodeStatus::SUCCESS;
      }
    }
  }

  RCLCPP_ERROR(node_->get_logger(),
               "[IsHumanDetected] No human detected en el buffer");
  return BT::NodeStatus::FAILURE;
}

void IsHumanDetected::halt() {
  detections_ = false;
  human_detected_ = false;
}

void IsHumanDetected::callback(
    const yolo_msgs::msg::DetectionArray::SharedPtr msg) {
  RCLCPP_INFO(node_->get_logger(), "[IsHumanDetected] detecciones yolo");
  detection_buffer_.push_back(msg);

  if (detection_buffer_.size() > BUFFER_SIZE) {
    detection_buffer_.pop_front();
  }
}

BT_REGISTER_NODES(factory) {
  factory.registerNodeType<IsHumanDetected>("IsHumanDetected");
}

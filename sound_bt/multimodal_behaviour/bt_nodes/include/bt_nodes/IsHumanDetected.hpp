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

#ifndef ISHUMANDETECTED_HPP_
#define ISHUMANDETECTED_HPP_

#include <algorithm>
#include <map>
#include <string>

#include "behaviortree_cpp_v3/behavior_tree.h"
#include "behaviortree_cpp_v3/bt_factory.h"
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_cascade_lifecycle/rclcpp_cascade_lifecycle.hpp"
#include "yolo_msgs/msg/detection_array.hpp"
#include <deque>

class IsHumanDetected : public BT::ActionNodeBase {
public:
  explicit IsHumanDetected(const std::string &xml_tag_name,
                           const BT::NodeConfiguration &conf);

  void halt();
  BT::NodeStatus tick();

  static BT::PortsList providedPorts() { return BT::PortsList({}); }

private:
  void callback(const yolo_msgs::msg::DetectionArray::SharedPtr msg);
  std::shared_ptr<rclcpp_cascade_lifecycle::CascadeLifecycleNode> node_;
  bool human_detected_ = false;
  bool detections_ = false;
  rclcpp::Subscription<yolo_msgs::msg::DetectionArray>::SharedPtr
      detections_sub_;
  std::deque<yolo_msgs::msg::DetectionArray::SharedPtr> detection_buffer_;
  static constexpr size_t BUFFER_SIZE = 5;
};

#endif // ISHUMANDETECTED_HPP_

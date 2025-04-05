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

#ifndef ISSOUNDDETECTED_HPP_
#define ISSOUNDDETECTED_HPP_

#include <algorithm>
#include <map>
#include <string>

#include "behaviortree_cpp_v3/behavior_tree.h"
#include "behaviortree_cpp_v3/bt_factory.h"
#include "geometry_msgs/msg/pose_stamped.hpp"
#include "geometry_msgs/msg/transform_stamped.hpp"
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_cascade_lifecycle/rclcpp_cascade_lifecycle.hpp"
#include "sound_msgs/msg/sound_detection.hpp"

#include <tf2/transform_datatypes.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#include <tf2_ros/buffer.h>
#include <tf2_ros/static_transform_broadcaster.h>
#include <tf2_ros/transform_listener.h>
#include <visualization_msgs/msg/marker.hpp>

class IsSoundDetected : public BT::ConditionNode {
public:
  explicit IsSoundDetected(const std::string &xml_tag_name,
                           const BT::NodeConfiguration &conf);

  BT::NodeStatus tick();

  static BT::PortsList providedPorts() {
    return BT::PortsList({BT::OutputPort<std::string>("sound_detected"),
                          BT::OutputPort<double>("source_angle"),
                          BT::OutputPort<double>("target_angle")});
  }

private:
  void sound_callback(const sound_msgs::msg::SoundDetection::SharedPtr msg);
  void publish_marker(const geometry_msgs::msg::PoseStamped &pose);
  double normalize(double angle);
  std::string sound_class_ = "";
  double source_angle_ = std::nan("");
  double target_angle_ = std::nan("");
  std::shared_ptr<rclcpp_cascade_lifecycle::CascadeLifecycleNode> node_;
  std::shared_ptr<tf2_ros::TransformListener> tf_listener_{nullptr};
  std::shared_ptr<tf2_ros::Buffer> tf_buffer_;
  rclcpp::Subscription<sound_msgs::msg::SoundDetection>::SharedPtr sound_sub_;
  rclcpp::Publisher<visualization_msgs::msg::Marker>::SharedPtr publisher_;
};

#endif // ISSOUNDDETECTED_HPP_

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

#include "bt_nodes/IsAudioLocated.hpp"

#include <limits>
#include <string>
#include <utility>

#include "behaviortree_cpp_v3/behavior_tree.h"
#include <cmath>
#include <tf2/LinearMath/Matrix3x3.h>
#include <tf2/LinearMath/Quaternion.h>
#include <visualization_msgs/msg/marker.hpp>

using namespace std::chrono_literals;
using namespace std::placeholders;

double
extract_yaw_from_pose(const geometry_msgs::msg::Quaternion &orientation) {
  double roll, pitch, yaw;

  tf2::Quaternion doa_quat(orientation.x, orientation.y, orientation.z,
                           orientation.w);

  tf2::Matrix3x3 rot_matrix(doa_quat);

  rot_matrix.getRPY(roll, pitch, yaw);

  return yaw;
}

IsAudioLocated::IsAudioLocated(const std::string &xml_tag_name,
                               const BT::NodeConfiguration &conf)
    : BT::ConditionNode(xml_tag_name, conf) {
  config().blackboard->get("node", node_);

  tf_buffer_ = std::make_unique<tf2_ros::Buffer>(node_->get_clock());
  tf_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);
}

BT::NodeStatus IsAudioLocated::tick() {

  rclcpp::spin_some(node_->get_node_base_interface());

  if (status() == BT::NodeStatus::IDLE) {
    RCLCPP_DEBUG(node_->get_logger(), "IsAudioLocated idle");
  }

  geometry_msgs::msg::TransformStamped transform_stamped;
  try {
    transform_stamped =
        tf_buffer_->lookupTransform("map", "base_link", tf2::TimePointZero);
  } catch (tf2::TransformException &ex) {
    RCLCPP_INFO(node_->get_logger(),
                "[IsAudioLocated] Could not transform base_link to map: %s",
                ex.what());

    return BT::NodeStatus::FAILURE;
  }

  getInput("target_angle", target_angle_);

  double yaw_robot =
      extract_yaw_from_pose(transform_stamped.transform.rotation);
  double error = normalize(target_angle_ - yaw_robot);

  if (std::abs(error) < ANGLE_TOLERANCE) {
    RCLCPP_INFO(node_->get_logger(),
                "[IsAudioLocated] Robot correctly oriented");
    return BT::NodeStatus::SUCCESS;
  } else {
    RCLCPP_INFO(node_->get_logger(), "[IsAudioLocated] Robot not oriented");
    double angle_to_rotate = target_angle_ - yaw_robot;
    source_angle_ = normalize(angle_to_rotate);

    setOutput("source_angle", source_angle_);
    RCLCPP_INFO(node_->get_logger(),
                "[IsSoundDetected] Angle to sound source: %f", source_angle_);
    return BT::NodeStatus::FAILURE;
  }
}

double IsAudioLocated::normalize(double angle) {
  while (angle > M_PI) {
    angle -= 2 * M_PI;
  }
  while (angle < -M_PI) {
    angle += 2 * M_PI;
  }
  return angle;
}

BT_REGISTER_NODES(factory) {
  factory.registerNodeType<IsAudioLocated>("IsAudioLocated");
}

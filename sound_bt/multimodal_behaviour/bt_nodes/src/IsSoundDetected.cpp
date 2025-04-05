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

#include "bt_nodes/IsSoundDetected.hpp"

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

IsSoundDetected::IsSoundDetected(const std::string &xml_tag_name,
                                 const BT::NodeConfiguration &conf)
    : BT::ConditionNode(xml_tag_name, conf) {
  config().blackboard->get("node", node_);

  tf_buffer_ = std::make_unique<tf2_ros::Buffer>(node_->get_clock());
  tf_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);
  publisher_ = node_->create_publisher<visualization_msgs::msg::Marker>(
      "visualization_marker", 10);
  sound_sub_ = node_->create_subscription<sound_msgs::msg::SoundDetection>(
      "/sound_detection", 20,
      std::bind(&IsSoundDetected::sound_callback, this, _1));
}

void IsSoundDetected::publish_marker(
    const geometry_msgs::msg::PoseStamped &pose) {
  auto marker = visualization_msgs::msg::Marker();
  marker.header.frame_id = "map"; // Cambia según el frame de tu sistema
  marker.header.stamp = node_->get_clock()->now();
  marker.ns = "basic_shapes";
  marker.id = 0;
  marker.type = visualization_msgs::msg::Marker::CUBE;
  marker.action = visualization_msgs::msg::Marker::ADD;

  // Posición del marcador
  marker.pose.position.x = pose.pose.position.x;
  marker.pose.position.y = pose.pose.position.y;
  marker.pose.position.z = pose.pose.position.z;
  marker.pose.orientation.x = pose.pose.orientation.x;
  marker.pose.orientation.y = pose.pose.orientation.y;
  marker.pose.orientation.z = pose.pose.orientation.z;
  marker.pose.orientation.w = pose.pose.orientation.w;

  // Escala del marcador
  marker.scale.x = 0.5;
  marker.scale.y = 0.5;
  marker.scale.z = 0.5;

  // Color del marcador (rojo con 50% de transparencia)
  marker.color.r = 1.0;
  marker.color.g = 0.0;
  marker.color.b = 0.0;
  marker.color.a = 0.5;

  marker.lifetime =
      rclcpp::Duration::from_seconds(0); // 0 significa que no desaparece

  publisher_->publish(marker);
  RCLCPP_INFO(node_->get_logger(),
              "Publicado marcador en /visualization_marker");
}

BT::NodeStatus IsSoundDetected::tick() {

  rclcpp::spin_some(node_->get_node_base_interface());

  if (status() == BT::NodeStatus::IDLE) {
    RCLCPP_DEBUG(node_->get_logger(), "IsSoundDetected idle");
  }

  RCLCPP_DEBUG(node_->get_logger(), "IsSoundDetected ticked");
  if (sound_class_.empty() && isnan(target_angle_)) {
    RCLCPP_WARN(node_->get_logger(), "[IsSoundDetected] No detections");
    return BT::NodeStatus::FAILURE;
  }

  setOutput("sound_detected", sound_class_);
  // setOutput("source_angle", source_angle_);
  setOutput("target_angle", target_angle_);

  RCLCPP_INFO(node_->get_logger(), "[IsSoundDetected] Detections published");
  return BT::NodeStatus::SUCCESS;
}

void IsSoundDetected::sound_callback(
    const sound_msgs::msg::SoundDetection::SharedPtr msg) {

  publish_marker(msg->sound_location);

  geometry_msgs::msg::TransformStamped transform_stamped;
  rclcpp::Time when = node_->get_clock()->now();
  try {
    transform_stamped =
        tf_buffer_->lookupTransform("map", "base_link", tf2::TimePointZero);
  } catch (tf2::TransformException &ex) {
    RCLCPP_INFO(node_->get_logger(),
                "[IsSoundDetected] Could not transform base_link to map: %s",
                ex.what());

    return;
  }

  sound_class_ = msg->class_name;
  RCLCPP_INFO(node_->get_logger(), "[IsSoundDetected] sound: %s",
              sound_class_.c_str());

  double target_x = msg->sound_location.pose.position.x;
  double target_y = msg->sound_location.pose.position.y;

  double robot_x = transform_stamped.transform.translation.x;
  double robot_y = transform_stamped.transform.translation.y;

  double yaw_robot =
      extract_yaw_from_pose(transform_stamped.transform.rotation);

  double delta_x = target_x - robot_x;
  double delta_y = target_y - robot_y;
  target_angle_ = std::atan2(delta_y, delta_x);

  // double angle_to_rotate = target_angle_ - yaw_robot;
  // angle_to_rotate = normalize(angle_to_rotate);
  // source_angle_ = angle_to_rotate;
  // RCLCPP_INFO(node_->get_logger(),
  //             "[IsSoundDetected] Angle to sound source: %f",
  //             angle_to_rotate);
}

double IsSoundDetected::normalize(double angle) {
  while (angle > M_PI) {
    angle -= 2 * M_PI;
  }
  while (angle < -M_PI) {
    angle += 2 * M_PI;
  }
  return angle;
}

BT_REGISTER_NODES(factory) {
  factory.registerNodeType<IsSoundDetected>("IsSoundDetected");
}

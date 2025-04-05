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

#include "audio_common_msgs/msg/audio_data.hpp"
#include "geometry_msgs/msg/pose_stamped.hpp"
#include "sound_msgs/msg/sound_detection.hpp"
#include "sound_msgs/msg/sound_event_detection.hpp"
#include "std_msgs/msg/bool.hpp"
#include "yolo_msgs/msg/detection_array.hpp"

#include "rclcpp/macros.hpp"
#include "rclcpp_lifecycle/lifecycle_node.hpp"
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#include <tf2_ros/buffer.h>
#include <tf2_ros/transform_broadcaster.h>
#include <tf2_ros/transform_listener.h>

using std::placeholders::_1;
using namespace std::chrono_literals;

/**
 * @class SoundContextEvaluation
 * @brief Core component that process incoming audio messages.
 */
class SoundContextEvaluation : public cs4home_core::Core {
public:
  RCLCPP_SMART_PTR_DEFINITIONS(SoundContextEvaluation)

  /**
   * @brief Constructs an SoundContextEvaluation object and initializes the
   * parent lifecycle node.
   * @param parent Shared pointer to the lifecycle node managing this
   * SoundContextEvaluation instance.
   */

  explicit SoundContextEvaluation(
      rclcpp_lifecycle::LifecycleNode::SharedPtr parent)
      : Core("sound_context_evaluation", parent) {
    RCLCPP_DEBUG(parent_->get_logger(),
                 "Core created: [SoundContextEvaluation]");
  }

  /**
   * @brief Timer callback function that retrieves an sound detection and
   * processes it.
   *
   * This function is called periodically and attempts to retrieve an sound
   * detection message from the afferent component. If a message is received, it
   * evaluate the context of the situation.
   */
  void timer_callback() {

    auto img_detections = afferent_->get_msg<yolo_msgs::msg::DetectionArray>(0);
    auto sound_detections =
        afferent_->get_msg<sound_msgs::msg::SoundDetection>(1);
    bool human_detected = false;

    if (img_detections && sound_detections) {
      RCLCPP_INFO(parent_->get_logger(), "img and sound detections");

      for (int i = 0; i < size(img_detections->detections); i++) {
        RCLCPP_INFO(parent_->get_logger(), "[SoundContextEvaluation] class: %s",
                    img_detections->detections[i].class_name.c_str());
        if (img_detections->detections[i].class_name == "person") {
          human_detected = true;
          RCLCPP_INFO(parent_->get_logger(),
                      "[SoundContextEvaluation] Human detected");
        }
      }

      auto controlled_sound = std::make_shared<std_msgs::msg::Bool>();

      if (sound_detections->type == "supervised" && human_detected) {
        controlled_sound->data = true;
      } else {
        controlled_sound->data = false;
      }

      efferent_->publish(0, controlled_sound);
      RCLCPP_INFO(parent_->get_logger(),
                  "Deactivating SoundContextEvaluation.");
      parent_->trigger_transition(
          lifecycle_msgs::msg::Transition::TRANSITION_DEACTIVATE);
    } else {
      RCLCPP_WARN(parent_->get_logger(),
                  "[SoundContextEvaluation] No detections");
    }
  }

  /**
   * @brief Configures the SoundContextEvaluation component.
   * @return True if configuration is successful.
   */
  bool configure() override {
    RCLCPP_DEBUG(parent_->get_logger(), "Core configured");
    return true;
  }

  /**
   * @brief Activates the SoundContextEvaluation component by initializing a
   * timer.
   *
   * The timer is set to call `timer_callback` every 50 milliseconds.
   *
   * @return True if activation is successful.
   */
  bool activate() override {
    timer_ = parent_->create_wall_timer(
        50ms, std::bind(&SoundContextEvaluation::timer_callback, this));
    return true;
  }

  /**
   * @brief Deactivates the SoundContextEvaluation component by disabling the
   * timer.
   *
   * The timer is reset to null, stopping periodic message processing.
   *
   * @return True if deactivation is successful.
   */
  bool deactivate() override {
    timer_ = nullptr;
    return true;
  }

private:
  rclcpp::TimerBase::SharedPtr
      timer_; /**< Timer for periodic execution of `timer_callback`. */
};

/// Registers the SoundContextEvaluation component with the ROS 2 class loader
CS_REGISTER_COMPONENT(SoundContextEvaluation)

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

#include "rclcpp/macros.hpp"
#include "rclcpp_lifecycle/lifecycle_node.hpp"
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#include <tf2_ros/buffer.h>
#include <tf2_ros/transform_broadcaster.h>
#include <tf2_ros/transform_listener.h>

using std::placeholders::_1;
using namespace std::chrono_literals;

/**
 * @class SoundRecognition
 * @brief Core component that process incoming audio messages.
 */
class SoundRecognition : public cs4home_core::Core {
public:
  RCLCPP_SMART_PTR_DEFINITIONS(SoundRecognition)

  /**
   * @brief Constructs an SoundRecognition object and initializes the parent
   * lifecycle node.
   * @param parent Shared pointer to the lifecycle node managing this
   * SoundRecognition instance.
   */

  explicit SoundRecognition(rclcpp_lifecycle::LifecycleNode::SharedPtr parent)
      : Core("sound_recognition", parent) {
    RCLCPP_DEBUG(parent_->get_logger(), "Core created: [SoundRecognition]");
    this->led_frames_ = {"led_1", "led_10", "led_11", "led_12",
                         "led_2", "led_3",  "led_4",  "led_5",
                         "led_6", "led_7",  "led_8",  "led_9"};
    this->mic_frames_ = {"mic_1", "mic_2", "mic_3", "mic_4"};
    this->tf_buffer_ = std::make_shared<tf2::BufferCore>();
    this->tf_listener_ =
        std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);
    this->tf_broadcaster_ =
        std::make_shared<tf2_ros::TransformBroadcaster>(parent_);
    this->last_check_ = parent_->get_clock()->now();
  }

  void process_audio_data(
      std::shared_ptr<geometry_msgs::msg::PoseStamped> doa_msg,
      std::shared_ptr<sound_msgs::msg::SoundEventDetection> sed_msg) {

    double yaw_source = extract_yaw_from_pose(doa_msg->pose.orientation);

    int closest_microphone = 0;
    double closest_angle = std::numeric_limits<double>::max();

    geometry_msgs::msg::TransformStamped transform;
    double mic_yaw;

    for (int i = 0; i < this->mic_frames_.size(); i++) {

      try {
        transform = this->tf_buffer_->lookupTransform(
            "mic_array_link", this->mic_frames_[i], tf2::TimePointZero);

        double yaw = extract_yaw_from_pose(transform.transform.rotation);

        // Calcular la diferencia angular entre el DOA y el micro
        double angle_diff = std::abs(yaw_source - yaw);

        if (angle_diff < closest_angle) {
          closest_angle = angle_diff;
          closest_microphone = i + 1;
          mic_yaw = yaw;
        }
      } catch (tf2::TransformException &ex) {
        RCLCPP_WARN(parent_->get_logger(),
                    "Error getting transformation %s: %s",
                    this->mic_frames_[i].c_str(), ex.what());
      }
    }

    RCLCPP_INFO(parent_->get_logger(), "Nearest source: mic_%d",
                closest_microphone);

    auto now = parent_->get_clock()->now();

    if (closest_microphone == this->last_mic_checked_) {
      auto diff_time = (now - this->last_check_).seconds();

      if (diff_time >= this->INTERVAL_TIME) {
        RCLCPP_INFO(parent_->get_logger(),
                    "[SoundRecognition] Hear new sound.");

        double dx = this->SOUND_DISTANCE * std::cos(mic_yaw);
        double dy = this->SOUND_DISTANCE * std::sin(mic_yaw);

        RCLCPP_INFO(parent_->get_logger(),
                    "[SoundRecognition] Event detection: %s",
                    sed_msg->class_name.c_str());

        geometry_msgs::msg::TransformStamped transform_mic_to_map;

        try {
          transform_mic_to_map = this->tf_buffer_->lookupTransform(
              "map", "mic_array_link", tf2::TimePointZero);
        } catch (const tf2::TransformException &ex) {
          RCLCPP_ERROR(parent_->get_logger(), "Transform not found: %s",
                       ex.what());
          return;
        }

        geometry_msgs::msg::PoseStamped sound_location;
        sound_location.header.stamp = parent_->get_clock()->now();
        sound_location.header.frame_id = "mic_array_link";

        // Posición relativa al micrófono
        sound_location.pose.position.x = dx;
        sound_location.pose.position.y = dy;
        sound_location.pose.position.z = 0.0;
        sound_location.pose.orientation.w = 1.0;

        // Transformar la posición del sonido de "mic_array_link" a "map"
        geometry_msgs::msg::PoseStamped sound_location_in_map;
        tf2::doTransform(sound_location, sound_location_in_map,
                         transform_mic_to_map);

        // Actualizar el mensaje de detección del sonido con la posición
        // transformada
        std::shared_ptr<sound_msgs::msg::SoundDetection> sound_detection;
        sound_detection->sound_location = sound_location_in_map;

        sound_detection->class_name = sed_msg->class_name;
        sound_detection->class_id = sed_msg->class_id;

        // this->tf_broadcaster_->sendTransform(sound_source_transform);

        efferent_->publish(0, sound_detection);

        this->last_check_ = now;
      }
    } else {
      this->last_mic_checked_ = closest_microphone;
      this->last_check_ = now;
      RCLCPP_WARN(parent_->get_logger(), "[SoundRecognition] Not new sound");
    }
  }

  double
  extract_yaw_from_pose(const geometry_msgs::msg::Quaternion &orientation) {
    double roll, pitch, yaw;

    tf2::Quaternion doa_quat(orientation.x, orientation.y, orientation.z,
                             orientation.w);

    tf2::Matrix3x3 rot_matrix(doa_quat);

    rot_matrix.getRPY(roll, pitch, yaw);

    return yaw;
  }

  /**
   * @brief Timer callback function that retrieves an audio message and
   * processes it.
   *
   * This function is called periodically and attempts to retrieve an audio
   * message from the afferent component. If a message is received, it is
   * passed to `process_audio`.
   */
  void timer_callback() {
    // RCLCPP_INFO(parent_->get_logger(), "Audio will be processed");
    auto msg_audio = afferent_->get_msg<audio_common_msgs::msg::AudioData>(0);
    auto msg_doa = afferent_->get_msg<geometry_msgs::msg::PoseStamped>(7);
    auto msg_sed = afferent_->get_msg<sound_msgs::msg::SoundEventDetection>(8);

    if (msg_doa && msg_sed) {
      RCLCPP_INFO(parent_->get_logger(), "DOA y SED");
      process_audio_data(msg_doa, msg_sed);
    }
  }

  /**
   * @brief Configures the SoundRecognition component.
   * @return True if configuration is successful.
   */
  bool configure() override {
    RCLCPP_DEBUG(parent_->get_logger(), "Core configured");
    return true;
  }

  /**
   * @brief Activates the SoundRecognition component by initializing a timer.
   *
   * The timer is set to call `timer_callback` every 50 milliseconds.
   *
   * @return True if activation is successful.
   */
  bool activate() override {
    timer_ = parent_->create_wall_timer(
        50ms, std::bind(&SoundRecognition::timer_callback, this));
    return true;
  }

  /**
   * @brief Deactivates the SoundRecognition component by disabling the timer.
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
  std::vector<std::string> mic_frames_;
  std::vector<std::string> led_frames_;
  std::shared_ptr<tf2::BufferCore> tf_buffer_;
  std::shared_ptr<tf2_ros::TransformListener> tf_listener_;
  std::shared_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_;
  int last_mic_checked_ = 0;
  rclcpp::Time last_check_;
  const double INTERVAL_TIME = 2.0;
  const double SOUND_DISTANCE = 2.0;
};

/// Registers the SoundRecognition component with the ROS 2 class loader
CS_REGISTER_COMPONENT(SoundRecognition)

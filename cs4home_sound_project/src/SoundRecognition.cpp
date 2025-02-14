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

#include "rclcpp/macros.hpp"
#include "rclcpp_lifecycle/lifecycle_node.hpp"

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
      : Core(parent) {
    RCLCPP_DEBUG(parent_->get_logger(), "Core created: [SoundRecognition]");
  }

  void process_audio(std::shared_ptr<audio_common_msgs::msg::AudioData> msg) {
    if (!msg || msg->uint8_data.empty()) {
      RCLCPP_ERROR(parent_->get_logger(),
                   "Received empty or null audio message!");
      return;
    }
    RCLCPP_INFO(parent_->get_logger(), "Audio is processing");
    RCLCPP_INFO(parent_->get_logger(), "Processing audio message of size: %zu",
                msg->uint8_data.size());

    std::vector<float> float_audio(1024);
    for (size_t i = 0; i < msg->uint8_data.size(); ++i) {
      if (msg->uint8_data[i] > 255) {
        RCLCPP_ERROR(parent_->get_logger(),
                     "Invalid audio data at index %zu: %d", i,
                     msg->uint8_data[i]);
      }
      float_audio[i] = static_cast<float>(msg->uint8_data[i]) / 128.0f;
    }
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
    RCLCPP_INFO(parent_->get_logger(), "Audio will be processed");
    auto msg = afferent_->get_msg<audio_common_msgs::msg::AudioData>();
    if (msg != nullptr) {
      process_audio(std::move(msg));
      RCLCPP_INFO(parent_->get_logger(), "Audio processed");
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
};

/// Registers the SoundRecognition component with the ROS 2 class loader
CS_REGISTER_COMPONENT(SoundRecognition)

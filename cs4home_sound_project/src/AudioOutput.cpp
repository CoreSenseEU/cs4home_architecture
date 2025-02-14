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

#include "cs4home_core/Efferent.hpp"
#include "cs4home_core/macros.hpp"

#include "sound_msgs/msg/sound_detection.hpp"

#include "rclcpp/macros.hpp"
#include "rclcpp_lifecycle/lifecycle_node.hpp"

/**
 * @class AudioOutput
 * @brief Manages sound output by creating publishers for specified topics and
 *        providing a method to publish sound messages.
 */
class AudioOutput : public cs4home_core::Efferent {
public:
  RCLCPP_SMART_PTR_DEFINITIONS(AudioOutput)

  /**
   * @brief Constructs a AudioOutput object and declares necessary
   * parameters.
   * @param parent Shared pointer to the lifecycle node managing this
   * AudioOutput instance.
   */
  explicit AudioOutput(rclcpp_lifecycle::LifecycleNode::SharedPtr parent)
      : Efferent(parent) {
    RCLCPP_DEBUG(parent_->get_logger(), "Afferent created: [AudioOutput]");

    // Declares the parameter for output topics.
    parent_->declare_parameter("audio_output.topics", output_topic_names_);
  }

  /**
   * @brief Configures the AudioOutput by creating publishers for each
   * specified topic.
   *
   * This method retrieves the topic names from the parameter server and
   * attempts to create a publisher for each topic to publish
   * `sound_msgs::msg::SoundDetection` messages.
   *
   * @return True if all publishers are created successfully.
   */
  bool configure() {
    parent_->get_parameter("audio_output.topics", output_topic_names_);

    for (size_t i = 0; i < output_topic_names_.size(); i++) {
      if (create_publisher(output_topic_names_[i],
                           "sound_msgs/msg/SoundDetection")) {
        RCLCPP_INFO(parent_->get_logger(),
                    "[AudioOutput] created publisher to [%s, "
                    "sound_msgs/msg/SoundDetection]",
                    output_topic_names_[i].c_str());
      } else {
        RCLCPP_WARN(parent_->get_logger(),
                    "[AudioOutput] Couldn't create publisher to [%s, "
                    "sound_msgs/msg/SoundDetection]",
                    output_topic_names_[i].c_str());
      }
    }
    return true;
  }

  /**
   * @brief Publishes an sound recognition message to all configured topics.
   * @param msg shared pointer to an sound recognition message of type
   * `sound_msgs/msg/SoundDetection`.
   */
  void publish_sound_detection(
      std::shared_ptr<sound_msgs::msg::SoundDetection> msg) {
    publish(std::move(msg));
  }

private:
  std::vector<std::string> output_topic_names_; /**< List of output topics to
                                                   publish sound detection. */
};

/// Registers the AudioOutput component with the ROS 2 class loader
CS_REGISTER_COMPONENT(AudioOutput)

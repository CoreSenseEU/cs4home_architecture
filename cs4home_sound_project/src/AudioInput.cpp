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

#include "cs4home_core/Afferent.hpp"
#include "cs4home_core/macros.hpp"

#include "audio_common_msgs/msg/audio_data.hpp"

#include "rclcpp/macros.hpp"
#include "rclcpp_lifecycle/lifecycle_node.hpp"

/**
 * @class AudioInput
 * @brief Manages audio input by creating subscribers for specified topics and
 *        handling audio messages from these sources.
 */
class AudioInput : public cs4home_core::Afferent {
public:
  RCLCPP_SMART_PTR_DEFINITIONS(AudioInput)

  /**
   * @brief Constructs a AudioInput object and declares necessary
   * parameters.
   * @param parent Shared pointer to the lifecycle node managing this
   * AudioInput instance.
   */
  explicit AudioInput(rclcpp_lifecycle::LifecycleNode::SharedPtr parent)
      : Afferent(parent) {
    RCLCPP_DEBUG(parent_->get_logger(), "Afferent created: [AudioInput]");

    // Declares the parameter for input topics.
    parent_->declare_parameter("audio_input.topics", input_topic_names_);
    parent_->declare_parameter("audio_input.types", input_types_names_);
  }

  /**
   * @brief Configures the AudioInput by creating subscribers for each
   * specified topic.
   *
   * This method retrieves the topic names from the parameter server and
   * attempts to create a subscription for each topic to receive
   * `audio_common_msgs::msg::AudioData` messages.
   *
   * @return True if all subscriptions are created successfully.
   */
  bool configure() override {
    std::string param_name_topics = "audio_input.topics";
    std::string param_name_types = "audio_input.types";
    parent_->get_parameter(param_name_topics, input_topic_names_);
    parent_->get_parameter(param_name_types, input_types_names_);

    RCLCPP_DEBUG(parent_->get_logger(),
                 "[AudioInput] Configuring %zu inputs [%s]",
                 input_topic_names_.size(), param_name_topics.c_str());
    for (size_t i = 0; i < input_topic_names_.size(); i++) {
      if (create_subscriber(input_topic_names_[i], input_types_names_[i])) {
        RCLCPP_INFO(parent_->get_logger(),
                    "[AudioInput] created subscription to [%s, "
                    "%s]",
                    input_topic_names_[i].c_str(),
                    input_types_names_[i].c_str());
      } else {
        RCLCPP_WARN(parent_->get_logger(),
                    "[AudioInput] Couldn't create subscription to [%s, "
                    "%s]",
                    input_topic_names_[i].c_str(),
                    input_types_names_[i].c_str());
      }
    }

    return true;
  }

private:
  /**< List of input topics to subscribe to for audio information. */
  std::vector<std::string> input_topic_names_;
  std::vector<std::string> input_types_names_;
};

/// Registers the AudioInput component with the ROS 2 class loader
CS_REGISTER_COMPONENT(AudioInput)

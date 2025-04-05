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
      : Afferent("audio_input", parent) {
    RCLCPP_DEBUG(parent_->get_logger(), "Afferent created: [AudioInput]");
  }

  /**
   * @brief Configures the AudioInput by creating subscribers for each
   * specified topic.
   *
   * This method retrieves the topic names from the parameter server and
   * attempts to create a subscription for each topic.
   *
   * @return True if all subscriptions are created successfully.
   */
  bool configure() override { return Afferent::configure(); }
};

/// Registers the AudioInput component with the ROS 2 class loader
CS_REGISTER_COMPONENT(AudioInput)

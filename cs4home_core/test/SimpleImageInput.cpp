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

#include "sensor_msgs/msg/image.hpp"

#include "rclcpp_lifecycle/lifecycle_node.hpp"
#include "rclcpp/macros.hpp"

/**
 * @class SimpleImageInput
 * @brief Manages image input by creating subscribers for specified topics and
 *        handling image messages from these sources.
 */
class SimpleImageInput : public cs4home_core::Afferent
{
public:
  RCLCPP_SMART_PTR_DEFINITIONS(SimpleImageInput)

  /**
   * @brief Constructs a SimpleImageInput object and declares necessary parameters.
   * @param parent Shared pointer to the lifecycle node managing this SimpleImageInput instance.
   */
  explicit SimpleImageInput(rclcpp_lifecycle::LifecycleNode::SharedPtr parent)
  : Afferent(parent)
  {
    RCLCPP_DEBUG(parent_->get_logger(), "Efferent created: [SimpleImageInput]");

    // Declares the parameter for input topics.
    parent_->declare_parameter("simple_image_input.topics", input_topic_names_);
  }

  /**
   * @brief Configures the SimpleImageInput by creating subscribers for each specified topic.
   *
   * This method retrieves the topic names from the parameter server and attempts to create
   * a subscription for each topic to receive `sensor_msgs::msg::Image` messages.
   *
   * @return True if all subscriptions are created successfully.
   */
  bool configure() override
  {
    std::string param_name = "simple_image_input.topics";
    parent_->get_parameter(param_name, input_topic_names_);

    RCLCPP_DEBUG(
      parent_->get_logger(), "[SimpleImageInput] Configuring %zu inputs [%s]",
      input_topic_names_.size(), param_name.c_str());

    for (size_t i = 0; i < input_topic_names_.size(); i++) {
      if (create_subscriber(input_topic_names_[i], "sensor_msgs/msg/Image")) {
        RCLCPP_DEBUG(
          parent_->get_logger(),
          "[SimpleImageInput] created subscription to [%s, sensor_msgs/msg/Image]",
          input_topic_names_[i].c_str());
      } else {
        RCLCPP_WARN(
          parent_->get_logger(),
          "[SimpleImageInput] Couldn't create subscription to [%s, sensor_msgs/msg/Image]",
          input_topic_names_[i].c_str());
      }
    }

    return true;
  }

private:
  /**< List of input topics to subscribe to for images. */
  std::vector<std::string> input_topic_names_;
};

/// Registers the SimpleImageInput component with the ROS 2 class loader
CS_REGISTER_COMPONENT(SimpleImageInput)

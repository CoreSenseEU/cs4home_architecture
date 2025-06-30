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

#include "sensor_msgs/msg/image.hpp"

#include "rclcpp/macros.hpp"
#include "rclcpp_lifecycle/lifecycle_node.hpp"

/**
 * @class SimpleImageOutput
 * @brief Manages image output by creating publishers for specified topics and
 *        providing a method to publish image messages.
 */
class SimpleImageOutput : public cs4home_core::Efferent {
public:
  RCLCPP_SMART_PTR_DEFINITIONS(SimpleImageOutput)

  /**
   * @brief Constructs a SimpleImageOutput object and declares necessary
   * parameters.
   * @param parent Shared pointer to the lifecycle node managing this
   * SimpleImageOutput instance.
   */
  explicit SimpleImageOutput(rclcpp_lifecycle::LifecycleNode::SharedPtr parent)
      : Efferent("simple_image_output", parent) {
    RCLCPP_DEBUG(parent_->get_logger(),
                 "Afferent created: [SimpleImageOutput]");
  }

  /**
   * @brief Configures the SimpleImageOutput by creating publishers for each
   * specified topic.
   *
   * This method retrieves the topic names from the parameter server and
   * attempts to create a publisher for each topic to publish
   * `sensor_msgs::msg::Image` messages.
   *
   * @return True if all publishers are created successfully.
   */
  bool configure() { return Efferent::configure(); }

  /**
   * @brief Publishes an image message to all configured topics.
   * @param msg shared pointer to an image message of type
   * `sensor_msgs::msg::Image`.
   */
  void publish_image(sensor_msgs::msg::Image::SharedPtr msg) {
    publish(0, msg);
  }
};

/// Registers the SimpleImageOutput component with the ROS 2 class loader
CS_REGISTER_COMPONENT(SimpleImageOutput)

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

#include "sensor_msgs/msg/image.hpp"

#include "rclcpp_lifecycle/lifecycle_node.hpp"
#include "rclcpp/macros.hpp"

using std::placeholders::_1;
using namespace std::chrono_literals;

/**
 * @class ImageFilter
 * @brief Core component that filters incoming image messages by modifying their headers and
 *        republishing them on a timer.
 */
class ImageFilter : public cs4home_core::Core
{
public:
  RCLCPP_SMART_PTR_DEFINITIONS(ImageFilter)


  /**
   * @brief Constructs an ImageFilter object and initializes the parent lifecycle node.
   * @param parent Shared pointer to the lifecycle node managing this ImageFilter instance.
   */

  explicit ImageFilter(rclcpp_lifecycle::LifecycleNode::SharedPtr parent)
  : Core(parent)
  {
    RCLCPP_DEBUG(parent_->get_logger(), "Core created: [ImageFilter]");
  }

  /**
   * @brief Processes an incoming image message by modifying its header.
   *
   * The `frame_id` in the image header is converted to an integer, doubled, and set as the new
   * `frame_id`. The modified message is then sent to the efferent component.
   *
   * @param msg Unique pointer to the incoming image message of type `sensor_msgs::msg::Image`.
   */
  void process_in_image(sensor_msgs::msg::Image::UniquePtr msg)
  {
    int counter = std::atoi(msg->header.frame_id.c_str());
    counter = counter * 2;
    msg->header.frame_id = std::to_string(counter);

    efferent_->publish(std::move(msg));
  }

  /**
   * @brief Timer callback function that retrieves an image message and processes it.
   *
   * This function is called periodically and attempts to retrieve an image message from the
   * afferent component. If a message is received, it is passed to `process_in_image`.
   */
  void timer_callback()
  {
    auto msg = afferent_->get_msg<sensor_msgs::msg::Image>();
    if (msg != nullptr) {
      process_in_image(std::move(msg));
    }
  }

  /**
   * @brief Configures the ImageFilter component.
   * @return True if configuration is successful.
   */
  bool configure() override
  {
    RCLCPP_DEBUG(parent_->get_logger(), "Core configured");
    return true;
  }

  /**
   * @brief Activates the ImageFilter component by initializing a timer.
   *
   * The timer is set to call `timer_callback` every 50 milliseconds.
   *
   * @return True if activation is successful.
   */
  bool activate() override
  {
    timer_ = parent_->create_wall_timer(
      50ms, std::bind(&ImageFilter::timer_callback, this));
    return true;
  }

  /**
   * @brief Deactivates the ImageFilter component by disabling the timer.
   *
   * The timer is reset to null, stopping periodic message processing.
   *
   * @return True if deactivation is successful.
   */
  bool deactivate() override
  {
    timer_ = nullptr;
    return true;
  }

private:
  rclcpp::TimerBase::SharedPtr timer_; /**< Timer for periodic execution of `timer_callback`. */
};

/// Registers the ImageFilter component with the ROS 2 class loader
CS_REGISTER_COMPONENT(ImageFilter)

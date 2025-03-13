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
 * @class ImageFilterCB
 * @brief Core component that filters incoming image messages by modifying their headers and
 *        republishing them. Uses ROS 2 lifecycle and callback functions for message processing.
 */
class ImageFilterCB : public cs4home_core::Core
{
public:
  RCLCPP_SMART_PTR_DEFINITIONS(ImageFilterCB)

  /**
   * @brief Constructs an ImageFilterCB object and initializes the parent lifecycle node.
   * @param parent Shared pointer to the lifecycle node managing this ImageFilterCB instance.
   */
  explicit ImageFilterCB(rclcpp_lifecycle::LifecycleNode::SharedPtr parent)
  : Core(parent)
  {
    RCLCPP_DEBUG(parent_->get_logger(), "Core created: [ImageFilterCB]");
  }

  /**
   * @brief Processes incoming serialized image messages, applies a simple transformation, and
   *        republishes them.
   *
   * The `frame_id` in the image header is converted to an integer, doubled, and set as the new
   * `frame_id`. The modified message is then sent to the efferent component.
   *
   * @param msg Unique pointer to the serialized incoming image message.
   */
  void process_in_image(std::unique_ptr<rclcpp::SerializedMessage> msg)
  {
    auto image_msg = afferent_->get_msg<sensor_msgs::msg::Image>(std::move(msg));

    int counter = std::atoi(image_msg->header.frame_id.c_str());
    counter = counter * 2;
    image_msg->header.frame_id = std::to_string(counter);

    efferent_->publish(std::move(image_msg));
  }

  /**
   * @brief Configures the ImageFilterCB component.
   *
   * Sets the afferent component mode to `CALLBACK`, binding the `process_in_image` method to
   * handle incoming messages.
   *
   * @return True if configuration is successful.
   */
  bool configure() override
  {
    RCLCPP_DEBUG(parent_->get_logger(), "Core configured");

    afferent_->set_mode(
      cs4home_core::Afferent::CALLBACK, std::bind(&ImageFilterCB::process_in_image, this, _1));

    return true;
  }

  /**
   * @brief Activates the ImageFilterCB component.
   * @return True if activation is successful.
   */
  bool activate() override
  {
    return true;
  }

  /**
   * @brief Deactivates the ImageFilterCB component.
   *
   * Disables the internal timer by resetting the timer shared pointer to null.
   *
   * @return True if deactivation is successful.
   */
  bool deactivate() override
  {
    timer_ = nullptr;
    return true;
  }

private:
  rclcpp::TimerBase::SharedPtr timer_; /**< Timer used for periodic operations if needed. */
};

/// Registers the ImageFilterCB component with the ROS 2 class loader
CS_REGISTER_COMPONENT(ImageFilterCB)

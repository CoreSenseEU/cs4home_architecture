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

#include "rclcpp/rclcpp.hpp"
#include "rclcpp/create_generic_subscription.hpp"
namespace cs4home_core
{

/**
 * @brief Constructor for the Afferent class.
 * @param parent Shared pointer to the lifecycle node that owns this Afferent instance.
 */
Afferent::Afferent(rclcpp_lifecycle::LifecycleNode::SharedPtr parent)
: parent_(parent)
{
}

/**
 * @brief Sets the operation mode and an optional callback function.
 *
 * This function allows configuring the Afferent object with a specific
 * processing mode and an optional callback to handle serialized messages.
 *
 * @param mode The processing mode for the Afferent object.
 * @param cb A callback function to process serialized messages, used if the mode is CALLBACK.
 */
void
Afferent::set_mode(
  EfferentProcessMode mode,
  std::function<void(std::unique_ptr<rclcpp::SerializedMessage>)> cb)
{
  if (mode == CALLBACK) {
    if (cb) {
      callback_ = cb;
    } else {
      RCLCPP_WARN(
        parent_->get_logger(), "[Afferent] Error setting callback: not function specified");
      return;
    }
  }
  mode_ = mode;
}


/**
 * @brief Creates a subscription to a specified topic and type.
 *
 * This method sets up a subscription to receive messages on a given topic with
 * a specified message type. The received messages are either processed through
 * a callback (if set) or stored in an internal message queue.
 *
 * @param topic The topic name to subscribe to.
 * @param type The type of messages expected on the topic.
 * @return True if the subscription was created successfully.
 */
bool
Afferent::create_subscriber(const std::string & topic, const std::string & type)
{
  RCLCPP_DEBUG(
    parent_->get_logger(),
    "[Afferent] Creating subscription [%s, %s]",
    topic.c_str(), type.c_str());

  auto sub = rclcpp::create_generic_subscription(
    parent_->get_node_topics_interface(), topic, type, 100,
    [&](std::unique_ptr<rclcpp::SerializedMessage> msg)
    {
      if (mode_ == CALLBACK) {
        if (callback_) {
          callback_(std::move(msg));
        } else {
          RCLCPP_WARN(
            parent_->get_logger(), "[Afferent] Error calling callback: not function specified");
        }
      } else {
        msg_queue_.push(std::move(msg));
        if (msg_queue_.size() > max_queue_size_) {
          msg_queue_.pop();
        }
      }
    });


  subs_.push_back(sub);

  return true;
}

}  // namespace cs4home_core

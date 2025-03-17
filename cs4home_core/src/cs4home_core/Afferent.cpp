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
Afferent::Afferent(const std::string & name, rclcpp_lifecycle::LifecycleNode::SharedPtr parent)
: parent_(parent),
  name_(name)
{
  // Declares the parameter for input topics. and types
  parent_->declare_parameter(name_ + ".topics", input_topic_names_);
  parent_->declare_parameter(name_ + ".types", input_topic_types_);
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
  const std::string & topic,
  EfferentProcessMode mode,
  std::function<void(std::unique_ptr<rclcpp::SerializedMessage>)> cb)
{
  if (mode == CALLBACK) {
    if (cb) {
      callbacks_[topic] = cb;
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


  if (msg_queues_.find(topic) == msg_queues_.end()) {
    msg_queues_[topic] = std::queue<std::unique_ptr<rclcpp::SerializedMessage>>();
  }

  auto sub = rclcpp::create_generic_subscription(
    parent_->get_node_topics_interface(), topic, type, 100,
    [&](std::unique_ptr<rclcpp::SerializedMessage> msg)
    {
      if (mode_ == CALLBACK) {
        if (callbacks_[topic]) {
          callbacks_[topic](std::move(msg));
        } else {
          RCLCPP_WARN(
            parent_->get_logger(), "[Afferent] Error calling callback: not function specified");
        }
      } else {
        msg_queues_[topic].push(std::move(msg));
        if (msg_queues_[topic].size() > max_queue_size_) {
          msg_queues_[topic].pop();
        }
      }
    });


  subs_.push_back(sub);

  return true;
}


/**
 * @brief Configures the Afferent by creating subscribers for each specified topic.
 *
 * This method retrieves the topic names from the parameter server and attempts to create
 * a subscription for each topic to receive messages.
 *
 * @return True if all subscriptions are created successfully.
 */
bool
Afferent::configure()
{
  parent_->get_parameter(name_ + ".topics", input_topic_names_);
  parent_->get_parameter(name_ + ".types", input_topic_types_);

  if (input_topic_names_.size() != input_topic_types_.size()) {
    RCLCPP_ERROR(
      parent_->get_logger(),
      "Parameter sizes are not correct (%zu != %zu)",
      input_topic_names_.size(), input_topic_types_.size());
    return false;
  }

  RCLCPP_DEBUG(
      parent_->get_logger(),
      "Creating %zu subscriptions: ", input_topic_names_.size());

  for (size_t i = 0; i < input_topic_names_.size(); i++) {
    RCLCPP_DEBUG(
      parent_->get_logger(),
      "\tCreating subscription for topic [%s] (%s)",
      input_topic_names_[i].c_str(), input_topic_types_[i].c_str());

    if (create_subscriber(input_topic_names_[i], input_topic_types_[i])) {
      RCLCPP_DEBUG(
        parent_->get_logger(),
        "[SimpleImageInput] created subscription to [%s, %s]",
        input_topic_names_[i].c_str(), input_topic_types_[i].c_str());
    } else {
      RCLCPP_WARN(
        parent_->get_logger(),
        "[SimpleImageInput] Couldn't create subscription to [%s, %s]",
        input_topic_names_[i].c_str(), input_topic_types_[i].c_str());
    }
  }

  return true;
}

}  // namespace cs4home_core

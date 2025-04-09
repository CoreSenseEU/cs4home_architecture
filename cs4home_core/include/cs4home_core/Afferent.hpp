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


#ifndef CS4HOME_CORE__AFFERENT_HPP_
#define CS4HOME_CORE__AFFERENT_HPP_

#include <map>
#include <memory>
#include <utility>
#include <queue>
#include <string>
#include <vector>

#include "rclcpp_lifecycle/lifecycle_node.hpp"
#include "rclcpp/rclcpp.hpp"
#include "rclcpp/macros.hpp"
#include "rclcpp/serialization.hpp"
#include "rclcpp/create_generic_subscription.hpp"

namespace cs4home_core
{

/**
 * @class Afferent
 * @brief Manages afferent processing in robotic nodes, including message handling,
 *        subscriptions, and modes for processing serialized data.
 */
class Afferent
{
public:
  RCLCPP_SMART_PTR_DEFINITIONS(Afferent)

  /**
  * @enum EfferentProcessMode
  * @brief Defines processing modes for serialized message handling.
  */
  enum EfferentProcessMode {CALLBACK, ONDEMAND};

  /**
   * @brief Constructor for the Afferent class.
   * @param parent Shared pointer to the lifecycle node managing this instance.
   */
  explicit Afferent(const std::string & name, rclcpp_lifecycle::LifecycleNode::SharedPtr parent);

  /**
   * @brief Configures the afferent component; intended for subclass implementation.
   * @return True if configuration is successful.
   */
  virtual bool configure();

  /**
   * @brief Sets the processing mode and an optional callback function.
   *
   * @param topic The topic to subscribe
   * @param mode Processing mode for handling messages.
   * @param cb Optional callback function for handling serialized messages in CALLBACK mode.
   */
  void set_mode(
    const std::string & topic,
    EfferentProcessMode mode,
    std::function<void(std::unique_ptr<rclcpp::SerializedMessage>)> cb = nullptr);

  /**
   * @brief Sets the processing mode and an optional callback function.
   *
   * @param topic_idx The index of the topic input.
   * @param mode Processing mode for handling messages.
   * @param cb Optional callback function for handling serialized messages in CALLBACK mode.
   */
  void set_mode(
    size_t topic_idx,
    EfferentProcessMode mode,
    std::function<void(std::unique_ptr<rclcpp::SerializedMessage>)> cb = nullptr);

  /**
   * @brief Gets the current processing mode.
   * @return The current EfferentProcessMode.
   */
  EfferentProcessMode get_mode() {return mode_;}

  /**
   * @brief Sets the maximum queue size for storing messages.
   * @param size Maximum number of messages the queue can hold.
   */
  void set_max_queue_size(size_t size) {max_queue_size_ = size;}

  /**
   * @brief Gets the maximum queue size.
   * @return The maximum queue size.
   */
  size_t get_max_queue_size() {return max_queue_size_;}

  /**
  * @brief Converts a serialized message to a typed message.
  * @tparam MessageT Type of the message to deserialize.
  * @param msg Serialized message to convert.
  * @return A unique pointer to the deserialized message.
  */
  template<class MessageT> std::unique_ptr<MessageT> get_msg(
    std::unique_ptr<rclcpp::SerializedMessage> msg)
  {
    auto typed_msg = std::make_unique<MessageT>();
    rclcpp::Serialization<MessageT> serializer;
    serializer.deserialize_message(msg.get(), typed_msg.get());

    return std::move(typed_msg);
  }

  /**
   * @brief Retrieves the next message from the queue, if available.
   * @tparam MessageT Type of message to retrieve.
   * @tparam topic The topic of the associated queue to retrieve the message.
   * @return A unique pointer to the next message, or nullptr if the queue is
   * empty or does not exists.
   */
  template<class MessageT> std::unique_ptr<MessageT> get_msg(const std::string & topic)
  {
    if (msg_queues_.find(topic) == msg_queues_.end() || msg_queues_[topic].empty()) {
      return nullptr;
    }

    std::unique_ptr<rclcpp::SerializedMessage> msg = std::move(msg_queues_[topic].front());
    msg_queues_[topic].pop();

    return get_msg<MessageT>(std::move(msg));
  }

  /**
   * @brief Retrieves the next message from the queue, if available.
   * @tparam MessageT Type of message to retrieve.
   * @tparam topic_idx The index fn the topic input associated queue to retrieve the message.
   * @return A unique pointer to the next message, or nullptr if the queue is
   * empty or does not exists.
   */
  template<class MessageT> std::unique_ptr<MessageT> get_msg(size_t topic_idx)
  {
    if (topic_idx >= msg_queues_.size()) {
      return nullptr;
    }

    const std::string & topic = input_topic_names_[topic_idx];

    if (msg_queues_.find(topic) == msg_queues_.end() || msg_queues_[topic].empty()) {
      return nullptr;
    }

    std::unique_ptr<rclcpp::SerializedMessage> msg = std::move(msg_queues_[topic].front());
    msg_queues_[topic].pop();

    return get_msg<MessageT>(std::move(msg));
  }

protected:
  /** Shared pointer to the parent node. */
  rclcpp_lifecycle::LifecycleNode::SharedPtr parent_;
  std::string name_;

  EfferentProcessMode mode_ {ONDEMAND}; /**< Current processing mode. */

  /** Default maximum queue size. */
  const size_t MAX_DEFAULT_QUEUE_SIZE = 100;
  /** Maximum queue size. */
  size_t max_queue_size_ {MAX_DEFAULT_QUEUE_SIZE};

  /** List of subscriptions. */
  std::vector<std::shared_ptr<rclcpp::GenericSubscription>> subs_;
  /** Queue for serialized messages. */
  std::map<std::string, std::queue<std::unique_ptr<rclcpp::SerializedMessage>>> msg_queues_;
  /**< List of input topics to subscribe to for images. */
  std::vector<std::string> input_topic_names_;
  /**< List of input topics types. */
  std::vector<std::string> input_topic_types_;

  /** Callback for serialized messages. */
  std::map<std::string, std::function<void(std::unique_ptr<rclcpp::SerializedMessage>)>> callbacks_;


  /**
   * @brief Creates a subscriber for a specific topic and message type.
   * @param topic Topic to subscribe to.
   * @param type Type of message for the subscription.
   * @return True if the subscriber was created successfully.
   */
  bool create_subscriber(const std::string & topic, const std::string & type);
};

}  // namespace cs4home_core

#endif  // CS4HOME_CORE__AFFERENT_HPP_

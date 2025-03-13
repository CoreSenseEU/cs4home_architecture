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

#include <memory>
#include <utility>
#include <queue>
#include <vector>
#include <string>

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
  explicit Afferent(rclcpp_lifecycle::LifecycleNode::SharedPtr parent);

  /**
   * @brief Configures the afferent component; intended for subclass implementation.
   * @return True if configuration is successful.
   */
  virtual bool configure() = 0;

  /**
   * @brief Sets the processing mode and an optional callback function.
   *
   * @param mode Processing mode for handling messages.
   * @param cb Optional callback function for handling serialized messages in CALLBACK mode.
   */
  void set_mode(
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
   * @return A unique pointer to the next message, or nullptr if the queue is empty.
   */
  template<class MessageT> std::unique_ptr<MessageT> get_msg()
  {
    if (msg_queue_.empty()) {
      return {};
    }

    std::unique_ptr<rclcpp::SerializedMessage> msg = std::move(msg_queue_.front());
    msg_queue_.pop();

    return get_msg<MessageT>(std::move(msg));
  }

protected:
  /** Shared pointer to the parent node. */
  rclcpp_lifecycle::LifecycleNode::SharedPtr parent_;
  /** List of subscriptions. */
  std::vector<std::shared_ptr<rclcpp::GenericSubscription>> subs_;

  EfferentProcessMode mode_ {ONDEMAND}; /**< Current processing mode. */

  /** Default maximum queue size. */
  const size_t MAX_DEFAULT_QUEUE_SIZE = 100;
  /** Maximum queue size. */
  size_t max_queue_size_ {MAX_DEFAULT_QUEUE_SIZE};
  /** Queue for serialized messages. */
  std::queue<std::unique_ptr<rclcpp::SerializedMessage>> msg_queue_;

  /** Callback for serialized messages. */
  std::function<void(std::unique_ptr<rclcpp::SerializedMessage>)> callback_;


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

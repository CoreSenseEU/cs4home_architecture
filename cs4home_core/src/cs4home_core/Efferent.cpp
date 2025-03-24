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

namespace cs4home_core {

/**
 * @brief Constructs an Efferent object and assigns the parent lifecycle node.
 * @param parent Shared pointer to the lifecycle node managing this Efferent
 * instance.
 */
Efferent::Efferent(const std::string &name,
                   rclcpp_lifecycle::LifecycleNode::SharedPtr parent)
    : parent_(parent), name_(name) {
  // Declares the parameter for output topics.
  parent_->declare_parameter(name_ + ".topics", output_topic_names_);
  parent_->declare_parameter(name_ + ".types", output_topic_names_);
}

/**
 * @brief Configures the Efferent by creating publishers for each specified
 * topic.
 *
 * This method retrieves the topic names from the parameter server and attempts
 * to create publishers for each topic to send messages.
 *
 * @return True if all publishers are created successfully.
 */
bool Efferent::configure() {
  parent_->get_parameter(name_ + ".topics", output_topic_names_);
  parent_->get_parameter(name_ + ".types", output_topic_types_);

  for (size_t i = 0; i < output_topic_names_.size(); i++) {
    if (create_publisher(output_topic_names_[i], output_topic_types_[i])) {
      RCLCPP_INFO(
          parent_->get_logger(), "[Efferent] created publisher to [%s, %s]",
          output_topic_names_[i].c_str(), output_topic_types_[i].c_str());
    } else {
      RCLCPP_WARN(parent_->get_logger(),
                  "[Efferent] Couldn't create publisher to [%s, %s]",
                  output_topic_names_[i].c_str(),
                  output_topic_types_[i].c_str());
    }
  }
  return true;
}

/**
 * @brief Creates a publisher for a specified topic and message type.
 *
 * This function sets up a generic publisher on a given topic, allowing the
 * Efferent component to send messages of the specified type.
 *
 * @param topic The topic name to publish messages to.
 * @param type The type of messages to publish on the topic.
 * @return True if the publisher was created successfully.
 */
bool Efferent::create_publisher(const std::string &topic,
                                const std::string &type) {
  auto pub = rclcpp::create_generic_publisher(
      parent_->get_node_topics_interface(), topic, type, 100);

  pubs_.push_back(pub);

  return true;
}

} // namespace cs4home_core

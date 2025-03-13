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

#ifndef CS4HOME_CORE__META_HPP_
#define CS4HOME_CORE__META_HPP_

#include "rclcpp_lifecycle/lifecycle_node.hpp"
#include "rclcpp/macros.hpp"

namespace cs4home_core
{

/**
 * @class Meta
 * @brief Provides a meta-component for managing additional lifecycle functions
 *        within a robotic system, associating with a parent lifecycle node.
 */
class Meta
{
public:
  RCLCPP_SMART_PTR_DEFINITIONS(Meta)

  /**
   * @brief Constructs a Meta object associated with a parent lifecycle node.
   * @param parent Shared pointer to the lifecycle node managing this Meta instance.
   */
  explicit Meta(rclcpp_lifecycle::LifecycleNode::SharedPtr parent);

  /**
   * @brief Configures the Meta component.
   * @return True if configuration is successful.
   */
  bool configure();

protected:
  /// < Shared pointer to the parent lifecycle node.
  rclcpp_lifecycle::LifecycleNode::SharedPtr parent_;
};

}  // namespace cs4home_core

#endif  // CS4HOME_CORE__META_HPP_

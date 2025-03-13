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

#ifndef CS4HOME_CORE__CORE_HPP_
#define CS4HOME_CORE__CORE_HPP_

#include <memory>

#include "cs4home_core/Afferent.hpp"
#include "cs4home_core/Efferent.hpp"

#include "rclcpp_lifecycle/lifecycle_node.hpp"
#include "rclcpp/macros.hpp"

namespace cs4home_core
{

/**
 * @class Core
 * @brief Manages core functionality for a robotic component, including lifecycle transitions
 *        and connections to afferent and efferent processing components.
 */
class Core
{
public:
  RCLCPP_SMART_PTR_DEFINITIONS(Core)

  /**
   * @brief Constructs a Core object associated with a parent lifecycle node.
   * @param parent Shared pointer to the lifecycle node managing this Core instance.
   */
  explicit Core(rclcpp_lifecycle::LifecycleNode::SharedPtr parent);

  /**
   * @brief Configures the Core component.
   * @return True if configuration is successful.
   */
  virtual bool configure() = 0;

  /**
   * @brief Activates the Core component.
   * @return True if activation is successful.
   */
  virtual bool activate() = 0;

  /**
   * @brief Deactivates the Core component.
   * @return True if deactivation is successful.
   */
  virtual bool deactivate() = 0;

  /**
   * @brief Sets the Afferent component associated with this Core.
   * @param afferent Shared pointer to an Afferent component.
   */
  void set_afferent(cs4home_core::Afferent::SharedPtr afferent) {afferent_ = afferent;}

  /**
   * @brief Sets the Efferent component associated with this Core.
   * @param efferent Shared pointer to an Efferent component.
   */
  void set_efferent(cs4home_core::Efferent::SharedPtr efferent) {efferent_ = efferent;}

protected:
  /** Shared pointer to the parent lifecycle node. */
  rclcpp_lifecycle::LifecycleNode::SharedPtr parent_;
  /** Shared pointer to the Afferent component. */
  cs4home_core::Afferent::SharedPtr afferent_;
  /** Shared pointer to the Efferent component. */
  cs4home_core::Efferent::SharedPtr efferent_;
};

}  // namespace cs4home_core

#endif  // CS4HOME_CORE__CORE_HPP_

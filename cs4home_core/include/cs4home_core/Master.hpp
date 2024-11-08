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

#ifndef CS4HOME_CORE__MASTER_HPP_
#define CS4HOME_CORE__MASTER_HPP_

#include <map>
#include <string>

#include "cs4home_core/CognitiveModule.hpp"

#include "rclcpp_lifecycle/lifecycle_node.hpp"
#include "rclcpp/rclcpp.hpp"
#include "rclcpp/macros.hpp"

namespace cs4home_core
{

/**
 * @class Master
 * @brief Represents the master node that manages multiple cognitive modules
 *        within a ROS 2 lifecycle-based robotic system.
 */
class Master : public rclcpp_lifecycle::LifecycleNode
{
public:
  RCLCPP_SMART_PTR_DEFINITIONS(Master)
  using CallbackReturnT = rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn;

  /**
   * @brief Constructs a Master lifecycle node with the specified options.
   * @param options Node options to configure the Master node.
   */
  explicit Master(const rclcpp::NodeOptions & options = rclcpp::NodeOptions());

  /**
   * @brief Configures the Master node.
   * @param state The current lifecycle state.
   * @return CallbackReturnT::SUCCESS if configuration is successful.
   */
  CallbackReturnT on_configure(const rclcpp_lifecycle::State & state);

  /**
   * @brief Activates the Master node.
   * @param state The current lifecycle state.
   * @return CallbackReturnT::SUCCESS if activation is successful.
   */
  CallbackReturnT on_activate(const rclcpp_lifecycle::State & state);

  /**
   * @brief Deactivates the Master node.
   * @param state The current lifecycle state.
   * @return CallbackReturnT::SUCCESS if deactivation is successful.
   */
  CallbackReturnT on_deactivate(const rclcpp_lifecycle::State & state);

  /**
   * @brief Cleans up the Master node.
   * @param state The current lifecycle state.
   * @return CallbackReturnT::SUCCESS indicating cleanup is complete.
   */
  CallbackReturnT on_cleanup(const rclcpp_lifecycle::State & state);

  /**
   * @brief Shuts down the Master node.
   * @param state The current lifecycle state.
   * @return CallbackReturnT::SUCCESS indicating shutdown is complete.
   */
  CallbackReturnT on_shutdown(const rclcpp_lifecycle::State & state);

  /**
   * @brief Handles errors in the Master node.
   * @param state The current lifecycle state.
   * @return CallbackReturnT::SUCCESS indicating error handling is complete.
   */
  CallbackReturnT on_error(const rclcpp_lifecycle::State & state);

protected:
  /** Map of cognitive modules managed by the Master node. */
  std::map<std::string, cs4home_core::CognitiveModule::SharedPtr> cog_modules_;
};

}  // namespace cs4home_core

#endif  // CS4HOME_CORE__MASTER_HPP_

// #include "rclcpp_components/register_node_macro.hpp"
//
// // Register the component with class_loader.
// // This acts as a sort of entry point, allowing the component to be discoverable when its library
// // is being loaded into a running process.
// RCLCPP_COMPONENTS_REGISTER_NODE(cs4home_core::Master)

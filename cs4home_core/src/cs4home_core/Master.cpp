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

#include "cs4home_core/Master.hpp"

namespace cs4home_core
{

/**
 * @brief Constructs a Master lifecycle node with the specified options.
 * @param options Node options to configure the Master node.
 */
Master::Master(const rclcpp::NodeOptions & options)
: LifecycleNode("master", options)
{
}

using CallbackReturnT = rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn;

/**
 * @brief Configures the Master node.
 * @param state The current lifecycle state.
 * @return CallbackReturnT::SUCCESS if configuration is successful.
 */
CallbackReturnT
Master::on_configure(const rclcpp_lifecycle::State & state)
{
  (void)state;

  return CallbackReturnT::SUCCESS;
}

/**
 * @brief Activates the Master node.
 * @param state The current lifecycle state.
 * @return CallbackReturnT::SUCCESS if activation is successful.
 */
CallbackReturnT
Master::on_activate(const rclcpp_lifecycle::State & state)
{
  (void)state;

  return CallbackReturnT::SUCCESS;
}

/**
 * @brief Deactivates the Master node.
 * @param state The current lifecycle state.
 * @return CallbackReturnT::SUCCESS if deactivation is successful.
 */
CallbackReturnT
Master::on_deactivate(const rclcpp_lifecycle::State & state)
{
  (void)state;

  return CallbackReturnT::SUCCESS;
}

/**
 * @brief Cleans up the Master node.
 * @param state The current lifecycle state.
 * @return CallbackReturnT::SUCCESS indicating cleanup is complete.
 */
CallbackReturnT
Master::on_cleanup(const rclcpp_lifecycle::State & state)
{
  (void)state;

  return CallbackReturnT::SUCCESS;
}

/**
 * @brief Shuts down the Master node.
 * @param state The current lifecycle state.
 * @return CallbackReturnT::SUCCESS indicating shutdown is complete.
 */
CallbackReturnT
Master::on_shutdown(const rclcpp_lifecycle::State & state)
{
  (void)state;

  return CallbackReturnT::SUCCESS;
}

/**
 * @brief Handles errors in the Master node.
 * @param state The current lifecycle state.
 * @return CallbackReturnT::SUCCESS indicating error handling is complete.
 */
CallbackReturnT
Master::on_error(const rclcpp_lifecycle::State & state)
{
  (void)state;

  return CallbackReturnT::SUCCESS;
}

}  // namespace cs4home_core

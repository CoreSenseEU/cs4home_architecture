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

#ifndef CS4HOME_CORE__COGNITIVEMODULE_HPP_
#define CS4HOME_CORE__COGNITIVEMODULE_HPP_

#include <dlfcn.h>
#include <tuple>
#include <string>

#include "cs4home_core/Afferent.hpp"
#include "cs4home_core/Core.hpp"
#include "cs4home_core/Coupling.hpp"
#include "cs4home_core/Efferent.hpp"
#include "cs4home_core/Meta.hpp"

#include "rclcpp/macros.hpp"
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_lifecycle/lifecycle_node.hpp"

namespace cs4home_core
{

/**
 * @class CognitiveModule
 * @brief Extends the LifecycleNode to manage cognitive processing components in a ROS 2 lifecycle,
 *        including afferent, efferent, core, meta, and coupling components.
 */
class CognitiveModule : public rclcpp_lifecycle::LifecycleNode
{
public:
  RCLCPP_SMART_PTR_DEFINITIONS(CognitiveModule)
  using CallbackReturnT = rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn;


  /**
   * @brief Constructs a CognitiveModule with the specified node options.
   * @param options Node options for configuring the lifecycle node.
   */
  explicit CognitiveModule(
    const std::string & name,
    const rclcpp::NodeOptions & options = rclcpp::NodeOptions());


  /**
   * @brief Lifecycle transition callback for configuration.
   * @param state The current lifecycle state.
   * @return Result of the configuration, typically success or failure.
   */
  CallbackReturnT on_configure(const rclcpp_lifecycle::State & state);

  /**
   * @brief Lifecycle transition callback for activation.
   * @param state The current lifecycle state.
   * @return Result of the activation, typically success or failure.
   */
  CallbackReturnT on_activate(const rclcpp_lifecycle::State & state);

  /**
   * @brief Lifecycle transition callback for deactivation.
   * @param state The current lifecycle state.
   * @return Result of the deactivation, typically success or failure.
   */
  CallbackReturnT on_deactivate(const rclcpp_lifecycle::State & state);

  /**
   * @brief Lifecycle transition callback for cleanup.
   * @param state The current lifecycle state.
   * @return Result of the cleanup, typically success or failure.
   */
  CallbackReturnT on_cleanup(const rclcpp_lifecycle::State & state);

  /**
   * @brief Lifecycle transition callback for shutdown.
   * @param state The current lifecycle state.
   * @return Result of the shutdown, typically success or failure.
   */
  CallbackReturnT on_shutdown(const rclcpp_lifecycle::State & state);

  /**
   * @brief Lifecycle transition callback for error handling.
   * @param state The current lifecycle state.
   * @return Result of the error handling, typically success or failure.
   */
  CallbackReturnT on_error(const rclcpp_lifecycle::State & state);

protected:
  Afferent::SharedPtr afferent_; /**< Pointer to the Afferent component. */
  Efferent::SharedPtr efferent_; /**< Pointer to the Efferent component. */
  Core::SharedPtr core_; /**< Pointer to the Core component. */
  Meta::SharedPtr meta_; /**< Pointer to the Meta component. */
  Coupling::SharedPtr coupling_; /**< Pointer to the Coupling component. */

  std::string core_name_; /**< Name of the Core component. */
  std::string afferent_name_; /**< Name of the Afferent component. */
  std::string efferent_name_; /**< Name of the Efferent component. */
  std::string meta_name_; /**< Name of the Meta component. */
  std::string coupling_name_; /**< Name of the Coupling component. */

  /**
   * @brief Loads a specified component by name and returns a shared pointer to it.
   * @tparam T Type of the component to load.
   * @param name Name of the component to load.
   * @param parent Shared pointer to the parent lifecycle node.
   * @return A tuple containing the loaded component pointer and its name.
   */
  template<class T>
  std::tuple<typename T::SharedPtr, std::string> load_component(
    const std::string & name, rclcpp_lifecycle::LifecycleNode::SharedPtr parent);
};

}  // namespace cs4home_core

#endif  // CS4HOME_CORE__COGNITIVEMODULE_HPP_

// #include "rclcpp_components/register_node_macro.hpp"
//
// // Register the component with class_loader.
// // This acts as a sort of entry point, allowing the component to be discoverable when its library
// // is being loaded into a running process.
// RCLCPP_COMPONENTS_REGISTER_NODE(cs4home_core::CognitiveModule)

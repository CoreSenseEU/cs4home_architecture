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

#include "cs4home_core/Meta.hpp"
#include "cs4home_core/macros.hpp"

#include "rclcpp_lifecycle/lifecycle_node.hpp"
#include "rclcpp/macros.hpp"

/**
 * @class DefaultMeta
 * @brief A Meta component that provides default configurations for meta-level operations.
 *
 * This class extends the Meta component, initializing with a basic configuration.
 * It is intended for meta-level tasks that require minimal setup.
 */
class DefaultMeta : public cs4home_core::Meta
{
public:
  RCLCPP_SMART_PTR_DEFINITIONS(DefaultMeta)

  /**
   * @brief Constructs a DefaultMeta object and initializes the parent lifecycle node.
   * @param parent Shared pointer to the lifecycle node managing this DefaultMeta instance.
   */
  explicit DefaultMeta(rclcpp_lifecycle::LifecycleNode::SharedPtr parent)
  : Meta(parent)
  {
    RCLCPP_DEBUG(parent_->get_logger(), "Meta created: [DefaultMeta]");
  }

  /**
   * @brief Configures the DefaultMeta component.
   *
   * Logs the configuration step and prepares the component for operation.
   *
   * @return True if configuration is successful.
   */
  bool configure()
  {
    RCLCPP_DEBUG(parent_->get_logger(), "Meta configured");
    return true;
  }
};

/// Registers the DefaultMeta component with the ROS 2 class loader
CS_REGISTER_COMPONENT(DefaultMeta)

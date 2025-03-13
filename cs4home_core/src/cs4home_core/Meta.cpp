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

namespace cs4home_core
{

/**
 * @brief Constructs a Meta object and assigns the parent lifecycle node.
 * @param parent Shared pointer to the lifecycle node managing this Meta instance.
 */
Meta::Meta(rclcpp_lifecycle::LifecycleNode::SharedPtr parent)
: parent_(parent)
{
}

/**
 * @brief Configures the Meta component.
 * @return True if configuration is successful.
 */
bool
Meta::configure()
{
  return true;
}

}  // namespace cs4home_core

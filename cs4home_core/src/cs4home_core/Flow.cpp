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


#include <list>
#include <string>
#include <vector>
#include <initializer_list>

#include "cs4home_core/Flow.hpp"

#include "rclcpp/rclcpp.hpp"
#include "rclcpp/macros.hpp"

namespace cs4home_core
{

Flow::Flow(const std::string & name)
: CascadeLifecycleNode(name)
{
  trigger_transition(lifecycle_msgs::msg::Transition::TRANSITION_CONFIGURE);
}

Flow::Flow(const std::string & name, const std::vector<std::string> & nodes)
: Flow(name)
{
  set_flow(nodes);
}

void
Flow::set_flow(const std::vector<std::string> & nodes)
{
  clear_activation();

  nodes_ = nodes;

  for (const auto & node : nodes) {
    add_activation(node);
  }
}

void
Flow::activate()
{
  trigger_transition(lifecycle_msgs::msg::Transition::TRANSITION_ACTIVATE);
}

void
Flow::deactivate()
{
  trigger_transition(lifecycle_msgs::msg::Transition::TRANSITION_DEACTIVATE);
}

void
Flow::print() const
{
  for (const auto & node : nodes_) {
    std::cout << " -> " << node;
  }
  std::cout << std::endl;
}


}  // namespace cs4home_core

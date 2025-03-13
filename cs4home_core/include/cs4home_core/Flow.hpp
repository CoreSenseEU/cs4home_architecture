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


#ifndef CS4HOME_CORE__FLOW_HPP_
#define CS4HOME_CORE__FLOW_HPP_

#include <list>
#include <string>
#include <vector>
#include <initializer_list>

#include "rclcpp/rclcpp.hpp"
#include "rclcpp/macros.hpp"
#include "rclcpp_lifecycle/lifecycle_node.hpp"
#include "rclcpp_cascade_lifecycle/rclcpp_cascade_lifecycle.hpp"

namespace cs4home_core
{

class Flow : public rclcpp_cascade_lifecycle::CascadeLifecycleNode
{
public:
  RCLCPP_SMART_PTR_DEFINITIONS(Flow)

  explicit Flow(const std::string & name);
  Flow(const std::string & name, const std::vector<std::string> & nodes);

  void print() const;
  const std::vector<std::string> & get_flow() const {return nodes_;}
  void set_flow(const std::vector<std::string> & nodes);

  void activate();
  void deactivate();
  
private:
  std::vector<std::string> nodes_;
};


}  // namespace cs4home_core

#endif  // CS4HOME_CORE__FLOW_HPP_

// #include "rclcpp_components/register_node_macro.hpp"
//
// // Register the component with class_loader.
// // This acts as a sort of entry point, allowing the component to be discoverable when its library
// // is being loaded into a running process.
// RCLCPP_COMPONENTS_REGISTER_NODE(cs4home_core::Flow)

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

namespace cs4home_core
{

/**
 * @class Flow
 * @brief Represents a sequence of nodes within a robotic system, with utilities
 *        to manage and print the node sequence.
 */
class Flow
{
public:
  RCLCPP_SMART_PTR_DEFINITIONS(Flow)

  /**
   * @brief Constructs a Flow object with a specified sequence of nodes.
   * @param nodes A vector of node names to initialize the flow sequence.
   */
  explicit Flow(const std::vector<std::string> & nodes);

  /**
   * @brief Prints the sequence of nodes in the flow to the standard output.
   */
  void print() const;

  /**
   * @brief Retrieves the sequence of nodes in the flow.
   * @return A constant reference to the vector of node names.
   */
  const std::vector<std::string> & get_nodes() const {return nodes_;}

private:
  std::vector<std::string> nodes_; /**< Sequence of nodes in the flow. */
};

}  // namespace cs4home_core

#endif  // CS4HOME_CORE__FLOW_HPP_

// #include "rclcpp_components/register_node_macro.hpp"
//
// // Register the component with class_loader.
// // This acts as a sort of entry point,

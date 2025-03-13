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

Master::Master(const std::string & name, const rclcpp::NodeOptions & options)
: LifecycleNode(name, options)
{
  std::vector<std::string> flows;
  declare_parameter("flows", std::vector<std::string>{});
}

using CallbackReturnT =
  rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn;

CallbackReturnT
Master::on_configure(const rclcpp_lifecycle::State & state)
{
  (void)state;

  std::vector<std::string> flows;
  get_parameter("flows", flows);

  for (const auto & flow: flows) {
    std::vector<std::string> flow_cms;
    declare_parameter(flow, flow_cms);
    get_parameter(flow, flow_cms);

    flows_[flow] = Flow::make_shared(flow, flow_cms);
  }

  return CallbackReturnT::SUCCESS;
}

CallbackReturnT
Master::on_activate(const rclcpp_lifecycle::State & state)
{
  (void)state;

  return CallbackReturnT::SUCCESS;
}

CallbackReturnT
Master::on_deactivate(const rclcpp_lifecycle::State & state)
{
  (void)state;

  return CallbackReturnT::SUCCESS;
}

CallbackReturnT
Master::on_cleanup(const rclcpp_lifecycle::State & state)
{
  (void)state;

  return CallbackReturnT::SUCCESS;
}

CallbackReturnT
Master::on_shutdown(const rclcpp_lifecycle::State & state)
{
  (void)state;

  return CallbackReturnT::SUCCESS;
}

CallbackReturnT
Master::on_error(const rclcpp_lifecycle::State & state)
{
  (void)state;

  return CallbackReturnT::SUCCESS;
}

}  // namespace cs4home_core

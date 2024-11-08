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

#include "ament_index_cpp/get_package_share_directory.hpp"

#include "cs4home_core/Flow.hpp"
#include "cs4home_core/CognitiveModule.hpp"
#include "gtest/gtest.h"

/**
 * @brief Unit test for verifying the creation and structure of Flow instances.
 *
 * This test checks that `Flow` instances are created correctly with the expected
 * sequence of nodes, ensuring that the get_nodes() function returns the correct
 * vector of node names.
 */
TEST(flow_test, flow_creation)
{
  cs4home_core::Flow flow1({"A", "B", "C", "D"});
  cs4home_core::Flow flow2({"A", "B", "C", "E"});

  // Check that the nodes in flow1 match the expected sequence
  ASSERT_EQ(flow1.get_nodes(), std::vector<std::string>({"A", "B", "C", "D"}));
  // Check that the nodes in flow2 match the expected sequence
  ASSERT_EQ(flow2.get_nodes(), std::vector<std::string>({"A", "B", "C", "E"}));
}

/**
 * @brief Main function for running all GoogleTest unit tests.
 *
 * Initializes GoogleTest and ROS 2, then runs all tests defined in the executable.
 *
 * @param argc Argument count
 * @param argv Argument vector
 * @return int Test run result (0 if all tests passed, non-zero otherwise)
 */
int main(int argc, char ** argv)
{
  testing::InitGoogleTest(&argc, argv);
  rclcpp::init(argc, argv);

  return RUN_ALL_TESTS();
}

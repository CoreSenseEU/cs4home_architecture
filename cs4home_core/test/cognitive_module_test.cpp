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

#include <optional>

#include "sensor_msgs/msg/image.hpp"
#include "vision_msgs/msg/detection2_d_array.hpp"
#include "lifecycle_msgs/msg/state.hpp"

#include "ament_index_cpp/get_package_share_directory.hpp"

#include "cs4home_core/Afferent.hpp"
#include "cs4home_core/CognitiveModule.hpp"
#include "gtest/gtest.h"

/**
 * @brief Loads a ROS 2 component dynamically.
 * @tparam T Component type.
 * @param name Component library name.
 * @param parent Lifecycle node parent for the component.
 * @return Tuple with the loaded component shared pointer and error message.
 */
template<class T> std::tuple<typename T::SharedPtr, std::string>
load_component(
  const std::string & name, rclcpp_lifecycle::LifecycleNode::SharedPtr parent)
{
  std::string lib_name = "lib" + name + ".so";
  void * handle = dlopen(lib_name.c_str(), RTLD_LAZY);
  if (!handle) {
    return {nullptr, "Cannot open library:" + lib_name};
  }
  using FactoryFunction = typename T::SharedPtr (*)(rclcpp_lifecycle::LifecycleNode::SharedPtr);
  FactoryFunction create_instance = (FactoryFunction)dlsym(handle, "create_instance");
  const char * dlsym_error = dlerror();
  if (dlsym_error) {
    dlclose(handle);
    return {nullptr, std::string("Cannot load symbol 'create_instance': ") + dlsym_error};
  }
  return {create_instance(parent), ""};
}

using namespace std::chrono_literals;

/**
 * @test Verifies the functionality of the afferent component in "on demand" mode.
 *
 * This test sets up an afferent component to operate in "on demand" mode, where it
 * retrieves messages from a queue rather than processing them immediately via a callback.
 * Messages are published to a topic, and the afferent component is configured to pull
 * messages from this queue as needed. The test ensures that the messages are correctly
 * retrieved in order and that once the queue is empty, further retrievals return `nullptr`.
 */
TEST(cognitive_module_test, afferent_on_demand)
{
  // Create main nodes for the test
  auto node = rclcpp_lifecycle::LifecycleNode::make_shared("test_lc_node");
  auto pub_node = rclcpp::Node::make_shared("pub_node");
  auto pub = pub_node->create_publisher<sensor_msgs::msg::Image>("/image", 100);

  // Single-threaded executor to handle callbacks
  rclcpp::executors::SingleThreadedExecutor exe;
  exe.add_node(node->get_node_base_interface());
  exe.add_node(pub_node);

  // Configure topics for afferent component
  std::vector<std::string> topics {"/image"};

  // Load afferent component and verify successful loading
  auto [afferent, error_afferent] = load_component<cs4home_core::Afferent>(
    "simple_image_input", node);
  ASSERT_NE(afferent, nullptr);

  // Set the topics parameter and configure afferent component
  node->set_parameter(rclcpp::Parameter("simple_image_input.topics", topics));
  ASSERT_TRUE(afferent->configure());

  // Publish test messages to the afferent component's subscribed topic
  sensor_msgs::msg::Image msg;
  for (int i = 0; i < 10; i++) {
    msg.header.frame_id = std::to_string(i);
    pub->publish(msg);
    exe.spin_some();
  }

  // Allow time for messages to be processed and enqueued
  auto start = node->now();
  while (node->now() - start < 1s) {
    exe.spin_some();
  }

  // Retrieve and verify messages from the afferent component's queue
  for (int i = 0; i < 10; i++) {
    auto in_msg = afferent->get_msg<sensor_msgs::msg::Image>();
    ASSERT_NE(in_msg, nullptr);
    ASSERT_EQ(i, std::atoi(in_msg->header.frame_id.c_str()));
  }

  // Verify that further retrieval attempts return `nullptr` as the queue is now empty
  auto in_msg = afferent->get_msg<sensor_msgs::msg::Image>();
  ASSERT_EQ(in_msg, nullptr);
}


/**
 * @test Verifies the functionality of the afferent component in callback mode.
 *
 * This test sets up an afferent component to operate in callback mode, where it
 * subscribes to a topic and processes incoming messages by storing them in a vector
 * for verification. The component listens to `/image` messages and ensures that all
 * received messages are processed and accessible through the callback mechanism.
 */
TEST(cognitive_module_test, afferent_on_subscription)
{
  // Create main nodes for the test
  auto node = rclcpp_lifecycle::LifecycleNode::make_shared("test_lc_node");
  auto pub_node = rclcpp::Node::make_shared("pub_node");
  auto pub = pub_node->create_publisher<sensor_msgs::msg::Image>("/image", 100);

  // Single-threaded executor to handle callbacks
  rclcpp::executors::SingleThreadedExecutor exe;
  exe.add_node(node->get_node_base_interface());
  exe.add_node(pub_node);

  // Setup topics and image storage
  std::vector<std::string> topics {"/image"};
  std::vector<std::unique_ptr<rclcpp::SerializedMessage>> images;

  // Load afferent component and verify successful loading
  auto [afferent, error_afferent] = load_component<cs4home_core::Afferent>(
    "simple_image_input", node);
  ASSERT_NE(afferent, nullptr);

  // Set the topics parameter and configure afferent mode
  node->set_parameter(rclcpp::Parameter("simple_image_input.topics", topics));
  afferent->set_mode(cs4home_core::Afferent::CALLBACK);

  ASSERT_EQ(afferent->get_mode(), cs4home_core::Afferent::ONDEMAND);

  afferent->set_mode(
    cs4home_core::Afferent::CALLBACK,
    [&images](std::unique_ptr<rclcpp::SerializedMessage> msg) {
      images.push_back(std::move(msg));
    }
  );
  ASSERT_TRUE(afferent->configure());

  // Publish test messages to the afferent component's subscribed topic
  sensor_msgs::msg::Image msg;
  for (int i = 0; i < 10; i++) {
    msg.header.frame_id = std::to_string(i);
    pub->publish(msg);
    exe.spin_some();
  }

  // Allow time for messages to be processed
  auto start = node->now();
  while (node->now() - start < 1s) {
    exe.spin_some();
  }

  // Verify that 10 messages were received and processed correctly
  ASSERT_EQ(images.size(), 10);
  for (int i = 0; i < 10; i++) {
    auto in_msg = std::move(images[i]);
    ASSERT_NE(in_msg, nullptr);

    rclcpp::Serialization<sensor_msgs::msg::Image> serializer;
    auto typed_msg = std::make_unique<sensor_msgs::msg::Image>();
    serializer.deserialize_message(in_msg.get(), typed_msg.get());

    ASSERT_EQ(i, std::atoi(typed_msg->header.frame_id.c_str()));
  }
}
/**
 * @test Verifies the functionality of the efferent component in publishing messages.
 *
 * This test sets up an efferent component to publish messages on a specified topic.
 * It publishes a series of messages with sequential `frame_id` values and verifies
 * that these messages are received correctly by a subscriber on the same topic.
 * The test ensures that the efferent component is able to correctly configure its
 * publishers and transmit messages to external listeners.
 */
TEST(cognitive_module_test, efferent)
{
  // Create main nodes for the test
  auto node = rclcpp_lifecycle::LifecycleNode::make_shared("test_lc_node");
  auto sub_node = rclcpp::Node::make_shared("sub_node");

  // Storage for received messages
  std::vector<sensor_msgs::msg::Image> images;
  auto sub = sub_node->create_subscription<sensor_msgs::msg::Image>(
    "/image", 100, [&images](sensor_msgs::msg::Image msg) {
      images.push_back(msg);
    });

  // Single-threaded executor to handle callbacks
  rclcpp::executors::SingleThreadedExecutor exe;
  exe.add_node(node->get_node_base_interface());
  exe.add_node(sub_node);

  // Configure topics for efferent component
  std::vector<std::string> topics {"/image"};

  // Load efferent component and verify successful loading
  auto [efferent, error_efferent] = load_component<cs4home_core::Efferent>(
    "simple_image_output", node);
  ASSERT_NE(efferent, nullptr);

  // Set the topics parameter and configure efferent component
  node->set_parameter(rclcpp::Parameter("simple_image_output.topics", topics));
  ASSERT_TRUE(efferent->configure());

  // Publish test messages via the efferent component
  for (int i = 0; i < 10; i++) {
    auto msg = std::make_unique<sensor_msgs::msg::Image>();
    msg->header.frame_id = std::to_string(i);
    efferent->publish(std::move(msg));
    exe.spin_some();
  }

  // Allow time for messages to be processed
  auto start = node->now();
  while (node->now() - start < 1s) {
    exe.spin_some();
  }

  // Verify that 10 messages were received and processed correctly
  ASSERT_EQ(images.size(), 10);
  for (int i = 0; i < 10; i++) {
    ASSERT_EQ(i, std::atoi(images[i].header.frame_id.c_str()));
  }
}


/**
 * @test Verifies the core component's behavior when processing incoming messages.
 *
 * This test sets up an afferent component to receive messages, a core component
 * to process them by doubling the `frame_id` in each message header, and an
 * efferent component to republish the processed messages. The test publishes a
 * series of messages, each with a sequential `frame_id`, and verifies that the
 * processed messages in the efferent component have doubled `frame_id` values.
 */
TEST(cognitive_module_test, core)
{
  auto node = rclcpp_lifecycle::LifecycleNode::make_shared("test_lc_node");
  auto pub_node = rclcpp::Node::make_shared("pub_node");
  auto sub_node = rclcpp::Node::make_shared("sub_node");

  // Publisher on /in_image and subscription to processed messages on /out_image.
  auto pub = pub_node->create_publisher<sensor_msgs::msg::Image>("/in_image", 100);

  std::vector<sensor_msgs::msg::Image> images;
  auto sub = sub_node->create_subscription<sensor_msgs::msg::Image>(
    "/out_image", 100, [&images](sensor_msgs::msg::Image msg) {
      images.push_back(msg);
    });

  rclcpp::executors::SingleThreadedExecutor exe;
  exe.add_node(node->get_node_base_interface());
  exe.add_node(pub_node);
  exe.add_node(sub_node);

  // Setup topics for afferent and efferent components
  std::vector<std::string> in_topics {"/in_image"};
  std::vector<std::string> out_topics {"/out_image"};

  // Load components
  auto [afferent, error_afferent] = load_component<cs4home_core::Afferent>(
    "simple_image_input", node);
  ASSERT_NE(afferent, nullptr);
  auto [efferent, error_efferent] = load_component<cs4home_core::Efferent>(
    "simple_image_output", node);
  ASSERT_NE(efferent, nullptr);
  auto [core, error_core] = load_component<cs4home_core::Core>(
    "image_filter", node);
  ASSERT_NE(core, nullptr);

  // Set parameters and configure components
  node->set_parameter(rclcpp::Parameter("simple_image_input.topics", in_topics));
  node->set_parameter(rclcpp::Parameter("simple_image_output.topics", out_topics));
  ASSERT_TRUE(afferent->configure());
  ASSERT_TRUE(efferent->configure());
  core->set_afferent(afferent);
  core->set_efferent(efferent);
  ASSERT_TRUE(core->configure());
  ASSERT_TRUE(core->activate());

  // Publish messages to be processed by core component
  sensor_msgs::msg::Image msg;
  for (int i = 0; i < 10; i++) {
    msg.header.frame_id = std::to_string(i);
    pub->publish(msg);
    exe.spin_some();
  }

  auto start = node->now();
  while (node->now() - start < 1s) {
    exe.spin_some();
  }

  ASSERT_TRUE(core->deactivate());

  // Verify that the messages were processed with doubled frame_id values
  ASSERT_EQ(images.size(), 10);
  for (int i = 0; i < 10; i++) {
    ASSERT_EQ(i * 2, std::atoi(images[i].header.frame_id.c_str()));
  }
}

/**
 * @test Verifies the core component with callback functionality (core_cb) processes incoming
 * messages and republishes them with doubled `frame_id` values.
 *
 * This test sets up an afferent component in callback mode to receive messages, a core
 * component with a callback (`core_cb`) to process the messages by doubling the `frame_id`
 * in each message header, and an efferent component to republish the processed messages.
 * The test publishes a sequence of messages and verifies that the processed messages have
 * doubled `frame_id` values.
 */
TEST(cognitive_module_test, core_cb)
{
  auto node = rclcpp_lifecycle::LifecycleNode::make_shared("test_lc_node");
  auto pub_node = rclcpp::Node::make_shared("pub_node");
  auto sub_node = rclcpp::Node::make_shared("sub_node");

  // Publisher on /in_image and subscription to processed messages on /out_image.
  auto pub = pub_node->create_publisher<sensor_msgs::msg::Image>("/in_image", 100);

  std::vector<sensor_msgs::msg::Image> images;
  auto sub = sub_node->create_subscription<sensor_msgs::msg::Image>(
    "/out_image", 100, [&images](sensor_msgs::msg::Image msg) {
      images.push_back(msg);
    });

  rclcpp::executors::SingleThreadedExecutor exe;
  exe.add_node(node->get_node_base_interface());
  exe.add_node(pub_node);
  exe.add_node(sub_node);

  // Setup topics for afferent and efferent components
  std::vector<std::string> in_topics {"/in_image"};
  std::vector<std::string> out_topics {"/out_image"};

  // Load components
  auto [afferent, error_afferent] = load_component<cs4home_core::Afferent>(
    "simple_image_input", node);
  ASSERT_NE(afferent, nullptr);
  auto [efferent, error_efferent] = load_component<cs4home_core::Efferent>(
    "simple_image_output", node);
  ASSERT_NE(efferent, nullptr);
  auto [core, error_core] = load_component<cs4home_core::Core>(
    "image_filter_cb", node);
  ASSERT_NE(core, nullptr);

  // Set parameters and configure components
  node->set_parameter(rclcpp::Parameter("simple_image_input.topics", in_topics));
  node->set_parameter(rclcpp::Parameter("simple_image_output.topics", out_topics));
  ASSERT_TRUE(afferent->configure());
  ASSERT_TRUE(efferent->configure());
  core->set_afferent(afferent);
  core->set_efferent(efferent);
  ASSERT_TRUE(core->configure());
  ASSERT_TRUE(core->activate());

  // Publish messages to be processed by core_cb component
  sensor_msgs::msg::Image msg;
  for (int i = 0; i < 10; i++) {
    msg.header.frame_id = std::to_string(i);
    pub->publish(msg);
    exe.spin_some();
  }

  auto start = node->now();
  while (node->now() - start < 1s) {
    exe.spin_some();
  }

  ASSERT_TRUE(core->deactivate());

  // Verify that the messages were processed with doubled frame_id values
  ASSERT_EQ(images.size(), 10);
  for (int i = 0; i < 10; i++) {
    ASSERT_EQ(i * 2, std::atoi(images[i].header.frame_id.c_str()));
  }
}


/**
 * @test Tests the initialization and basic functionality of the CognitiveModule using a configuration file.
 *
 * This test verifies that the CognitiveModule initializes correctly from a configuration file
 * and transitions through the ROS 2 lifecycle states (configure, activate, deactivate).
 * It sets up publishers and subscribers to check that messages are processed with expected modifications
 * (doubling of `frame_id`) and are received correctly after processing.
 */
TEST(cognitive_module_test, startup_simple)
{
  // Obtain the path to the configuration file
  std::string pkgpath = ament_index_cpp::get_package_share_directory("cs4home_core");
  std::string config_file = pkgpath + "/config/startup_simple_1.yaml";

  rclcpp::NodeOptions options;
  options.arguments(
    {"--ros-args", "--params-file", config_file});


  // Instantiate the CognitiveModule using the specified configuration file
  auto cm1 = cs4home_core::CognitiveModule::make_shared("cognitive_module_1", options);

  ASSERT_EQ(std::string(cm1->get_name()), "cognitive_module_1");

  // Verify the number of parameters loaded from the configuration file
  auto params = cm1->list_parameters({}, 0);
  ASSERT_EQ(params.names.size(), 7u);

  // Set up publisher and subscriber nodes for testing message processing
  auto pub_node = rclcpp::Node::make_shared("pub_node");
  auto sub_node = rclcpp::Node::make_shared("sub_node");

  auto pub = pub_node->create_publisher<sensor_msgs::msg::Image>("/image_raw", 100);

  std::vector<sensor_msgs::msg::Image> images;
  auto sub = sub_node->create_subscription<sensor_msgs::msg::Image>(
    "/detections", 100, [&images](sensor_msgs::msg::Image msg) {
      images.push_back(msg);
    });

  rclcpp::executors::SingleThreadedExecutor exe;
  exe.add_node(cm1->get_node_base_interface());
  exe.add_node(pub_node);
  exe.add_node(sub_node);

  // Transition the CognitiveModule through the lifecycle states
  cm1->trigger_transition(lifecycle_msgs::msg::Transition::TRANSITION_CONFIGURE);
  ASSERT_EQ(cm1->get_current_state().id(), lifecycle_msgs::msg::State::PRIMARY_STATE_INACTIVE);

  cm1->trigger_transition(lifecycle_msgs::msg::Transition::TRANSITION_ACTIVATE);
  ASSERT_EQ(cm1->get_current_state().id(), lifecycle_msgs::msg::State::PRIMARY_STATE_ACTIVE);

  // Publish a series of messages to test processing by the CognitiveModule
  sensor_msgs::msg::Image msg;
  for (int i = 0; i < 10; i++) {
    msg.header.frame_id = std::to_string(i);
    pub->publish(msg);
    exe.spin_some();
  }

  auto start = cm1->now();
  while (cm1->now() - start < 1s) {
    exe.spin_some();
  }

  // Transition the module back to inactive state
  cm1->trigger_transition(lifecycle_msgs::msg::Transition::TRANSITION_DEACTIVATE);
  ASSERT_EQ(cm1->get_current_state().id(), lifecycle_msgs::msg::State::PRIMARY_STATE_INACTIVE);

  // Verify that the messages were processed correctly with doubled frame_id values
  ASSERT_EQ(images.size(), 10);
  for (int i = 0; i < 10; i++) {
    ASSERT_EQ(i * 2, std::atoi(images[i].header.frame_id.c_str()));
  }
}

int main(int argc, char ** argv)
{
  testing::InitGoogleTest(&argc, argv);
  rclcpp::init(argc, argv);

  return RUN_ALL_TESTS();
}

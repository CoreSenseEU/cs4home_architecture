#include "cs4home_core/Master.hpp"
#include "cs4home_core/Flow.hpp"
#include "lifecycle_msgs/srv/change_state.hpp"
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_lifecycle/lifecycle_node.hpp"
#include "sound_msgs/msg/sound_detection.hpp"
#include "std_msgs/msg/bool.hpp"

using namespace std::chrono_literals;
using namespace std::placeholders;

class SoundMaster : public cs4home_core::Master {
public:
  explicit SoundMaster(
      const rclcpp::NodeOptions &options = rclcpp::NodeOptions())
      : cs4home_core::Master("sound_master", options) {
    RCLCPP_INFO(this->get_logger(), "SoundMaster initialized");
  }

  // using CallbackReturnT =
  //     rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn;

  // CallbackReturnT on_configure(const rclcpp_lifecycle::State &state) override
  // {

  //   auto result = cs4home_core::Master::on_configure(state);

  //   if (result == CallbackReturnT::SUCCESS) {

  //     sound_detections_sub_ =
  //         this->create_subscription<sound_msgs::msg::SoundDetection>(
  //             "/sound_detection", 20,
  //             std::bind(&SoundMaster::sound_callback, this, _1));
  //     sound_context_sub_ = this->create_subscription<std_msgs::msg::Bool>(
  //         "/sound_controlled", 20,
  //         std::bind(&SoundMaster::context_callback, this, _1));
  //   }

  //   return result;
  // }

  CallbackReturnT on_activate(const rclcpp_lifecycle::State &state) override {
    if (!flows_.empty()) {

      for (const auto &flow : flows_) {

        std::string key = flow.first;
        auto value = flow.second;

        RCLCPP_INFO(this->get_logger(), "%s", key.c_str());

        for (const auto &node : value->get_flow()) {
          RCLCPP_INFO(this->get_logger(), "%s", node.c_str());
        }
      }

    } else {
      RCLCPP_WARN(this->get_logger(), "No flow :(");
    }
    return CallbackReturnT::SUCCESS;
  }

  // CallbackReturnT on_deactivate(const rclcpp_lifecycle::State &state)
  // override {
  //   return CallbackReturnT::SUCCESS;
  // }

private:
  // rclcpp::Subscription<sound_msgs::msg::SoundDetection>::SharedPtr
  //     sound_detections_sub_;
  // rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr sound_context_sub_;
  // bool sound_controlled_ = std::nan("");
  // rclcpp::Client<lifecycle_msgs::srv::ChangeState>::SharedPtr
  //     change_state_client_;

  // void sound_callback(const sound_msgs::msg::SoundDetection::SharedPtr msg) {
  //   // if msg->type environment do nothing
  //   // if msg->type emergency add_activation(emergency_action)
  //   if (msg->type == "emergency") {
  //     change_node_state("emergency_action",
  //                       lifecycle_msgs::msg::State::PRIMARY_STATE_ACTIVE);
  //   }
  //   // if msg->type supervised add_activation(supervision) cuando se
  //   desactive
  //   // activar sound_context_evaluation si sound_controlled true
  //   // activar(human_assistance) y si no activar(emergency_action)
  // }
  // void context_callback(const std_msgs::msg::Bool::SharedPtr msg) {
  //   sound_controlled_ = msg->data;
  // }

  // void change_node_state(const std::string &node_name, uint8_t target_state)
  // {

  //   change_state_client_ =
  //       this->create_client<lifecycle_msgs::srv::ChangeState>("/" + node_name
  //       +
  //                                                             "/change_state");

  //   auto request =
  //       std::make_shared<lifecycle_msgs::srv::ChangeState::Request>();
  //   request->transition.id = target_state;

  //   auto future = change_state_client_->async_send_request(request);
  //   try {
  //     auto result = future.get();
  //     RCLCPP_INFO(this->get_logger(), "Node %s state changed to %u",
  //                 node_name.c_str(), target_state);
  //   } catch (const std::exception &e) {
  //     RCLCPP_ERROR(this->get_logger(), "Failed to change state for node %s:
  //     %s",
  //                  node_name.c_str(), e.what());
  //   }
  // }
};
int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<SoundMaster>();
  rclcpp::spin(node->get_node_base_interface());
  rclcpp::shutdown();
  return 0;
}

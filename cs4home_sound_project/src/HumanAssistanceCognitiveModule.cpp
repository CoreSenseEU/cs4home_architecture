#include "cs4home_core/CognitiveModule.hpp"
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_lifecycle/lifecycle_node.hpp"

class HumanAssistanceModuleCognitive : public cs4home_core::CognitiveModule {
public:
  explicit HumanAssistanceModuleCognitive(
      const rclcpp::NodeOptions &options = rclcpp::NodeOptions())
      : cs4home_core::CognitiveModule("human_assistance_module_cognitive",
                                      options) {
    RCLCPP_INFO(this->get_logger(),
                "HumanAssistanceModuleCognitive initialized");
  }
};

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<HumanAssistanceModuleCognitive>();
  rclcpp::spin(node->get_node_base_interface());
  rclcpp::shutdown();
  return 0;
}

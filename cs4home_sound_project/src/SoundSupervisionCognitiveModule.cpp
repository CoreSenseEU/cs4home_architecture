#include "cs4home_core/CognitiveModule.hpp"
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_lifecycle/lifecycle_node.hpp"

class SoundSupervisionCognitiveModule : public cs4home_core::CognitiveModule {
public:
  explicit SoundSupervisionCognitiveModule(
      const rclcpp::NodeOptions &options = rclcpp::NodeOptions())
      : cs4home_core::CognitiveModule("sound_supervision_module_cognitive",
                                      options) {
    RCLCPP_INFO(this->get_logger(),
                "SoundSupervisionCognitiveModule initialized");
  }
};

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<SoundSupervisionCognitiveModule>();
  rclcpp::spin(node->get_node_base_interface());
  rclcpp::shutdown();
  return 0;
}

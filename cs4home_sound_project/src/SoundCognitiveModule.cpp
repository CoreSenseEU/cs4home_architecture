#include "cs4home_core/CognitiveModule.hpp"
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_lifecycle/lifecycle_node.hpp"

class SoundModuleCognitive : public cs4home_core::CognitiveModule {
public:
  explicit SoundModuleCognitive(
      const rclcpp::NodeOptions &options = rclcpp::NodeOptions())
      : cs4home_core::CognitiveModule("sound_module_cognitive", options) {
    RCLCPP_INFO(this->get_logger(), "SoundModuleCognitive initialized");

    // Suscribirse a datos de audio o inicializar componentes específicos
  }

  void process_sound_data() {
    RCLCPP_INFO(this->get_logger(), "Processing sound data...");
    // Implementar lógica específica de procesamiento de sonido aquí
  }
};

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<SoundModuleCognitive>();
  rclcpp::spin(node->get_node_base_interface());
  rclcpp::shutdown();
  return 0;
}

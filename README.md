# cs4home_architecture

This work presents a model for the integration and management of the functional components of a general robotic system, using ROS 2 as a technical foundation. The proposal is inspired by the organization of the human nervous system, developing a management metamodel based on neuroregulatory centers and structured into afferent and efferent components that facilitate information flow and processing within the robotic system.

The model introduces a service-oriented management approach adapted to the distributed environment of ROS 2. This enables the creation and coordination of functional entities according to principles inspired by the human neuroregulatory system, where afferent components gather and process data from the environment, while efferent components distribute and execute commands, using ROS 2 communication patterns such as publish-subscribe, services, and actions.

This structure addresses traditional challenges in robotics, such as hardware-business logic coupling, the need for rapid development of robotic systems, and the complexity of incorporating new knowledge and technologies into existing systems. Key features of this model include:

- Uniform management of system components: Functional elements are managed in a homogeneous way, where each component contributes from a modular and functional perspective. Using ROS 2, interoperability and modular control are achieved, allowing the simplified integration of afferent and efferent components within the system.

- Metamodel inspired by the human neuroregulatory system: The organization of components follows dynamics similar to the nervous system, using ROS 2 nodes and their lifecycle to manage activations and deactivations. Afferent nodes capture sensory data and external signals, while efferent nodes act on the system based on decisions made, allowing controlled and adaptive responses.

- Integration of functional entities via ROS 2 technologies: The proposal incorporates ROS 2's publish-subscribe, service, and action techniques for distributed integration of functional entities, facilitating dynamic and adaptable component connections.

This model is designed as an adaptable solution for a wide variety of robotic systems based on ROS 2, from low-level controls to complex inter-robot coordination and communication. This adaptability allows the scaling and distribution of components across diverse architectures, facilitating the incorporation of new functionalities without the need for redesign.

## Installation

```bash
cd ~/ros2_ws/src
git clone -b humble https://github.com/igonzf/cs4home_architecture.git
vcs import --recursive < cs4home_architecture/CoreSense4Home/robocup_bringup/thirdparty.repos
cd ~/ros2_ws
colcon build --symlink-install
ros2 launch cs4home_sound_project launch_sound.launch.py
```

## Simulator

If needed, you can use the RB1 robot simulation:

git clone -b mic-array-urdf https://github.com/igonzf/ros2_rb1.git

## Creating a Cognitive Module

Each cognitive module is defined through configuration and implemented using the base classes provided by the architecture.

### 1. Define the module in YAML

Edit the file:

cs4home_sound_project/config/params.yaml

Example:

```yaml
sound_recognition:
  ros_parameters:
    core: sound_context_evaluation
    afferent: sound_input
    sound_input:
      topics: ["/audio", "/doa"]
      types:
        ["audio_common_msgs/msg/AudioData", "geometry_msgs/msg/PoseStamped"]
    efferent: sound_context_output
    sound_context_output:
      topics: ["/sound_context", "/context_markers"]
      types: ["sound_msgs/msg/SoundContext", "visualization_msgs/msg/Marker"]
    meta: sound_meta
    coupling: sound_coupling
```

Fields:

- `core`: name of the main processing node
- `afferent`: input handler (subscribes to topics)
- `efferent`: output handler (publishes results)
- `meta`: module metadata manager
- `coupling`: logic that connects with other modules

### 2. Implement the Module

Create a class that inherits from cs4home_core::CognitiveModule:

```cpp
class SoundModuleCognitive : public cs4home_core::CognitiveModule {
// Instantiate lifecycle components here
};
```

Define each component as a subclass:

```cpp
class AudioInput : public cs4home_core::Afferent {};
class AudioOutput : public cs4home_core::Efferent {};
class DefaultCoupling : public cs4home_core::Coupling {};
class AudioMeta : public cs4home_core::Meta {};

class SoundRecognition : public cs4home*core::Core {
void process() override {
    // Access afferent input by index
    auto msg_audio = afferent*->get_msg<audio_common_msgs::msg::AudioData>(0);

    // Or by topic name
    auto msg_doa = afferent_->get_msg<geometry_msgs::msg::PoseStamped>("/doa");

    // Publish processed result
    efferent_->publish(0, sound);

}
};
```

Each component is managed as a ROS 2 Lifecycle Node and instantiated according to the YAML configuration.

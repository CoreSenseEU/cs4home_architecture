# cs4home_architecture

This work presents a model for the integration and management of the functional components of a general robotic system, using ROS 2 as a technical foundation. The proposal is inspired by the organization of the human nervous system, developing a management metamodel based on neuroregulatory centers and structured into afferent and efferent components that facilitate information flow and processing within the robotic system.

The model introduces a service-oriented management approach adapted to the distributed environment of ROS 2. This enables the creation and coordination of functional entities according to principles inspired by the human neuroregulatory system, where afferent components gather and process data from the environment, while efferent components distribute and execute commands, using ROS 2 communication patterns such as publish-subscribe, services, and actions.

This structure addresses traditional challenges in robotics, such as hardware-business logic coupling, the need for rapid development of robotic systems, and the complexity of incorporating new knowledge and technologies into existing systems. Key features of this model include:

- Uniform management of system components: Functional elements are managed in a homogeneous way, where each component contributes from a modular and functional perspective. Using ROS 2, interoperability and modular control are achieved, allowing the simplified integration of afferent and efferent components within the system.
- Metamodel inspired by the human neuroregulatory system: The organization of components follows dynamics similar to the nervous system, using ROS 2 nodes and their lifecycle to manage activations and deactivations. Afferent nodes capture sensory data and external signals, while efferent nodes act on the system based on decisions made, allowing controlled and adaptive responses.
- Integration of functional entities via ROS 2 technologies: The proposal incorporates ROS 2's publish-subscribe, service, and action techniques for distributed integration of functional entities, facilitating dynamic and adaptable component connections.

This model is designed as an adaptable solution for a wide variety of robotic systems based on ROS 2, from low-level controls to complex inter-robot coordination and communication. This adaptability allows the scaling and distribution of components across diverse architectures, facilitating the incorporation of new functionalities without the need for redesign.

## CoreSense role

The terms below follow the [CoreSense Ontology (CSO)](https://w3id.org/coresense/cso).

- A [Cognitive Function](https://w3id.org/coresense/cso#CognitiveFunction) processes information through the module's `Core` component.
- A system obtains a [Cognitive Capability](https://w3id.org/coresense/cso#CognitiveCapability) when that function can be performed successfully under its stated conditions.
- A [Functionality](https://w3id.org/coresense/cso#Functionality) places a capability in a context together with its goals, actions and expected performance.
- The architecture's `Afferent`, `Efferent`, `Meta` and `Coupling` components support the information flow and lifecycle around the `Core`; these software names are kept unchanged.

## Module flow

~~~mermaid
flowchart LR
    source["System or environment information"] --> afferent["Afferent"]
    afferent --> core["Core: cognitive function"]
    core --> efferent["Efferent"]
    efferent --> consumer["Other modules or robot system"]
    meta["Meta"] -. "module information" .-> core
    coupling["Coupling"] -. "module coordination" .-> core
~~~

All components are managed through ROS 2 lifecycle transitions. The package also provides a master/flow mechanism for coordinating several cognitive modules.

## Build

~~~bash
mkdir -p ~/cs4home_ws/src
cd ~/cs4home_ws/src
git clone https://github.com/CoreSenseEU/cs4home_architecture.git
cd ..
rosdep install --from-paths src --ignore-src -r -y
colcon build --symlink-install
source install/setup.bash
~~~

## Documentation

- Doxygen: [https://coresenseeu.github.io/cs4home_architecture/](https://coresenseeu.github.io/cs4home_architecture/)
- General documentation: [https://coresenseeu.github.io/](https://coresenseeu.github.io/)

## Examples

Implementation of the architecture from the Social Testbed point of view.

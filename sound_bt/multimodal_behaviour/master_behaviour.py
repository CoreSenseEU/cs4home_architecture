import rclpy
from rclpy.node import Node
import subprocess
from lifecycle_msgs.srv import GetState
from lifecycle_msgs.srv import ChangeState
from lifecycle_msgs.msg import State
import time
from lifecycle_msgs.msg import Transition

class LifecycleManager(Node):
    def __init__(self):
        super().__init__('lifecycle_manager')
        self.nodes = [
            '/sound_recognition',
            '/emergency_action'
        ]

        self.get_logger().info("Configurando todos los nodos")

        for node in self.nodes:
            self.change_state(node, Transition.TRANSITION_CONFIGURE)
            time.sleep(1)
        
        self.change_state('/sound_recognition', State.PRIMARY_STATE_ACTIVE)
        self.change_state('/emergency_action', State.PRIMARY_STATE_ACTIVE)
        #self.change_state('/sound_supervision', State.PRIMARY_STATE_ACTIVE)

        #state = self.get_state('/sound_supervision')
        
        # Esperar a que /sound_supervision se vuelva INACTIVE
        #self.timer = self.create_timer(1.0, self.check_sound_supervision_state)
    
    def change_state(self, node_name, target_state):
        client = self.create_client(ChangeState, f'{node_name}/change_state')
        if not client.wait_for_service(timeout_sec=5.0):
            self.get_logger().error(f'Service {node_name}/change_state not available')
            return
        
        request = ChangeState.Request()
        request.transition.id = target_state
        
        future = client.call_async(request)
        rclpy.spin_until_future_complete(self, future)
        if future.result() is not None:
            self.get_logger().info(f'Successfully changed state of {node_name} to {target_state}')
        else:
            self.get_logger().error(f'Failed to change state of {node_name}')
    
    def get_state(self, node_name):
        self.get_logger().info(f'Get state {node_name}')
        client = self.create_client(GetState, f'{node_name}/get_state')
        if not client.wait_for_service(timeout_sec=5.0):
            self.get_logger().error(f'Service {node_name}/get_state not available')
            return None

        request = GetState.Request()
        future = client.call_async(request)
        future.add_done_callback(self.state_callback)
    
    def check_sound_supervision_state(self):
        state = self.get_state('/sound_supervision')
        if state == State.PRIMARY_STATE_INACTIVE:
            self.get_logger().info('/sound_supervision is inactive, proceeding with next steps')
            
            self.change_state('/sound_context_evaluation', State.PRIMARY_STATE_ACTIVE)
            self.change_state('/emergency_action', State.PRIMARY_STATE_ACTIVE)
            
            self.destroy_timer(self.timer) 
    def state_callback(self, future):
        """Callback para manejar la respuesta del estado"""
        try:
            result = future.result()
            state_id = result.current_state.id
            self.get_logger().info(f'Estado: {state_id}')

            # Comprobar si el estado es INACTIVE
            if state_id == State.PRIMARY_STATE_INACTIVE:
                self.get_logger().info(f'Node is inactive, proceeding with next steps')
                self.change_state('/sound_context_evaluation', Transition.TRANSITION_ACTIVATE)
                self.change_state('/emergency_action', Transition.TRANSITION_ACTIVATE)
                self.destroy_timer(self.timer)
        except Exception as e:
            self.get_logger().error(f"Error al obtener el estado: {str(e)}")

def main(args=None):
    rclpy.init(args=args)
    node = LifecycleManager()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
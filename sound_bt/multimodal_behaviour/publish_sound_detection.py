import rclpy
from rclpy.node import Node
from sound_msgs.msg import SoundDetection
from geometry_msgs.msg import PoseStamped
from std_msgs.msg import Bool
from geometry_msgs.msg import Point
from geometry_msgs.msg import Quaternion

class SoundDetectionPublisher(Node):

    def __init__(self):
        super().__init__('sound_detection_publisher')
        self.publisher_ = self.create_publisher(SoundDetection, '/sound_detection', 10)
        self.timer = self.create_timer(1.0, self.publish_message)  

    def publish_message(self):
        msg = SoundDetection()

        # Set class_id and class_name
        msg.class_id = 9
        # msg.class_name = 'Vacuum_cleaner'
        msg.class_name = 'Running_water'

        # Set voice and text_data
        msg.voice = Bool(data=False)
        msg.text_data = ""

        # Set sound_location
        sound_location = PoseStamped()
        sound_location.header.stamp = self.get_clock().now().to_msg()
        sound_location.header.frame_id = 'map'  # El marco de referencia es 'map'
        sound_location.pose.position = Point(x=1.1448, y=4.009, z=0.053)
        # sound_location.pose.position = Point(x=4.520, y=6.511, z=0.003)
        sound_location.pose.orientation = Quaternion(x=0.0, y=0.0, z=0.0, w=1.0)

        msg.sound_location = sound_location

        self.publisher_.publish(msg)
        self.get_logger().info('Publishing: "%s"' % msg)

def main(args=None):
    rclpy.init(args=args)
    sound_detection_publisher = SoundDetectionPublisher()
    rclpy.spin(sound_detection_publisher)
    sound_detection_publisher.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()

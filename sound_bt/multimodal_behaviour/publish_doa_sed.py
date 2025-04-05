import rclpy
from rclpy.node import Node
from sound_msgs.msg import SoundDetection, SoundEventDetection
from geometry_msgs.msg import PoseStamped
from std_msgs.msg import Bool
from geometry_msgs.msg import Point
from geometry_msgs.msg import Quaternion

class SoundDetectionPublisher(Node):

    def __init__(self):
        super().__init__('sound_detection_publisher')
        self.publisher_doa = self.create_publisher(PoseStamped, '/doa', 10)
        self.publisher_sed = self.create_publisher(SoundEventDetection, '/sed', 10)
        self.timer = self.create_timer(1.0, self.publish_message) 

    def publish_message(self):
        msg_sed = SoundEventDetection()

        # Set class_id and class_name
        msg_sed.class_id = 9
        # msg.class_name = 'Vacuum_cleaner'
        msg_sed.class_name = 'Alarm_bell_ringing'

        self.publisher_sed.publish(msg_sed)
        self.get_logger().info('Publishing: "%s"' % msg_sed)

        # Set sound_location
        sound_location = PoseStamped()
        sound_location.header.stamp = self.get_clock().now().to_msg()
        sound_location.header.frame_id = 'map' 

        sound_location.pose.orientation = Quaternion(x=0.0, y=0.0, z=0.0, w=1.0)
        sound_location.pose.position = Point(x=1.38, y=4.478, z=0.053)
        # sound_location.pose.position = Point(x=1.1448, y=4.009, z=0.053)
        # sound_location.pose.position = Point(x=4.520, y=6.511, z=0.003)

        self.publisher_doa.publish(sound_location)
        self.get_logger().info('Publishing: "%s"' % sound_location)

def main(args=None):
    rclpy.init(args=args)
    sound_detection_publisher = SoundDetectionPublisher()
    rclpy.spin(sound_detection_publisher)
    sound_detection_publisher.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()

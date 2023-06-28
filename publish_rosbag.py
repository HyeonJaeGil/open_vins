import rosbag
import rospy
from sensor_msgs.msg import Image, Imu, CameraInfo, CompressedImage
import getch


cam_pub = rospy.Publisher('/camera0/image_raw/compressed', CompressedImage, queue_size=10)
imu_pub = rospy.Publisher('/imu0', Imu, queue_size=10)
camimu_pub = rospy.Publisher('/camera/imu0', Imu, queue_size=10)
cam_info_pub = rospy.Publisher('/camera0/color/camera_info', CameraInfo, queue_size=10)
depth_pub = rospy.Publisher('/camera0/aligned_depth_to_color/image_raw', Image, queue_size=10)

class TimeWithTopicMsg:
    def __init__(self, timestamp, topic, msg):
        self.timestamp = timestamp
        self.topic = topic
        self.msg = msg

    def __lt__(self, other):
        return self.timestamp < other.timestamp

    def __repr__(self):
        return f"TimeWithTopicMsg({self.timestamp}, {self.topic}, {self.msg})"
            
        
def publish_topic(topic, msg):
    if topic == '/camera/color/image_raw/compressed':
        cam_pub.publish(msg)
    elif topic == '/imu':
        imu_pub.publish(msg)
    elif topic == '/camera/imu':
        camimu_pub.publish(msg)
    elif topic == '/camera/color/camera_info':
        cam_info_pub.publish(msg)
    elif topic == '/camera/aligned_depth_to_color/image_raw':
        depth_pub.publish(msg)
    print(f"{msg.header.stamp.secs}.{msg.header.stamp.nsecs}: Published {topic}")
    

def process_rosbag(bag_file):
    bag = rosbag.Bag(bag_file)
    topics = bag.get_type_and_topic_info()[1].keys()
    print(topics)

    # iterate through all messages in the bag
    # and store them in a list of TimeWithTopicMsg
    # so that we can sort them by timestamp
    # and publish them in order
    messages = []
    for topic, msg, t in bag.read_messages(topics=topics):
        messages.append(TimeWithTopicMsg(t, topic, msg))
    messages.sort()
    for message in messages:
        print(f"{message.timestamp}: {message.topic}")

    # publish the messages in order, until the user presses 'q'
    for msg in messages:
        publish_topic(msg.topic, msg.msg)
        char = getch.getch()
        if char == 'q':
            break

    bag.close()

if __name__ == '__main__':
    bag_filepath = "/media/hj/T7/HJ_Secret_Diary/all_2023-05-25-18-28-57.bag"
    rospy.init_node('rosbag_publisher')
    process_rosbag(bag_filepath)

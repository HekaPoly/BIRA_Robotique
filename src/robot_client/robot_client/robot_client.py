import sys

from bira_msgs.srv import PlanTrajectory
from sensor_msgs.msg import JointState
from geometry_msgs.msg import PoseStamped

import rclpy
from rclpy.node import Node

import asyncio

async def spin_until_complete(node, future):
    while rclpy.ok() and not future.done():
        rclpy.spin_once(node, timeout_sec=0.1)
        await asyncio.sleep(0)


class RobotClient(Node):

    def __init__(self):
        super().__init__('robot_client')
        # TODO: adapt to service name and type
        self.cli = self.create_client(PlanTrajectory, 'plan_trajectory')
        
        while not self.cli.wait_for_service(timeout_sec=1.0):
            self.get_logger().info('service not available, waiting again...')
        self.req = PlanTrajectory.Request()

    def send_request(self, a, b):
        joint_state = JointState()
        joint_state.name = ['joint1', 'joint2', 'joint3']
        joint_state.position = [0.0, 0.5, -0.5]
        joint_state.velocity = []
        joint_state.effort = []

        pose = PoseStamped()
        pose.header.frame_id = 'base_link'

        pose.pose.position.x = 0.4
        pose.pose.position.y = 0.1
        pose.pose.position.z = 0.3

        pose.pose.orientation.x = 0.0
        pose.pose.orientation.y = 0.0
        pose.pose.orientation.z = 0.0
        pose.pose.orientation.w = 1.0

        self.req.start_joints = joint_state
        self.req.final_pos = pose

        self.future = self.cli.call_async(self.req)
        # print("Is awaitable: ", inspect.isawaitable(self.future))
        # rclpy.spin_until_future_complete(self, self.future)
        return self.future


async def main():
    rclpy.init()
    node = RobotClient()

    future = node.send_request(1, 2)
    await spin_until_complete(node, future)

    response = future.result()
    node.get_logger().info(f"Result: {response.traj}")

    node.destroy_node()
    rclpy.shutdown()



if __name__ == '__main__':
    asyncio.run(main())
    #main()
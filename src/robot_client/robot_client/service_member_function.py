from bira_msgs.srv import PlanTrajectory

import rclpy
from rclpy.node import Node


from sensor_msgs.msg import JointState
from geometry_msgs.msg import PoseStamped
from moveit_msgs.msg import MotionPlanResponse, RobotTrajectory
from trajectory_msgs.msg import JointTrajectory, JointTrajectoryPoint


class MinimalService(Node):

    def __init__(self):
        super().__init__('minimal_service')
        self.srv = self.create_service(PlanTrajectory, 'plan_trajectory', self.dummy_callback)
        self.get_logger().info('Created service')

    def dummy_callback(self, request, response):
        self.get_logger().info('Received request')

        # =========================
        # Trajectoire bidon
        # =========================
        traj = RobotTrajectory()
        joint_traj = JointTrajectory()

        joint_traj.joint_names = ['joint1', 'joint2', 'joint3']

        # Point 1
        p1 = JointTrajectoryPoint()
        p1.positions = [0.0, 0.0, 0.0]
        p1.time_from_start.sec = 1

        # Point 2
        p2 = JointTrajectoryPoint()
        p2.positions = [0.5, 0.2, -0.2]
        p2.time_from_start.sec = 2

        # Point 3
        p3 = JointTrajectoryPoint()
        p3.positions = [1.0, 0.5, -0.5]
        p3.time_from_start.sec = 3

        joint_traj.points = [p1, p2, p3]
        traj.joint_trajectory = joint_traj

        # =========================
        # MotionPlanResponse bidon
        # =========================
        motion_response = MotionPlanResponse()
        motion_response.trajectory = traj

        # (optionnel mais propre)
        motion_response.error_code.val = 1  # SUCCESS

        # =========================
        # Remplir la réponse
        # =========================
        response.traj = motion_response

        self.get_logger().info('Returning fake trajectory')

        return response



def main(args=None):
    rclpy.init(args=args)

    minimal_service = MinimalService()

    rclpy.spin(minimal_service)

    rclpy.shutdown()


if __name__ == '__main__':
    main()
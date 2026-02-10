import sys

from example_interfaces.srv import AddTwoInts
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
        self.cli = self.create_client(AddTwoInts, 'add_two_ints')
        
        while not self.cli.wait_for_service(timeout_sec=1.0):
            self.get_logger().info('service not available, waiting again...')
        self.req = AddTwoInts.Request()

    def send_request(self, a, b):
        # TODO: adapt to service request
        self.req.a = a
        self.req.b = b
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
    node.get_logger().info(f"Result: {response.sum}")

    node.destroy_node()
    rclpy.shutdown()



if __name__ == '__main__':
    asyncio.run(main())
    #main()
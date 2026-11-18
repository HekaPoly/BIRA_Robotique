#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>
#include <trajectory_msgs/msg/joint_trajectory.hpp>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <memory>

class MotorControlNode : public rclcpp::Node {
public:
    MotorControlNode() : Node("motor_control_node") {
        publisher = create_publisher<std_msgs::msg::String>(
            "/uart_tx", 10
        );

        trajectorySubscriber =
            create_subscription<trajectory_msgs::msg::JointTrajectory>(
                "/motor_trajectory",
                10,
                [this](trajectory_msgs::msg::JointTrajectory::SharedPtr msg) {
                    receiveTrajectory(msg);
                }
            );

        uartSubscriber =
            create_subscription<std_msgs::msg::String>(
                "/uart_rx",
                10,
                [this](std_msgs::msg::String::SharedPtr msg) {
                    receiveResponse(msg);
                }
            );

        RCLCPP_INFO(get_logger(), "Motor control initialise");
    }

private:
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher;

    rclcpp::Subscription<
        trajectory_msgs::msg::JointTrajectory
    >::SharedPtr trajectorySubscriber;

    rclcpp::Subscription<
        std_msgs::msg::String
    >::SharedPtr uartSubscriber;

    void receiveTrajectory(
        trajectory_msgs::msg::JointTrajectory::SharedPtr msg
    ) {
        if (msg->points.empty()) {
            return;
        }

        if (msg->joint_names.size() != 6) {
            return;
        }

        auto point = msg->points.back();

        if (point.positions.size() != 6) {
            return;
        }

        std::stringstream command;

        command << "T";
        command << std::fixed << std::setprecision(6);

        for (double position : point.positions) {
            if (!std::isfinite(position)) {
                return;
            }

            command << " " << position;
        }

        std_msgs::msg::String message;

        message.data = command.str();

        publisher->publish(message);

        RCLCPP_INFO(
            get_logger(),
            "Trajectoire de test transmise"
        );
    }

    void receiveResponse(
        std_msgs::msg::String::SharedPtr msg
    ) {
        RCLCPP_INFO(
            get_logger(),
            "Reponse du STM32 : %s",
            msg->data.c_str()
        );
    }
};

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);

    auto node = std::make_shared<MotorControlNode>();

    rclcpp::spin(node);

    rclcpp::shutdown();

    return 0;
}
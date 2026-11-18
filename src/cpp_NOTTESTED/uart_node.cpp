#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>
#include "bira_motor_control/uart.hpp"
#include <chrono>
#include <memory>
#include <string>
#include <stdexcept>

using namespace std::chrono_literals;

class UARTNode : public rclcpp::Node {
public:
    UARTNode() : Node("uart_node") {
        std::string port = declare_parameter<std::string>(
            "port", "/dev/ttyUSB0"
        );

        if (!uart.openPort(port)) {
            throw std::runtime_error(
                "Impossible d'ouvrir le port UART"
            );
        }

        publisher = create_publisher<std_msgs::msg::String>(
            "/uart_rx", 10
        );

        subscriber = create_subscription<std_msgs::msg::String>(
            "/uart_tx",
            10,
            [this](std_msgs::msg::String::SharedPtr msg) {
                sendCommand(msg);
            }
        );

        timer = create_wall_timer(
            20ms,
            [this]() {
                receiveData();
            }
        );

        RCLCPP_INFO(get_logger(), "UART initialise");
    }

private:
    UART uart;
    std::string received;

    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher;

    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscriber;

    rclcpp::TimerBase::SharedPtr timer;

    void sendCommand(
        std_msgs::msg::String::SharedPtr msg
    ) {
        if (msg->data.find_first_of("\r\n") != std::string::npos) {
            return;
        }

        bool success = uart.sendData(msg->data + "\n");

        if (success) {
            RCLCPP_INFO(
                get_logger(),
                "Commande envoyee : %s",
                msg->data.c_str()
            );
        } else {
            RCLCPP_ERROR(get_logger(), "Erreur UART");
        }
    }

    void receiveData() {
        received += uart.readData();

        size_t position;

        while ((position = received.find('\n')) != std::string::npos) {
            std_msgs::msg::String msg;

            msg.data = received.substr(0, position);

            received.erase(0, position + 1);

            publisher->publish(msg);

            RCLCPP_INFO(
                get_logger(),
                "STM32 : %s",
                msg.data.c_str()
            );
        }

        if (received.size() > 1024) {
            received.clear();
        }
    }
};

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);

    try {
        auto node = std::make_shared<UARTNode>();
        rclcpp::spin(node);
    } catch (const std::exception& error) {
        rclcpp::shutdown();
        return 1;
    }

    rclcpp::shutdown();

    return 0;
}
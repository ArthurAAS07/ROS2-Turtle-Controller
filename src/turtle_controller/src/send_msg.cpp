#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

using namespace std::chrono_literals;

class SendMsg : public rclcpp::Node {
    public:
        SendMsg() : Node("send_msg") {
            publisher_ = this->create_publisher<std_msgs::msg::String>("/comandos", 10);
        }
        void send_command(const std::string & command) {
            auto msg = std_msgs::msg::String();

            msg.data = command;

            publisher_->publish(msg);

            RCLCPP_INFO(this->get_logger(), "Comando enviado: %s", msg.data.c_str());
        }
    private:

        rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
        std::vector<std::string> commands_ = {"up", "right", "down", "left"};

};


int main(int argc, char * argv[]) {

    rclcpp::init(argc, argv);

    auto node = std::make_shared<SendMsg>();

    std::string command;

    while (rclcpp::ok()) {

        std::cout << "Digite um comando: ";
        std::cin >> command;

        if (command == "up" || command == "down" || command == "left" || command == "right") {
            node->send_command(command);
        } else {
            std::cout << "Comando inválido. Use: " << "up, down, left ou right." << std::endl;
        }

        rclcpp::spin_some(node);
    }

    rclcpp::shutdown();

    return 0;
}
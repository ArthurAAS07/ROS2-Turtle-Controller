#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"

using namespace std::chrono_literals;


class TurtleController : public rclcpp::Node {

    public:
        TurtleController() : Node("turtle_controller") {
            publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);

            timer_ = this->create_wall_timer(500ms, std::bind(&TurtleController::send_velocity, this));
        }
    private:
        void send_velocity(){
            auto msg = geometry_msgs::msg::Twist();

            msg.linear.x = 2.0;
            msg.angular.z = 1.0;

            publisher_->publish(msg);

            RCLCPP_INFO(this->get_logger(), "Movendo tartaruga");
        }
        rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
        rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char * argv[]) {
    // Inicializa o ROS2
    rclcpp::init(argc, argv);

    //Cria o nó do controlador da tartaruga e mantém ele em execução com o rclcpp::spin
    rclcpp::spin(std::make_shared<TurtleController>());

    // Encerra o ROS2
    rclcpp::shutdown();

    return 0;
}

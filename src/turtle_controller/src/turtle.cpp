#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "std_msgs/msg/string.hpp"
#include "turtlesim/msg/pose.hpp"
#include "turtlesim/srv/teleport_absolute.hpp"

using namespace std::chrono_literals;

class Turtle : public rclcpp::Node {

    public:
        Turtle() : Node("turtle") {
            publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);
            cmd_subscription_ = this->create_subscription<std_msgs::msg::String>("/comandos", 10, std::bind(&Turtle::command_callback, this, std::placeholders::_1));
            pose_subscription_ = this->create_subscription<turtlesim::msg::Pose>("/turtle1/pose", 10, std::bind(&Turtle::pose_callback, this, std::placeholders::_1));
            teleport_client_ = this->create_client<turtlesim::srv::TeleportAbsolute>("/turtle1/teleport_absolute");
            timer_ = this->create_wall_timer(125ms, std::bind(&Turtle::send_velocity, this));
        }
    private:
    void command_callback(const std_msgs::msg::String::SharedPtr msg) {
        if (lock_) {
            if (msg->data == "right") {
                target_x_ = x_ + 1.0;
                target_y_ = y_ ;
                target_theta_ = 0.0;
            } else if (msg->data == "left") {
                target_x_ = x_ - 1.0;
                target_y_ = y_;
                target_theta_ = M_PI;
            } else if (msg->data == "up") {
                target_x_ = x_;
                target_y_ = y_ + 1.0;
                target_theta_ = M_PI / 2.0;
            } else if (msg->data == "down") {
                target_x_ = x_;
                target_y_ = y_ - 1.0;
                target_theta_ = -M_PI / 2.0;
            }
            rotating_ = true;
            moving_ = false;
            lock_ = false;
        }
        RCLCPP_INFO(this->get_logger(), "Comando recebido: %s", msg->data.c_str());
        }

        void pose_callback(const turtlesim::msg::Pose::SharedPtr msg) {
            x_ = msg->x;
            y_ = msg->y;
            theta_ = msg->theta;

            RCLCPP_INFO(this->get_logger(), "Pose: x=%.2f y=%.2f theta=%.2f", x_, y_, theta_);
        }

        void send_velocity() {
            auto msg = geometry_msgs::msg::Twist();

            if (rotating_) {

                if (std::abs(theta_ - target_theta_) < 0.2) {
                    rotating_ = false;
                    moving_ = true;
                    msg.angular.z = 0.0;

                    auto request = std::make_shared<turtlesim::srv::TeleportAbsolute::Request>();
                    request->x = x_;
                    request->y = y_;
                    request->theta = target_theta_;
                    teleport_client_->async_send_request(request);

                    RCLCPP_INFO(this->get_logger(), "Rotação concluída: theta=%.2f", theta_);
                } else {
                    moving_ = false;
                    msg.angular.z = (target_theta_ - theta_)/std::abs(target_theta_ - theta_) * angular_velocity_;
                    if (std::abs(target_theta_ - theta_) > M_PI) {
                        msg.angular.z = -msg.angular.z;
                    }
                }
                publisher_->publish(msg);

            } else if (moving_) {

                if (std::hypot(target_x_ - x_, target_y_ - y_) < 0.2) {
                    moving_ = false;
                    msg.linear.x = 0.0;
                    msg.angular.z = 0.0;
                    
                    auto request = std::make_shared<turtlesim::srv::TeleportAbsolute::Request>();
                    request->x = target_x_;
                    request->y = target_y_;
                    request->theta = target_theta_;
                    teleport_client_->async_send_request(request);

                    lock_ = true;

                    RCLCPP_INFO(this->get_logger(), "Destino alcançado: x=%.2f y=%.2f", x_, y_);
                } else {
                    msg.linear.x = linear_velocity_;
                }    
                publisher_->publish(msg);
                
            }
        }
        
        rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
        rclcpp::Subscription<std_msgs::msg::String>::SharedPtr cmd_subscription_;
        rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr pose_subscription_;
        rclcpp::Client<turtlesim::srv::TeleportAbsolute>::SharedPtr teleport_client_;
        rclcpp::TimerBase::SharedPtr timer_;
        
        double linear_velocity_ = 1.0;
        double angular_velocity_ = 1.0;
        double x_ = 0.0;
        double y_ = 0.0;
        double theta_ = 0.0;
        double target_x_ = 0.0;
        double target_y_ = 0.0;
        double target_theta_ = 0.0;
        bool moving_ = false;
        bool rotating_ = false;
        bool lock_ = true;
};

int main(int argc, char * argv[]) {
    // Inicializa o ROS2
    rclcpp::init(argc, argv);

    //Cria o nó do controlador da tartaruga e mantém ele em execução com o rclcpp::spin
    rclcpp::spin(std::make_shared<Turtle>());

    // Encerra o ROS2
    rclcpp::shutdown();

    return 0;
}

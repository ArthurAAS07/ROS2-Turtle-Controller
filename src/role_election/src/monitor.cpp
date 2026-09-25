#include "rclcpp/rclcpp.hpp"
#include "role_election/msg/role.hpp"

#include <map>
#include <set>
#include <string>

class Monitor : public rclcpp::Node {
public:
    // Construtor da classe Monitor
    Monitor() : Node("monitor") {
        subscription_ = this->create_subscription<role_election::msg::Role>("/papeis", 10, std::bind(&Monitor::role_callback, this, std::placeholders::_1));
    }

private:
    void role_callback(const role_election::msg::Role::SharedPtr msg) {
        // Armazena o papel atribuído a cada robô em um mapa
        roles_[msg->robot_id] = msg->role;
        updated_robots_.insert(msg->robot_id);

        // Espera receber os papéis dos três robôs
        if (updated_robots_.size() == 3) {
            RCLCPP_INFO(this->get_logger(), "Robô 1: %s | Robô 2: %s | Robô 3: %s", roles_[1].c_str(), roles_[2].c_str(), roles_[3].c_str());
            updated_robots_.clear();
        }
    }

    rclcpp::Subscription<role_election::msg::Role>::SharedPtr subscription_;

    std::map<int, std::string> roles_;
    std::set<int> updated_robots_;
};

int main(int argc, char * argv[]) {
    // Inicializa o ROS 2
    rclcpp::init(argc, argv);

    // Cria um nó Monitor e inicia o loop de execução
    rclcpp::spin(std::make_shared<Monitor>());

    // Encerra o ROS 2
    rclcpp::shutdown();

    return 0;
}
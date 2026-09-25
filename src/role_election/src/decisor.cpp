#include "rclcpp/rclcpp.hpp"
#include "role_election/msg/detection.hpp"
#include "role_election/msg/role.hpp"

#include <algorithm>
#include <map>
#include <vector>

using namespace std::chrono_literals;

class Decisor : public rclcpp::Node {
public:
    // Construtor da classe Decisor
    Decisor() : Node("decisor") {
        subscription_ = this->create_subscription<role_election::msg::Detection>("/deteccoes", 10, std::bind(&Decisor::detection_callback, this, std::placeholders::_1));
        publisher_ = this->create_publisher<role_election::msg::Role>("/papeis", 10);
        timer_ = this->create_wall_timer(1s, std::bind(&Decisor::decide_roles, this));
    }

private:
    void detection_callback(const role_election::msg::Detection::SharedPtr msg) {
        distances_[msg->robot_id] = msg->distance; // Guarda a última distância recebida de cada robô
    }

    void decide_roles() {
        // Só decide depois de receber pelo menos uma detecção dos 3 robôs
        if (distances_.size() < 3) { return; }

        std::vector<std::pair<double, int>> robots;

        // Cria um vetor de pares (distância, robot_id) para facilitar a ordenação
        for (const auto & robot : distances_) {
            robots.push_back({robot.second, robot.first});
        }

        // Ordena primeiro pela distância e, em caso de empate, pelo menor robot_id
        std::sort(robots.begin(), robots.end());

        // Atribui os papéis com base na ordenação
        int attacker_id = robots[0].second;
        int remaining_1 = robots[1].second;
        int remaining_2 = robots[2].second;

        // Os robôs restantes são atribuídos como goleiro e apoio com base no menor ID
        int goalkeeper_id = std::min(remaining_1, remaining_2);
        int support_id = std::max(remaining_1, remaining_2);

        // Publica os papéis atribuídos 
        publish_role(attacker_id, "atacante");
        publish_role(support_id, "apoio");
        publish_role(goalkeeper_id, "goleiro");
    }

    void publish_role(int robot_id, const std::string & role) {
        // Cria uma mensagem do tipo Role
        role_election::msg::Role msg;

        msg.robot_id = robot_id;
        msg.role = role;

        // Publica a mensagem no tópico "/papeis"
        publisher_->publish(msg);
    }

    rclcpp::Subscription<role_election::msg::Detection>::SharedPtr subscription_;
    rclcpp::Publisher<role_election::msg::Role>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;

    std::map<int, double> distances_;
};

int main(int argc, char * argv[]) {
    // Inicializa o ROS 2
    rclcpp::init(argc, argv);

    // Cria um nó Decisor e inicia o loop de execução
    rclcpp::spin(std::make_shared<Decisor>());

    // Encerra o ROS 2
    rclcpp::shutdown();
    
    return 0;
}
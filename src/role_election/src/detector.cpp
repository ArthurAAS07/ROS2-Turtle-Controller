#include "rclcpp/rclcpp.hpp"
#include "role_election/msg/detection.hpp"

#include <random>
#include <unistd.h>

class Detector : public rclcpp::Node {
public:
    // Construtor da classe Detector
    Detector() : Node("detector") {

        // Declara o parâmetro "robot_id" com valor padrão 1
        this->declare_parameter<int>("robot_id", 1);

        // Obtém o valor do parâmetro "robot_id" e armazena na variável robot_id_
        robot_id_ = this->get_parameter("robot_id").as_int();

        // Publica todas as detecções no mesmo tópico
        publisher_ = this->create_publisher<role_election::msg::Detection>("/deteccoes", 10);

        // Publica uma detecção a cada 1 segundo
        timer_ = this->create_wall_timer(std::chrono::seconds(1), std::bind(&Detector::publish_detection, this));

        // Loga uma mensagem informando que o detector foi iniciado
        RCLCPP_INFO(this->get_logger(), "Detector iniciado - Robô %d", robot_id_);
    }

private:
    void publish_detection() {
        // Cria uma mensagem do tipo Detection
        role_election::msg::Detection msg;

        // Gera uma distância aleatória entre 0.5 e 10.0 metros
        msg.robot_id = robot_id_;
        msg.distance = distance_distribution_(generator_);

        // Loga a mensagem informando o ID do robô e a distância até a bola
        RCLCPP_INFO(this->get_logger(), "Robô %d | Distância até a bola: %.2f", msg.robot_id, msg.distance);
        
        // Incrementa o contador de publicações e aguarda 1 segundo a cada 3 publicações
        publisher_count_++;
        if (publisher_count_ == 3) {
            sleep(1);  // Aguarda 1 segundo antes de publicar novamente
            publisher_count_ = 0;
        }

        // Publica a mensagem no tópico "/deteccoes"
        publisher_->publish(msg);
    }

    int robot_id_;
    int publisher_count_ = 0;

    rclcpp::Publisher<role_election::msg::Detection>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;

    std::random_device random_device_;
    std::mt19937 generator_{random_device_()};
    std::uniform_real_distribution<double> distance_distribution_{0.5, 10.0};
};

int main(int argc, char * argv[]) {
    // Inicializa o ROS 2
    rclcpp::init(argc, argv);

    // Cria um nó Detector e inicia o loop de execução
    rclcpp::spin(std::make_shared<Detector>());

    // Encerra o ROS 2
    rclcpp::shutdown();

    return 0;
}
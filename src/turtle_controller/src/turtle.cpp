#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "std_msgs/msg/string.hpp"
#include "turtlesim/msg/pose.hpp"
#include "turtlesim/srv/teleport_absolute.hpp"
#include <queue>
#include <string>

#define FREQUENCY 20        // Frequência de envio da send_velocity()
#define SOBRA 0.1           // Tolerância para considerar que a tartaruga chegou ao destino
#define LINEAR_VEL 1.0      // Velocidade linear da tartaruga
#define ANGULAR_VEL 1.0     // Velocidade angular da tartaruga

using namespace std::chrono_literals;

class Turtle : public rclcpp::Node {

    public:
        // Construtor da classe Turtle, que inicializa o nó e cria os publishers, subscribers e clientes necessários
        Turtle() : Node("turtle") {
            publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);
            cmd_subscription_ = this->create_subscription<std_msgs::msg::String>("/comandos", 10, std::bind(&Turtle::command_callback, this, std::placeholders::_1));
            pose_subscription_ = this->create_subscription<turtlesim::msg::Pose>("/turtle1/pose", 10, std::bind(&Turtle::pose_callback, this, std::placeholders::_1));
            teleport_client_ = this->create_client<turtlesim::srv::TeleportAbsolute>("/turtle1/teleport_absolute");
            timer_ = this->create_wall_timer(FREQUENCY * 1ms, std::bind(&Turtle::send_velocity, this));
        }
    private:
        // Callback para receber comandos do sendo_msg.cpp através do topic /comandos
        void command_callback(const std_msgs::msg::String::SharedPtr msg) {
            
            // Adiciona o comando recebido à fila de comandos e chama a função next_command() para processar o próximo comando
            command_queue_.push(msg->data); 
            next_command();
            
            // Printa no console o comando recebido
            // RCLCPP_INFO(this->get_logger(), "Comando recebido: %s", msg->data.c_str());
        }
        
        // Função que processa o próximo comando na fila de comandos
        void next_command() {
            
            // Se a fila de comandos estiver vazia ou se o robô estiver bloqueado, retorna sem fazer nada
            if (command_queue_.empty()) { return; }
            if (lock_) { return; }
            
            // Pega o próximo comando da fila e remove ele da fila
            command = command_queue_.front();
            command_queue_.pop();

            // Define as coordenadas alvo e o ângulo alvo com base no comando recebido
            if (command == "right") {
                target_x_ = x_ + 1.0;
                target_y_ = y_ ;
                target_theta_ = 0.0;
            } else if (command == "left") {
                target_x_ = x_ - 1.0;
                target_y_ = y_;
                target_theta_ = M_PI;
            } else if (command == "up") {
                target_x_ = x_;
                target_y_ = y_ + 1.0;
                target_theta_ = M_PI / 2.0;
            } else if (command == "down") {
                target_x_ = x_;
                target_y_ = y_ - 1.0;
                target_theta_ = -M_PI / 2.0;
            }

            // Verifica se o destino está dentro da tela do turtlesim (0.5, 0.5) a (11.5, 11.5)
            if (target_x_ < 0.5 || target_x_ > 11.5 || target_y_ < 0.5 || target_y_ > 11.5) {
                RCLCPP_INFO(this->get_logger(),"Comando ignorado: destino fora da tela");
                RCLCPP_INFO(this->get_logger(), "Coordenadas: (%.0f, %.0f)", x_-5.54444, y_-5.54444);
                next_command();
                return;
            }

            // Define o estado do robô como rotacionando e não movendo para atualizar send_velocity()
            rotating_ = true;
            moving_ = false;
        }

        // Callback para receber a posição atual da tartaruga através do topic /turtle1/pose do turtlesim
        void pose_callback(const turtlesim::msg::Pose::SharedPtr msg) {
            x_ = msg->x;
            y_ = msg->y;
            theta_ = msg->theta;

            // Printa no console as coordenadas iniciais da tartaruga na tela do turtlesim
            if (init) {
                RCLCPP_INFO(this->get_logger(), "Coordenadas: (0, 0)");
                init = false;
            }
            // RCLCPP_INFO(this->get_logger(), "Coordenadas: (%.4f, %.4f)", x_-5.54444, y_-5.54444);
        }
        
        // Função que envia a velocidade linear e angular da tartaruga para o turtlesim através do topic /turtle1/cmd_vel
        void send_velocity() {

            // mensagem do tipo Twist
            auto msg = geometry_msgs::msg::Twist();
            
            // Se o robô estiver rotacionando, calcula a velocidade angular necessária para atingir o ângulo alvo
            if (rotating_) {

                // RCLCPP_INFO(this->get_logger(), "ESTADO: ROTACIONANDO");
                
                lock_ = true; // Bloqueia o robô para não receber novos comandos enquanto estiver rotacionando

                // Se a diferença entre o ângulo atual e o ângulo alvo for menor que SOBRA, considera que a rotação está completa
                if (std::abs(theta_ - target_theta_) < SOBRA) {

                    // RCLCPP_INFO(this->get_logger(), "ROTACAO OK -> iniciando movimento para (%.2f, %.2f)", target_x_, target_y_);
                    // Atualiza o estado do robô para movendo e não rotacionando, e publica a velocidade angular como 0
                    rotating_ = false;
                    moving_ = true;
                    msg.angular.z = 0.0; 
                    publisher_->publish(msg); // Publica antes de teleportar
                    
                    // Teleporta a tartaruga para a posição alvo usando o serviço /turtle1/teleport_absolute do turtlesim
                    auto request = std::make_shared<turtlesim::srv::TeleportAbsolute::Request>();
                    request->x = x_;
                    request->y = y_;
                    request->theta = target_theta_;
                    teleport_client_->async_send_request(request);

                } else {
                    // Se a rotação ainda não estiver completa, calcula a velocidade angular necessária para atingir o ângulo alvo
                    moving_ = false;
                    msg.angular.z = (target_theta_ - theta_)/std::abs(target_theta_ - theta_) * ANGULAR_VEL;
                    if (std::abs(target_theta_ - theta_) > M_PI) {
                        msg.angular.z = -msg.angular.z;
                    }
                }

                // Publica a velocidade angular mesmo que já tenha sido teleportado, para garantir e publicar no caso else
                publisher_->publish(msg);
                
            } else if (moving_) {

                // RCLCPP_INFO(this->get_logger(), "ESTADO: MOVENDO | atual=(%.2f, %.2f) alvo=(%.2f, %.2f)", x_, y_, target_x_, target_y_);
                // Se a distância entre a posição atual e a posição alvo for menor que SOBRA, considera que o movimento está completo
                if (std::hypot(target_x_ - x_, target_y_ - y_) < SOBRA) {

                    // RCLCPP_INFO(this->get_logger(), "MOVIMENTO OK!");
                    // Atualiza o estado do robô para não movendo e publica a velocidade linear como 0
                    moving_ = false;
                    msg.linear.x = 0.0;
                    msg.angular.z = 0.0;
                    publisher_->publish(msg); // Publica antes de teleportar
                    
                    // Teleporta a tartaruga para a posição alvo usando o serviço /turtle1/teleport_absolute do turtlesim
                    auto request = std::make_shared<turtlesim::srv::TeleportAbsolute::Request>();
                    request->x = target_x_;
                    request->y = target_y_;
                    request->theta = target_theta_;

                    // Define um callback para o serviço de teleporte que atualiza só permite o próximo comando quando o teleporte for concluído
                    teleport_client_->async_send_request(request, [this](rclcpp::Client<turtlesim::srv::TeleportAbsolute>::SharedFuture future) {
                        future.get();
                        
                        // Atualiza as coordenadas atuais da tartaruga para as coordenadas alvo, sem esperar pose_callback
                        x_ = target_x_;
                        y_ = target_y_;
                        theta_ = target_theta_;
                        
                        // Printa no console as coordenadas atuais da tartaruga na tela do turtlesim
                        if (command_queue_.empty()) {
                            RCLCPP_INFO(this->get_logger(), "Coordenadas: (%.0f, %.0f)", x_-5.54444, y_-5.54444);
                        }
                        lock_ = false; // Libera o robô para receber novos comandos
                        next_command();
                    });  
                } else {
                    msg.linear.x = LINEAR_VEL; // Se a distância ainda não estiver completa, define a velocidade linear
                }    
                publisher_->publish(msg); // Publica para garantir e para o caso else    
            }
        }
        
        rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;           // Publisher para enviar velocidade ao turtlesim
        rclcpp::Subscription<std_msgs::msg::String>::SharedPtr cmd_subscription_;     // Subscription para receber comandos do send_msg.cpp
        rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr pose_subscription_;     // Subscription para receber a pose da tartaruga
        rclcpp::Client<turtlesim::srv::TeleportAbsolute>::SharedPtr teleport_client_; // Client para o serviço de teleporte
        rclcpp::TimerBase::SharedPtr timer_;                                          // Timer para controlar a frequência das atualizações

        std::queue<std::string> command_queue_; // Fila de comandos recebidos do send_msg.cpp
        std::string command;                    // Comando atual que está sendo processado

        double x_ = 0.0;             // Coordenada x atual da tartaruga
        double y_ = 0.0;             // Coordenada y atual da tartaruga
        double theta_ = 0.0;         // Ângulo atual da tartaruga
        double target_x_ = 0.0;      // Coordenada x alvo da tartaruga
        double target_y_ = 0.0;      // Coordenada y alvo da tartaruga
        double target_theta_ = 0.0;  // Ângulo alvo da tartaruga
        bool moving_ = false;        // Flag para indicar se a tartaruga está se movendo
        bool rotating_ = false;      // Flag para indicar se a tartaruga está rotacionando
        bool lock_ = false;          // Flag para indicar se a tartaruga está bloqueada para receber novos comandos
        bool init = true;            // Flag para indicar se é a primeira vez que a pose da tartaruga é recebida
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

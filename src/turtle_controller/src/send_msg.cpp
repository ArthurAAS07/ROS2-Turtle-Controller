#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

using namespace std::chrono_literals;

class SendMsg : public rclcpp::Node {
    public:
        // Construtor da classe SendMsg, que inicializa o nó e cria o publisher para enviar comandos
        SendMsg() : Node("send_msg") {
            publisher_ = this->create_publisher<std_msgs::msg::String>("/comandos", 10);
        }
        // Função que envia um comando para o tópico /comandos
        void send_command(const std::string & command) {
            
            // Cria uma mensagem do tipo std_msgs::msg::String e publica o comando no tópico /comandos
            auto msg = std_msgs::msg::String();
            msg.data = command;
            publisher_->publish(msg);

            RCLCPP_INFO(this->get_logger(), "Comando enviado: %s", msg.data.c_str());
        }
    private:
        // Publisher para enviar comandos para o tópico /comandos
        rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
};

int main(int argc, char * argv[]) {

    // Inicializa o ROS2
    rclcpp::init(argc, argv);

    auto node = std::make_shared<SendMsg>(); // Instancia o nó SendMsg
    std::string command; // Variável para o comando digitado

    // Loop que lê o comando digitado e envia para o tópico
    while (rclcpp::ok()) {

        // std::cout << "Digite um comando: ";
        std::cin >> command; // Entrada do comando 

        // Verifica se o comando é válido e envia para o tópico /comandos
        if (command == "up" || command == "down" || command == "left" || command == "right") {
            node->send_command(command);
        } else {
            std::cout << "Comando inválido. Use: " << "up, down, left ou right." << std::endl;
        }

        // Mantém o nó ativo para processar callbacks, mesmo que não haja mensagens recebidas
        rclcpp::spin_some(node);
    }

    // Encerra o ROS2
    rclcpp::shutdown();

    return 0;
}
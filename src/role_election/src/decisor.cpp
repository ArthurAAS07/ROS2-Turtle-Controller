#include "rclcpp/rclcpp.hpp"
#include "role_election/msg/detection.hpp"
#include "role_election/msg/role.hpp"

#include <algorithm>
#include <map>
#include <vector>

using namespace std::chrono_literals;

class Decisor : public rclcpp::Node {
public:
    Decisor() : Node("decisor") {
        subscription_ = this->create_subscription<role_election::msg::Detection>("/deteccoes", 10, std::bind(&Decisor::detection_callback, this, std::placeholders::_1));
        publisher_ = this->create_publisher<role_election::msg::Role>("/papeis", 10);
        timer_ = this->create_wall_timer(1s, std::bind(&Decisor::decide_roles, this));
    }

private:
    void detection_callback(const role_election::msg::Detection::SharedPtr msg) {
        // Guarda a última distância recebida de cada robô
        distances_[msg->robot_id] = msg->distance;
    }

    void decide_roles() {
        // Só decide depois de receber pelo menos uma detecção dos 3 robôs
        if (distances_.size() < 3) { return; }

        std::vector<std::pair<double, int>> robots;

        for (const auto & robot : distances_) {
            robots.push_back({robot.second, robot.first});
        }

        // Ordena primeiro pela distância e, em caso de empate, pelo menor robot_id
        std::sort(robots.begin(), robots.end());

        int attacker_id = robots[0].second;
        int remaining_1 = robots[1].second;
        int remaining_2 = robots[2].second;

        int goalkeeper_id = std::min(remaining_1, remaining_2);
        int support_id = std::max(remaining_1, remaining_2);

        publish_role(attacker_id, "atacante");
        publish_role(support_id, "apoio");
        publish_role(goalkeeper_id, "goleiro");
    }

    void publish_role(int robot_id, const std::string & role) {
        role_election::msg::Role msg;

        msg.robot_id = robot_id;
        msg.role = role;

        publisher_->publish(msg);
    }

    rclcpp::Subscription<role_election::msg::Detection>::SharedPtr subscription_;
    rclcpp::Publisher<role_election::msg::Role>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;

    std::map<int, double> distances_;
};

int main(int argc, char * argv[]) {
    rclcpp::init(argc, argv);

    rclcpp::spin(std::make_shared<Decisor>());

    rclcpp::shutdown();

    return 0;
}
# ROS 2 Turtle Controller

Projeto desenvolvido utilizando ROS 2 Humble e turtlesim para estudo de comunicação entre nós através de tópicos.

## Objetivo

Implementar um sistema de controle da tartaruga no turtlesim utilizando múltiplos nós ROS 2 que se comunicam através de topics.

## Conceitos estudados

- ROS 2 Nodes
- Publishers
- Subscribers
- Topics
- Messages
- C++
- rclcpp
- CMake

## Arquitetura inicial
Controller Node
  |
  | publish
  ↓
/cmd_vel
  |
  ↓
Turtle Controller Node
  |
  | publish Twist
  ↓
/turtle1/cmd_vel
  |
  ↓
turtlesim


## Ambiente

- Ubuntu 22.04
- ROS 2 Humble
- C++
- Python

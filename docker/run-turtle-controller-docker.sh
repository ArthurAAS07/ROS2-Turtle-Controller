#!/bin/bash

# Diretório raiz do projeto
ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"

cd "$ROOT_DIR"

# Verifica se o Docker está instalado
if ! command -v docker &> /dev/null; then
    echo "Erro: Docker não está instalado."
    exit 1
fi

# Verifica se o Docker está funcionando
if ! docker info &> /dev/null; then
    echo "Erro: Docker não está em execução."
    exit 1
fi

echo "Construindo imagem..."

docker build \
    -t turtle-controller \
    -f docker/Dockerfile \
    .

# Permite acesso à interface gráfica
xhost +local:docker > /dev/null

# Remove a permissão quando o script terminar
trap 'xhost -local:docker > /dev/null' EXIT

echo "Iniciando Turtle Controller..."

docker run --rm -it \
    --net=host \
    -e DISPLAY="$DISPLAY" \
    -v /tmp/.X11-unix:/tmp/.X11-unix \
    turtle-controller \
    bash -c '
        source /opt/ros/jazzy/setup.bash
        source /ros2_ws/install/setup.bash

        ros2 run turtlesim turtlesim_node &
        TURTLESIM_PID=$!

        ros2 run turtle_controller turtle &
        TURTLE_PID=$!

        trap "kill $TURTLESIM_PID $TURTLE_PID 2>/dev/null" EXIT

        ros2 run turtle_controller send_msg

        wait
    '

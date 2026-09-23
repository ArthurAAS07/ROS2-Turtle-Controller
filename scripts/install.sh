#!/bin/bash

set -e

# Diretório raiz do workspace
ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"

cd "$ROOT_DIR"

echo "======================================"
echo "   Configuração do Turtle Controller"
echo "======================================"

# Verifica se o ROS 2 está instalado
if [ -z "$ROS_DISTRO" ]; then
    echo "ERRO: ROS 2 não foi encontrado."
    echo "Instale o ROS 2 antes de continuar."
    exit 1
fi

echo "ROS 2 encontrado: $ROS_DISTRO"

# Carrega o ambiente do ROS 2
source /opt/ros/$ROS_DISTRO/setup.bash

# Verifica o colcon
if ! command -v colcon &> /dev/null; then
    echo "colcon não encontrado."
    echo "Instalando..."

    sudo apt update
    sudo apt install -y python3-colcon-common-extensions
fi

# Verifica o rosdep
if ! command -v rosdep &> /dev/null; then
    echo "rosdep não encontrado."
    echo "Instalando..."

    sudo apt update
    sudo apt install -y python3-rosdep
fi

# Atualiza o rosdep
echo "Atualizando rosdep..."
rosdep update

# Instala as dependências do projeto
echo "Instalando dependências..."

rosdep install \
    --from-paths src \
    --ignore-src \
    -r \
    -y

# Compila o workspace
echo "Compilando o projeto..."

colcon build

# Dá permissão de execução aos scripts
chmod +x scripts/run.sh
chmod +x scripts/install.sh

echo ""
echo "======================================"
echo " Projeto configurado com sucesso!"
echo "======================================"
echo ""
echo "Para executar:"
echo "./scripts/run.sh"
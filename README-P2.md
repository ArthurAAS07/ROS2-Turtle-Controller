# ROS2 Role Election

Este pacote implementa o problema extra do projeto utilizando ROS 2.

O objetivo é simular três robôs detectando a distância até uma bola e, a partir dessas informações, realizar uma eleição dinâmica dos papéis de cada robô:

- **Atacante**
- **Apoio**
- **Goleiro**

Os três robôs executam simultaneamente e enviam suas detecções para um nó responsável por decidir os papéis.

A arquitetura do sistema é:

```text
detector_1 ─┐
detector_2 ─┼──> /deteccoes ──> decisor ──> /papeis ──> monitor
detector_3 ─┘
```

## Funcionamento

O sistema possui três tipos de nós:

- `detector`
- `decisor`
- `monitor`

O executável `detector` é iniciado três vezes, uma para cada robô.

Cada instância recebe um parâmetro `robot_id`, permitindo identificar qual robô aquela instância representa:

```text
detector_1 → robot_id = 1
detector_2 → robot_id = 2
detector_3 → robot_id = 3
```

Cada detector gera periodicamente uma distância simulada até a bola e publica essa informação no tópico ```/deteccoes```

A mensagem enviada possui o identificador do robô e a distância estimada até a bola.

O nó `decisor` é inscrito no tópico `/deteccoes` e mantém armazenada a última distância recebida de cada robô.

Quando já existe uma detecção dos três robôs, é realizada a eleição dos papéis.

A regra utilizada é:

1. O robô com a menor distância até a bola é escolhido como **atacante**.
2. Em caso de empate na menor distância, o robô com menor `robot_id` vence o desempate.
3. Entre os dois robôs restantes:
   - o menor `robot_id` é definido como **goleiro**;
   - o maior `robot_id` é definido como **apoio**.

Os papéis escolhidos são publicados no tópico ```/papeis```

É enviada uma mensagem para cada robô contendo seu identificador e o papel escolhido.

O nó `monitor` recebe essas mensagens e apresenta o resultado de forma legível no terminal.

## Compilar e Executar (Manualmente)

Escolher um diretório e clonar o repositório  
> `git clone https://github.com/ArthurAAS07/ROS2-Turtle-Controller.git`

Entrar na pasta do projeto  
> `cd ROS2-Turtle-Controller`

Dar permissão e instalar as depências  
> `chmod +x scripts/install.sh`  
> `./scripts/install.sh`

Rodar o pacote  
> `./scripts/run-role-election.sh`

## Compilar e Executar (Docker)
É necessário ter:

- Docker instalado e em execução;
- Ubuntu/Linux com suporte à interface gráfica do TurtleSim.

No diretório raiz do projeto, execute:

> `./docker/run-turtle-controller-docker.sh`
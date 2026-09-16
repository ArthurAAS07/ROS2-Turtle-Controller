# ROS2 Turtle Controller

## 1. Introdução

Projeto desenvolvido utilizando ROS 2 Humble com objetivo de criar um sistema distribuído de controle de movimentação utilizando comunicação entre nós através de tópicos ROS.

O projeto consiste em desenvolver nós independentes responsáveis por gerar comandos e controlar a movimentação da tartaruga no ambiente de simulação Turtlesim.

## 2. Descrição do Projeto
### 2.1 Problema

O problema consiste em desenvolver um controlador capaz de interagir com um robô simulado através da infraestrutura ROS2, permitindo o envio e processamento de comandos de movimentação.

### 2.2 Solução Proposta

A solução proposta utiliza uma arquitetura baseada em múltiplos nós ROS2:

- Um nó responsável pela geração dos comandos de movimentação;
- Um nó controlador responsável pelo processamento das informações;
- Comunicação entre componentes através de tópicos ROS2.

A partir dessa estrutura, o sistema consegue enviar comandos de velocidade linear e angular para controlar a trajetória da tartaruga no simulador.

### 2.3 Arquitetura do Sistema
+----------------+
| Command Node   |
+----------------+
        |
        |
        v
 /control_command
        |
        v
+----------------+
| Controller Node|
+----------------+
        |
        |
        v
/turtle1/cmd_vel
        |
        v
+----------------+
|   Turtlesim    |
+----------------+

## 3. Metodologia de desenvolvimento

A organização do projeto foi baseada em princípios de desenvolvimento incremental e arquitetura top-down.

A estratégia adotada consiste em dividir o desenvolvimento em pequenas entregas funcionais (checkpoints), permitindo validar cada etapa antes de evoluir para a próxima.

Além disso, foi utilizada uma abordagem de versionamento contínuo utilizando Git, onde cada avanço significativo representa uma nova versão do projeto.

## 4. Objetivos e Metas

- Configurar ambiente ROS 2 Humble para desenvolvimento.
- Compreender conceitos fundamentais do ROS 2.
- Criar os nós utilizando C++.
- Implementar comunicação entre nós utilizando tópicos.
- Desenvolver controlador de movimentação da tartaruga.
- Documentar arquitetura e processo de desenvolvimento.

## 5. Planejamento por Checkpoints

| Checkpoint | Objetivo                       | Entregável                         | Previsão |
| ---------- | ------------------------------ | ---------------------------------- | -------- |
| CP1        | Configuração do ambiente       | ROS2 instalado e funcionando       | 16/09    |
| CP2        | Estudo dos conceitos ROS2      | Testes com turtlesim e tópicos     | 16/09    |
| CP3        | Estruturação do projeto        | Repositório e documentação inicial | 16/09    |
| CP4        | Desenvolvimento do primeiro nó | Node ROS2 funcionando              | 17/09    |
| CP5        | Comunicação entre nós          | Sistema completo funcionando       | 19/09    |
| CP6        | Melhorias e testes             | Código refinado                    | 21/09    |
| CP7        | Documentação final             | README completo + vídeo            | 22/09    |
| CP8        | Revisão e possíveis extensões  | Avaliação do problema 2            | 23/09    |

## Histórico de Desenvolvimento



## 7. Desafios Encontrados



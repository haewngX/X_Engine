# A simple FPS game demo using OpenGL

## Introduction

This project is an independent game dedicated to  game AI. The construction of game engine refers to the book  *Game Programming in C++* by Sanjay Madhav and *learnopengl-cn*. And I implemented input system, FPS actor, enemy actor, simple behavior tree AI and simple reinforcement learning AI, etc. There are many shortcomings in function and performance need to be optimized.

##### Third-party libraries used in this project

- Opengl
- glm
- glfw
- rapidjson
- SDL
- stb image
- ZeroMQ
- Pytorch
- TensorboardX

## Simple AI system

#### Behavior Tree

A basic framework of behavior tree was implemented ,including sequence,selector,decorator,condition evaluator and action node. A simple enemy AI is shown in the figure below. 

![image](https://github.com/haewngX/X_Engine/blob/main/img/BT.png)

![image](https://github.com/haewngX/X_Engine/blob/main/img/Player2.gif)

![image](https://github.com/haewngX/X_Engine/blob/main/img/Player3.gif)

![image](https://github.com/haewngX/X_Engine/blob/main/img/Player4.gif)

![image](https://github.com/haewngX/X_Engine/blob/main/img/Player5.gif)

![image](https://github.com/haewngX/X_Engine/blob/main/img/Player6.gif)

![image](https://github.com/haewngX/X_Engine/blob/main/img/Player7.gif)

#### Reinforcement Learning

##### observation

The observation space of agent includes the distance and angle of the enemy to itself .（if agent doesn't detect the enemy,the value will be a large negative value.

##### Action

The action space of agent includes move forward, turn left,turn right and shoot.

##### Reward

At every moment, if agent see the enemy, it will get a reward value of  +0.02, otherwise -0.02. If the agent shoots at the enemy, it will get a reward value of +0.5. When the agent is shot, it will get a reward value of -0.5. The rewards for winning and losing are +5 and -5.

##### RL algorithm

Using DQN and SAC to train the model on a PC with Nvidia Geforce RTX 2070 GPU, the average reward over episodes is shown in the figure below. The algorithm were implemented with Pytorch. It can converge quickly because of dimensions of observation space and action space are small.

![image](https://github.com/haewngX/X_Engine/blob/main/img/reward.png)

##### Interaction between RL Algorithm and engine 

Using ZeroMQ to establish network communication between engine and algorithm. The engine is the client and the algorithm is the server. Engine will send the request message(observation and reward) to server in each loop. When algorithm server receive the message, it will reply the action output by the neural network.

![image](https://github.com/haewngX/X_Engine/blob/main/img/ZMQ.png)

![image](https://github.com/haewngX/X_Engine/blob/main/img/RLAgent.gif)

## TODO

- Sound effect 
- Skybox
- Particle effect
- Multi-agent RL

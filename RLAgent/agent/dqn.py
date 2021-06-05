import torch
import torch.optim as optim
import torch.nn as nn
import torch.nn.functional as F
import numpy as np
from model import *
from memory import ReplayMemory, PrioritizedReplayMemory



class DQN(object):
    def __init__(self, config, env, doubleDQN=False, duelingDQN=False, NoisyDQN=False, N_stepDQN=False, Prioritized=False):
        self.device = config.device


        self.doubleDQN = doubleDQN
        self.duelingDQN = duelingDQN
        self.NoisyDQN = NoisyDQN
        self.N_stepDQN = N_stepDQN
        self.Prioritized = Prioritized

        self.gamma = config.gamma  # 折扣因子
        self.learning_rate = config.learning_rate  # 学习率
        self.replace_target_iter = config.replace_target_iter  # 目标网络更新频率
        self.replay_size = config.replay_size  # 经验池大小
        self.batch_size = config.batch_size  # 批样本数
        self.priority_alpha = config.priority_alpha
        self.priority_beta_start = config.priority_beta_start
        self.priority_beta_frames = config.priority_beta_frames

        self.epsilon = config.epsilon  # epsilon初始值，以其概率选择最大值的动作
        self.epsilon_final = config.epsilon_final  # epsilon的最小值
        self.epsilon_decay = config.epsilon_decay  # epsilon衰减率

        self.num_states = env.s_dim  # 状态空间维度
        self.num_actions = env.a_dim  # 动作空间维度

        self.learn_start = self.batch_size * 3  # 控制学习的参数

        self.learn_step_counter = 0  # 学习的总步数

        self.N_step = config.N_step  # 多步学习的步数

        self.N_step_buffer = []

        if self.Prioritized:
            self.memory = PrioritizedReplayMemory(self.replay_size, self.priority_alpha, self.priority_beta_start,
                                                  self.priority_beta_frames)  # 初始化经验池
        else:
            self.memory = ReplayMemory(self.replay_size)  # 初始化经验池

        if self.duelingDQN:
            # 初始化评估网络
            self.eval_net = DuelingDQNNet(self.num_states, self.num_actions).to(self.device)
            # 初始化目标网络
            self.target_net = DuelingDQNNet(self.num_states, self.num_actions).to(self.device)
        elif self.NoisyDQN:
            # 初始化评估网络
            self.eval_net = NoisyNet(self.num_states, self.num_actions).to(self.device)
            # 初始化目标网络
            self.target_net = NoisyNet(self.num_states, self.num_actions).to(self.device)
        else:
            self.eval_net = DQNNet(self.num_states, self.num_actions).to(self.device)
            # 初始化目标网络
            self.target_net = DQNNet(self.num_states, self.num_actions).to(self.device)

        # 目标网络和评估网络初始时参数一致
        self.target_net.load_state_dict(self.eval_net.state_dict())

        # 训练的优化器
        self.optimizer = optim.Adam(self.eval_net.parameters(), lr=self.learning_rate)

        # 均方损失函数
        self.loss_func = nn.MSELoss()

    # 储存记忆
    def store_transition(self, state, action, reward, next_state, done):
        if self.N_stepDQN:
            # 把当前经验放入N_step buffer中
            self.N_step_buffer.append((state, action, reward, next_state, done))

            # 如果没有达到设定的步数，return
            if len(self.N_step_buffer) < self.N_step:
                return

            # 计算N步回报
            R = sum([self.N_step_buffer[i][2] * (self.gamma ** i) for i in range(self.N_step)])
            state, action, _, _, _ = self.N_step_buffer.pop(0)

            self.memory.push((state, action, R, next_state, done))
        else:
            self.memory.push((state, action, reward, next_state, done))

    # 选择动作
    def choose_action(self, s):
        with torch.no_grad():
            if np.random.random(1) >= self.epsilon:  # 如果大于等于epsilon，动作为网络中Q值最大的
                X = torch.tensor([s], device=self.device, dtype=torch.float)
                a = self.eval_net(X).max(1)[1].view(1, 1)  # 用eval网络计算q值
                return a.item()
            else:  # 如果小于epsilon，动作随机
                return np.random.randint(0, self.num_actions)

    # 从经验池中选取样本
    def get_batch(self):
        transitions, indices, weights = self.memory.sample(self.batch_size)  # 批样本

        # 解压批样本
        # 例如zipped为[(1, 4), (2, 5), (3, 6)]，zip(*zipped)解压为[(1, 2, 3), (4, 5, 6)]
        batch_state, batch_action, batch_reward, batch_next_state, batch_done = zip(*transitions)

        # 将样本转化为tensor
        batch_state = torch.tensor(batch_state, device=self.device, dtype=torch.float)
        batch_action = torch.tensor(batch_action, device=self.device, dtype=torch.long).squeeze().view(-1, 1)  # view转换为列tensor
        batch_reward = torch.tensor(batch_reward, device=self.device, dtype=torch.float).squeeze().view(-1, 1)
        batch_next_state = torch.tensor(batch_next_state, device=self.device, dtype=torch.float)
        batch_done = torch.tensor(batch_done, device=self.device, dtype=torch.float).squeeze().view(-1, 1)
        # print("状态:", batch_state.shape) 128,4
        # print("动作:", batch_action.shape)
        # print("奖励:", batch_reward.shape)
        # print("done:", batch_done.shape)
        #
        return batch_state, batch_action, batch_reward, batch_next_state, batch_done, indices, weights

    # 学习
    def learn(self):
        # 更新目标网络
        if self.learn_step_counter % self.replace_target_iter == 0:
            self.target_net.load_state_dict(self.eval_net.state_dict())

        # 获取批样本
        batch_state, batch_action, batch_reward, batch_next_state, batch_done, indices, weights = self.get_batch()

        # print("状态:", batch_state)
        # print("动作:", batch_action)
        # print("done:", batch_done)

        # 计算q(s,a;θ)
        if self.NoisyDQN:
            self.eval_net.sample_noise()
        q_s_a = self.eval_net(batch_state).gather(1, batch_action)
        # print("q_s_a:", q_s_a.shape)

        # 计算target yj = rj + (1 - done) * gamma * max(q(s',a;θ'))
        with torch.no_grad():
            if self.NoisyDQN:
                self.target_net.sample_noise()
            if self.doubleDQN:
                next_max_action = self.eval_net(batch_next_state).max(dim=1)[1].view(-1, 1)
                q_target = batch_reward + (1. - batch_done) * self.gamma * self.target_net(batch_next_state).gather(1,
                                                                                                                    next_max_action)
                # print("q_target:", q_target)
                # print("q_target.shape:", q_target.shape)
            else:
                next_q = self.target_net(batch_next_state)
                # print("next_q:", next_q)
                max_next_q_a = next_q.max(1)[0].view(-1, 1)
                # print("max_next_q_a:", max_next_q_a)
                # print("max_next_q_a.shape:", max_next_q_a.shape)
                q_target = batch_reward + (1. - batch_done) * self.gamma * max_next_q_a
                # print("q_target:", q_target)
                # print("q_target.shape:", q_target.shape)

        # 损失函数更新
        if self.Prioritized:
            diff = (q_target - q_s_a)
            self.memory.update_priorities(indices, diff.detach().squeeze().abs().cpu().numpy().tolist())
        loss = self.loss_func(q_target, q_s_a)
        self.optimizer.zero_grad()
        loss.backward()
        self.optimizer.step()

        # 学习的步数加一
        self.learn_step_counter += 1

    # 保存模型
    def save(self):
        if self.duelingDQN:
            torch.save(self.eval_net, 'duelingDQN.pkl')
        elif self.NoisyDQN:
            torch.save(self.eval_net, 'NoisyDQN.pkl')
        elif self.N_stepDQN:
            torch.save(self.eval_net, 'N_stepDQN.pkl')
        elif self.Prioritized:
            torch.save(self.eval_net, 'PriorityReplayDQN.pkl')
        else:
            torch.save(self.eval_net, 'DQN.pkl')

    # 加载模型
    def load(self):
        print("load")
        if self.duelingDQN:
            self.eval_net = torch.load('duelingDQN.pkl')
        elif self.NoisyDQN:
            self.eval_net = torch.load('NoisyDQN.pkl')
        elif self.N_stepDQN:
            self.eval_net = torch.load('N_stepDQN.pkl')
        elif self.Prioritized:
            self.eval_net = torch.load('PriorityReplayDQN.pkl')
        else:
            self.eval_net = torch.load('DQN.pkl')



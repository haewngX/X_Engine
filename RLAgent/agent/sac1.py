import torch
import torch.optim as optim
import torch.nn as nn
import torch.nn.functional as F
import numpy as np
from RL.model import ValueNetwork
from RL.model import SoftQNetwork
from RL.model import PolicyNetwork
from RL.memory import ReplayMemory
from torch.distributions import Normal


class SAC(object):
    def __init__(self, config, env):
        self.device = config.device

        self.gamma = config.gamma  # 折扣因子

        self.tau = config.tau

        # 学习率
        self.value_lr = config.value_lr
        self.soft_q_lr = config.soft_q_lr
        self.policy_lr = config.policy_lr

        self.replace_target_iter = config.replace_target_iter  # 目标网络更新频率
        self.replay_size = config.replay_size  # 经验池大小
        self.batch_size = config.batch_size  # 批样本数

        self.num_states = env.s_dim  # 状态空间维度
        self.num_actions = env.a_dim  # 动作空间维度

        self.learn_start = self.batch_size * 3  # 控制学习的参数

        self.learn_step_counter = 0  # 学习的总步数

        self.memory = ReplayMemory(self.replay_size)  # 初始化经验池

        # 初始化V网络
        self.value_net = ValueNetwork(self.num_states, 256).to(self.device)
        # 初始化V目标网络
        self.target_value_net = ValueNetwork(self.num_states, 256).to(self.device)

        # V目标网络和V网络初始时参数一致
        for target_param, param in zip(self.target_value_net.parameters(), self.value_net.parameters()):
            target_param.data.copy_(param.data)

        # 初始化Q网络
        self.soft_q_net = SoftQNetwork(self.num_states, self.num_actions, 256).to(self.device)

        # 初始化策略网络
        self.policy_net = PolicyNetwork(self.num_states, self.num_actions, 256).to(self.device)

        # 训练的优化器
        self.value_optimizer = optim.Adam(self.value_net.parameters(), lr=self.value_lr)
        self.soft_q_optimizer = optim.Adam(self.soft_q_net.parameters(), lr=self.soft_q_lr)
        self.policy_optimizer = optim.Adam(self.policy_net.parameters(), lr=self.policy_lr)

        # 均方损失函数
        self.value_criterion = nn.MSELoss()
        self.soft_q_criterion = nn.MSELoss()

    # 储存记忆
    def store_transition(self, state, action, reward, next_state, done):
        self.memory.push((state, action, reward, next_state, done))

    # 选择动作
    def choose_action(self, s):
        s = torch.FloatTensor(s).to(self.device)
        mean, log_std = self.policy_net(s)
        std = log_std.exp()

        normal = Normal(mean, std)
        z = normal.sample()
        action = torch.tanh(z)

        action = action.detach().cpu().numpy()
        return action

    # 获取动作的log_prob
    def get_action_log_prob(self, s, epsilon=1e-6):
        mean, log_std = self.policy_net(s)
        std = log_std.exp()

        normal = Normal(mean, std)
        z = normal.sample()
        action = torch.tanh(z)

        log_prob = normal.log_prob(z) - torch.log(1 - action.pow(2) + epsilon)
        log_prob = log_prob.sum(-1, keepdim=True)
        # log_prob = Normal(mean, std).log_prob(mean + std * z.to(self.device)) - torch.log(1 - action.pow(2) + epsilon)  # reparameterization

        return action, log_prob, z, mean, log_std

    # 从经验池中选取样本
    def get_batch(self):
        transitions, _, _ = self.memory.sample(self.batch_size)  # 批样本

        # 解压批样本
        # 例如zipped为[(1, 4), (2, 5), (3, 6)]，zip(*zipped)解压为[(1, 2, 3), (4, 5, 6)]
        batch_state, batch_action, batch_reward, batch_next_state, batch_done = zip(*transitions)

        # 将样本转化为tensor
        batch_state = torch.tensor(batch_state, device=self.device, dtype=torch.float)
        batch_action = torch.tensor(batch_action, device=self.device, dtype=torch.float).squeeze() # view转换为列tensor
        batch_reward = torch.tensor(batch_reward, device=self.device, dtype=torch.float).squeeze().view(-1, 1)
        batch_next_state = torch.tensor(batch_next_state, device=self.device, dtype=torch.float)
        batch_done = torch.tensor(batch_done, device=self.device, dtype=torch.float).squeeze().view(-1, 1)
        # print("状态:", batch_state.shape) 128,4
        # print("动作:", batch_action.shape)
        # print("奖励:", batch_reward.shape)
        # print("done:", batch_done.shape)
        #
        return batch_state, batch_action, batch_reward, batch_next_state, batch_done, _, _

    # 学习
    def learn(self):
        # 获取批样本
        batch_state, batch_action, batch_reward, batch_next_state, batch_done, _, _ = self.get_batch()

        # print("状态:", batch_state)
        # print("动作:", batch_action)
        # print("done:", batch_done)

        expected_q_value = self.soft_q_net(batch_state, batch_action)  # q(s,a)
        expected_value = self.value_net(batch_state)  # v(s)
        new_action, log_prob, z, mean, log_std = self.get_action_log_prob(batch_state)  # a~, logpi(a~|s), dist, 均值，标准差

        target_value = self.target_value_net(batch_next_state)  # vtar(s')
        next_q_value = batch_reward + (1 - batch_done) * self.gamma * target_value  # r + gamma*(1-d)*vtar(s')
        q_value_loss = self.soft_q_criterion(expected_q_value, next_q_value.detach()).mean()

        expected_new_q_value = self.soft_q_net(batch_state, new_action)  # q(s,a~)
        next_value = expected_new_q_value - log_prob
        value_loss = self.value_criterion(expected_value, next_value.detach()).mean()

        log_prob_target = expected_new_q_value - expected_value  # q(s,a) - v(s)
        policy_loss = (log_prob * (log_prob - log_prob_target).detach()).mean()

        self.soft_q_optimizer.zero_grad()
        q_value_loss.backward()
        self.soft_q_optimizer.step()

        self.value_optimizer.zero_grad()
        value_loss.backward()
        self.value_optimizer.step()

        self.policy_optimizer.zero_grad()
        policy_loss.backward()
        self.policy_optimizer.step()

        for target_param, param in zip(self.target_value_net.parameters(), self.value_net.parameters()):
            target_param.data.copy_(
                target_param.data * (1.0 - self.tau) + param.data * self.tau
            )

        # 学习的步数加一
        self.learn_step_counter += 1

    # 保存模型
    def save(self):
        torch.save(self.soft_q_net, 'sac1_q.pkl')
        torch.save(self.value_net, 'sac1_v.pkl')
        torch.save(self.policy_net, 'sac1_policy.pkl')

    # 加载模型
    def load(self):
        self.soft_q_net = torch.load('sac1_q.pkl')
        self.value_net = torch.load('sac1_v.pkl')
        self.policy_net = torch.load('sac1_policy.pkl')

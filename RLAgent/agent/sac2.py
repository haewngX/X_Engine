import torch
import torch.optim as optim
import torch.nn as nn
import torch.nn.functional as F
import numpy as np
from model import ValueNetwork
from model import SoftQNetwork
from model import PolicyNetwork
from memory import ReplayMemory
from torch.distributions import Normal


class SAC(object):
    def __init__(self, config, env):
        self.device = config.device

        self.gamma = config.gamma  # 折扣因子

        self.tau = config.tau

        self.alpha = config.alpha

        # 学习率
        self.value_lr = config.value_lr
        self.soft_q_lr = config.soft_q_lr
        self.policy_lr = config.policy_lr

        self.replace_target_iter = config.replace_target_iter  # 目标网络更新频率
        self.replay_size = config.replay_size  # 经验池大小
        self.batch_size = config.batch_size  # 批样本数

        self.num_states = env.observation_space.shape[0]  # 状态空间维度
        self.num_actions = env.action_space.shape[0]  # 动作空间维度

        self.learn_start = self.batch_size * 3  # 控制学习的参数

        self.learn_step_counter = 0  # 学习的总步数

        self.memory = ReplayMemory(self.replay_size)  # 初始化经验池

        # 初始化Q网络
        self.soft_q1 = SoftQNetwork(self.num_states, self.num_actions, 256).to(self.device)
        self.soft_q2 = SoftQNetwork(self.num_states, self.num_actions, 256).to(self.device)
        self.soft_q1_target = SoftQNetwork(self.num_states, self.num_actions, 256).to(self.device)
        self.soft_q2_target = SoftQNetwork(self.num_states, self.num_actions, 256).to(self.device)

        # 目标网络和网络初始时参数一致
        for target_param, param in zip(self.soft_q1_target.parameters(), self.soft_q1.parameters()):
            target_param.data.copy_(param.data)
        for target_param, param in zip(self.soft_q2_target.parameters(), self.soft_q2.parameters()):
            target_param.data.copy_(param.data)

        # 初始化策略网络
        self.policy_net = PolicyNetwork(self.num_states, self.num_actions, 256).to(self.device)

        # 训练的优化器
        self.soft_q1_optimizer = optim.Adam(self.soft_q1.parameters(), lr=self.soft_q_lr)
        self.soft_q2_optimizer = optim.Adam(self.soft_q2.parameters(), lr=self.soft_q_lr)
        self.policy_optimizer = optim.Adam(self.policy_net.parameters(), lr=self.policy_lr)

        # 自动调节alpha Target Entropy = −dim(A) (e.g. , -6 for HalfCheetah-v2) as given in the paper
        # self.target_entropy = -torch.prod(torch.Tensor(env.action_space.shape).to(self.device)).item()
        # self.log_alpha = torch.zeros(1, requires_grad=True, device=self.device)
        # self.alpha_optim = optim.Adam([self.log_alpha], lr=config.learning_rate)

        # 均方损失函数
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
        return action[0]

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
        batch_action = torch.tensor(batch_action, device=self.device, dtype=torch.float).squeeze().view(-1, 1)  # view转换为列tensor
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

        with torch.no_grad():
            next_state_action, next_state_log_prob, _, _, _ = self.get_action_log_prob(batch_next_state)  # a', logpi(a'|s')
            q1_next_target = self.soft_q1_target(batch_next_state, next_state_action)  # q1θ'(s',a')
            q2_next_target = self.soft_q2_target(batch_next_state, next_state_action)  # q2θ'(s',a')
            q_next_target = torch.min(q1_next_target, q2_next_target) - self.alpha * next_state_log_prob
            # r(s,a) + gamma*(1-d)*(qθ'(s',a')-alpha*logpi(a'|s'))
            next_q_value = batch_reward + (1 - batch_done) * self.gamma * q_next_target

        q1 = self.soft_q1(batch_state, batch_action)  # q1θ(s,a)
        q2 = self.soft_q1(batch_state, batch_action)  # q2θ(s,a)
        q1_loss = self.soft_q_criterion(q1, next_q_value.detach()).mean()
        q2_loss = self.soft_q_criterion(q2, next_q_value.detach()).mean()

        next_action, log_prob, z, mean, log_std = self.get_action_log_prob(batch_state)  # a~, logpi(a~|s), dist, 均值，标准差
        q1_a = self.soft_q1(batch_state, next_action)  # q1θ(s,a~)
        q2_a = self.soft_q2(batch_state, next_action)  # q2θ(s,a~)

        min_qf_pi = torch.min(q1_a, q2_a)
        policy_loss = ((self.alpha * log_prob) - min_qf_pi).mean()  # Jπ = 𝔼st∼D,εt∼N[α * logπ(f(εt;st)|st) − Q(st,f(εt;st))]

        # 更新q1
        self.soft_q1_optimizer.zero_grad()
        q1_loss.backward()
        self.soft_q1_optimizer.step()

        # 更新q2
        self.soft_q2_optimizer.zero_grad()
        q2_loss.backward()
        self.soft_q2_optimizer.step()

        self.policy_optimizer.zero_grad()
        policy_loss.backward()
        self.policy_optimizer.step()

        # 更新alpha
        # alpha_loss = -(self.log_alpha * (log_prob + self.target_entropy).detach()).mean()
        # self.alpha_optim.zero_grad()
        # alpha_loss.backward()
        # self.alpha_optim.step()
        # self.alpha = self.log_alpha.exp()


        for target_param, param in zip(self.soft_q1_target.parameters(), self.soft_q1.parameters()):
            target_param.data.copy_(
                target_param.data * (1.0 - self.tau) + param.data * self.tau
            )

        for target_param, param in zip(self.soft_q2_target.parameters(), self.soft_q2.parameters()):
            target_param.data.copy_(
                target_param.data * (1.0 - self.tau) + param.data * self.tau
            )

        # 学习的步数加一
        self.learn_step_counter += 1

    # 保存模型
    def save(self):
        torch.save(self.soft_q1, 'sac2_q1.pkl')
        torch.save(self.soft_q2, 'sac2_q2.pkl')
        torch.save(self.policy_net, 'sac2_policy.pkl')

    # 加载模型
    def load(self):
        self.soft_q1 = torch.load('sac2_q1.pkl')
        self.soft_q2 = torch.load('sac2_q2.pkl')
        self.policy_net = torch.load('sac2_policy.pkl')
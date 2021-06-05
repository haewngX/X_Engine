import torch


class Config(object):

    def __init__(self):
        self.device = torch.device("cuda" if torch.cuda.is_available() else "cpu")

        # 环境名称
        self.name = ''

        # epsilon贪婪策略
        self.epsilon = 0.2  # epsilon初始值，以其概率选择最大值的动作
        self.epsilon_final = 0.001  # epsilon的最小值
        self.epsilon_decay = 0.99  # epsilon衰减率

        # 智能体相关参数
        self.gamma = 0.999  # 折扣因子
        self.learning_rate = 0.01  # 学习率

        self.replace_target_iter = 1  # 目标网络更新频率
        # self.replace_target_iter = 100
        self.tau = 0.005  # 目标网络软更新的参数

        # 经验池相关参数
        self.replay_size = 1000000
        self.batch_size = 128
        self.priority_alpha = 0.6
        self.priority_beta_start = 0.4
        self.priority_beta_frames = 100000

        # 各网络的学习率
        self.value_lr = 3e-4
        self.soft_q_lr = 3e-4
        self.policy_lr = 3e-4

        # 控制学习的参数
        self.learn_start = self.batch_size * 3

        # episode和step相关参数
        self.episode = 200  # 最大episode数
        self.episode_step = 800  # 每个episode的最大步数

        # N-step的步数
        self.N_step = 3

        # 控制最大熵强化学习的参数
        self.alpha = 0.2


import random
import tensorflow as tf

from bootstrap import WindowEvent


class RandomAgent:
    def __init__(self):
        self.actions = tf.constant([WindowEvent.PRESS_ARROW_RIGHT, WindowEvent.PRESS_BUTTON_A, WindowEvent.PRESS_BUTTON_B])
        self.action_size = len(self.actions)

    def get_action(self, state):
        action_index = random.choice(range(self.action_size))
        return self.actions[action_index]

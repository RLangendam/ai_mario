import sys
import numpy as np
import tensorflow as tf
from tensorflow import keras
from tensorflow.keras import layers
from pyboy import PyBoy, WindowEvent
import time
import random

class RandomAgent:
    def __init__(self):
        self.actions = [WindowEvent.PRESS_ARROW_RIGHT, WindowEvent.PRESS_BUTTON_A, WindowEvent.PRESS_BUTTON_B]
        self.action_size = len(self.actions)

    def get_action(self):
        action_index = random.choice(range(self.action_size))
        return self.actions[action_index]

# agent = RandomAgent()

class SequentialAgent(RandomAgent):
    def __init__(self):
        super().__init__()
        self.build_model()


    def build_model(self):
        initializer = tf.keras.initializers.GlorotNormal()

        self.model = tf.keras.Sequential()
        self.model.add(tf.keras.layers.Dense(50,
                                             input_shape=(320,),
                                             activation="relu",
                                             kernel_initializer=initializer))
        self.model.add(tf.keras.layers.Dense(self.action_size, activation="relu"))

        optimizer = tf.keras.optimizers.Adam(learning_rate=0.01)

        self.model.compile(loss='mse',
                           metrics=[],
                           optimizer = optimizer)

    def get_action(self, state):
        action_prediction = self.model.predict(state)
        action_random = super().get_action()
        action_model = self.actions[np.argmax(action_prediction)]
        print(action_prediction)
        return action_random if random.random() < 0.1 else action_model
        #return action_model


agent = SequentialAgent()
agent.model.summary()
#tf.keras.utils.plot_model(agent.model)

rom = "Mario1.gb"
quiet = "--quiet" in sys.argv
pyboy = PyBoy(rom, window_type="headless" if quiet else "SDL2", window_scale=3, debug=not quiet, game_wrapper=True)
pyboy.set_emulation_speed(0)
assert pyboy.cartridge_title() == "SUPER MARIOLAN"

mario = pyboy.game_wrapper()
gym = pyboy.openai_gym(observation_type="minimal", action_type="toggle", simultaneous_actions=False)

state = gym.reset()

for _ in range(500):
    action = agent.get_action(state.reshape(1,320))
    state, reward, done, info = gym.step(action)



pyboy.stop()
import random

import numpy as np
import tensorflow as tf

from RandomAgent import RandomAgent


class SequentialAgent(RandomAgent):
    def __init__(self, _):
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
        state = state.reshape(1,320)
        action_prediction = self.model.predict(state)
        action_random = super().get_action()
        action_model = self.actions[np.argmax(action_prediction)]
        print(action_prediction)
        return action_random if random.random() < 0.1 else action_model
        #return action_model
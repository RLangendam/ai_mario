import sys
import numpy
import tensorflow as tf

from bootstrap import bootstrap


class AiMario:
    def __init__(self):
        rom, PyBoy, self.WindowEvent = bootstrap()
        quiet = "--quiet" in sys.argv
        self.pyboy = PyBoy(rom, window_type="headless" if quiet else "SDL2", window_scale=3, debug=not quiet,
                           game_wrapper=True)

    @staticmethod
    def to_simple_value(n):
        if n == 144:
            return -1
        elif n < 100:
            return 2
        elif n > 350:
            return 1
        else:
            return 0

    @staticmethod
    def create_model(_):
        return (tf.keras.models.Sequential([
            tf.keras.layers.Dense(320, activation='relu', input_shape=(320,), kernel_initializer='random_normal'),
            # tf.keras.layers.Dense(100, activation='relu', input_shape=(320,), kernel_initializer='random_normal'),
            tf.keras.layers.Dense(2, activation='relu', input_shape=(320,), kernel_initializer='random_normal')
        ]), 0)

    def screen_to_action(self, model, screen):
        flattened = numpy.atleast_2d(screen.flatten())
        predictions = model.predict(flattened)
        max = numpy.argmax(predictions)
        return self.WindowEvent.PRESS_BUTTON_A if max == 0 else self.WindowEvent.PRESS_ARROW_RIGHT

    def run(self):
        self.pyboy.set_emulation_speed(0)
        assert self.pyboy.cartridge_title() == "SUPER MARIOLAN"

        mario = self.pyboy.game_wrapper()

        # botsupport = self.pyboy.botsupport_manager()

        gym = self.pyboy.openai_gym(observation_type="minimal", action_type="toggle", simultaneous_actions=False)

        state = gym.reset()

        assert mario.score == 0
        assert mario.lives_left == 2
        assert mario.time_left == 400
        assert mario.world == (1, 1)
        assert mario.fitness == 0  # A built-in fitness score for AI development
        last_fitness = 0

        print(mario)

        models = map(self.create_model, range(10))

        # print(model.summary())

        state, reward, done, info = gym.step(self.WindowEvent.PRESS_ARROW_RIGHT)
        for model, fitness in models:
            gym.reset()
            for _ in range(1000):
                state, reward, done, info = gym.step(self.screen_to_action(model, state))
            fitness = mario.fitness

        mario.reset_game()
        assert mario.lives_left == 2

        # time.sleep(10)
        self.pyboy.stop()


def main():
    mario = AiMario()
    mario.run()


if __name__ == "__main__":
    main()

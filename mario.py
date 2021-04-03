import sys
import numpy
import tensorflow as tf

from bootstrap import bootstrap

rom = bootstrap()

from pyboy import PyBoy, WindowEvent

quiet = "--quiet" in sys.argv
pyboy = PyBoy(rom, window_type="headless" if quiet else "SDL2", window_scale=3, debug=not quiet, game_wrapper=True)
pyboy.set_emulation_speed(0)
assert pyboy.cartridge_title() == "SUPER MARIOLAN"

mario = pyboy.game_wrapper()

botsupport = pyboy.botsupport_manager()

gym = pyboy.openai_gym()

state = gym.reset()

assert mario.score == 0
assert mario.lives_left == 2
assert mario.time_left == 400
assert mario.world == (1, 1)
assert mario.fitness == 0  # A built-in fitness score for AI development
last_fitness = 0

print(mario)


def to_simple_value(n):
    if n == 144:
        return -1
    elif n < 100:
        return 2
    elif n > 350:
        return 1
    else:
        return 0


def create_model(_):
    return (tf.keras.models.Sequential([
        tf.keras.layers.Dense(320, activation='relu', input_shape=(320,), kernel_initializer='random_normal'),
        # tf.keras.layers.Dense(100, activation='relu', input_shape=(320,), kernel_initializer='random_normal'),
        tf.keras.layers.Dense(2, activation='relu', input_shape=(320,), kernel_initializer='random_normal')
    ]), 0)


models = map(create_model, range(10))


# print(model.summary())


def screen_to_action(model, screen):
    flattened = numpy.atleast_2d(screen.flatten())
    predictions = model.predict(flattened)
    max = numpy.argmax(predictions)
    return WindowEvent.PRESS_BUTTON_A if max == 0 else WindowEvent.PRESS_ARROW_RIGHT


state, reward, done, info = gym.step(WindowEvent.PRESS_ARROW_RIGHT)
for model, fitness in models:
    gym.reset()
    for _ in range(1000):
        state, reward, done, info = gym.step(
            screen_to_action(model, numpy.array(list(map(lambda l: list(map(to_simple_value, l)), state)))))
    fitness = mario.fitness

mario.reset_game()
assert mario.lives_left == 2

# time.sleep(10)
pyboy.stop()

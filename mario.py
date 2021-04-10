import sys
import numpy
import tensorflow as tf

from bootstrap import rom, WindowEvent, PyBoy
from SequentialAgent import SequentialAgent
from RandomAgent import RandomAgent



class AiMario:
    def __init__(self):
        quiet = "--quiet" in sys.argv
        self.pyboy = PyBoy(rom, window_type="headless" if quiet else "SDL2", window_scale=3, debug=not quiet,
                           game_wrapper=True)

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

        agents = map(SequentialAgent, range(1))

        for agent in agents:
            state = gym.reset()
            for _ in range(1000):
                state, reward, done, info = gym.step(agent.get_action(state))

        print(list(agents))

        mario.reset_game()
        assert mario.lives_left == 2

        # time.sleep(10)
        self.pyboy.stop()


def main():
    mario = AiMario()
    mario.run()


if __name__ == "__main__":
    main()

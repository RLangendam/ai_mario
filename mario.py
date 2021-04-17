import sys
import numpy as np
from bootstrap import rom, PyBoy
from SequentialAgent import SequentialAgent
from EvolutionaryAlgorithm import EvolutionaryAlgorithm


class AiMario:
    def __init__(self):
        quiet = "--quiet" in sys.argv
        self.pyboy = PyBoy(rom, window_type="headless" if quiet else "SDL2", window_scale=3, debug=not quiet,
                           game_wrapper=True)

    def run(self):
        self.pyboy.set_emulation_speed(0)
        assert self.pyboy.cartridge_title() == "SUPER MARIOLAN"

        mario = self.pyboy.game_wrapper()
        gym = self.pyboy.openai_gym(
            observation_type="minimal", action_type="press", simultaneous_actions=False)

        mario_algorithm = EvolutionaryAlgorithm()
        mario_algorithm.run(agent=lambda _: SequentialAgent.create_random_initialized_agent(), gym=gym)

        self.pyboy.stop()


def main():
    mario = AiMario()
    mario.run()


if __name__ == "__main__":
    main()

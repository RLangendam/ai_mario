import sys
import numpy as np
from bootstrap import rom, PyBoy
from SequentialAgent import SequentialAgent
from EvolutionaryAlgorithm import EvolutionaryAlgorithm


class AiMario:
    def __init__(self, number_of_gyms=2):
        quiet = "--quiet" in sys.argv
        self.pyboys = [PyBoy(rom, window_type="headless" if quiet else "SDL2", window_scale=3, debug=not quiet,
                             game_wrapper=True) for _ in range(number_of_gyms)]

    def run(self):
        for pyboy in self.pyboys:
            pyboy.set_emulation_speed(0)
            assert pyboy.cartridge_title() == "SUPER MARIOLAN"
            mario = pyboy.game_wrapper()

        gyms = [pyboy.openai_gym(observation_type="minimal", action_type="all", simultaneous_actions=False)
                for pyboy in self.pyboys]

        mario_algorithm = EvolutionaryAlgorithm()
        mario_algorithm.run(agent=lambda _: SequentialAgent(), gyms=gyms)

        self.pyboys.stop()


def main():
    mario = AiMario()
    mario.run()


if __name__ == "__main__":
    main()

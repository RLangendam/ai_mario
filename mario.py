import sys
import numpy as np
from bootstrap import rom, PyBoy
from SequentialAgent import SequentialAgent
from EvolutionaryAlgorithm import EvolutionaryAlgorithm
import concurrent.futures


def create_gym(quiet):
    pyboy = PyBoy(rom, window_type="headless" if quiet else "SDL2", window_scale=3, debug=not quiet, game_wrapper=True)
    pyboy.set_emulation_speed(0)
    assert pyboy.cartridge_title() == "SUPER MARIOLAN"
    mario = pyboy.game_wrapper()
    gym = pyboy.openai_gym(observation_type="minimal", action_type="all", simultaneous_actions=False)
    return gym


class AiMario:
    def __init__(self, number_of_gyms=2):
        pass

    def run(self):
        mario_algorithm = EvolutionaryAlgorithm(create_gym)
        mario_algorithm.run(agent=lambda _: SequentialAgent())


def main():
    mario = AiMario()
    mario.run()


if __name__ == "__main__":
    main()

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

    def get_weight_bias_init(self):
        return [{"weights": np.random.rand(320, 50), "biases": np.random.rand(50)},
                {"weights": np.random.rand(50, 2), "biases": np.random.rand(2)}]

    def run(self):
        self.pyboy.set_emulation_speed(0)
        assert self.pyboy.cartridge_title() == "SUPER MARIOLAN"

        mario = self.pyboy.game_wrapper()
        gym = self.pyboy.openai_gym(
            observation_type="minimal", action_type="press", simultaneous_actions=False)

        mario_algorithm = EvolutionaryAlgorithm()
        mario_algorithm.run(agent=lambda _: SequentialAgent(self.get_weight_bias_init()), gym=gym)

        # population_size = 10
        #
        # weights_biases_init = [self.get_weight_bias_init() for _ in range(population_size)]
        #
        # agents = list(map(SequentialAgent, **weights_biases_init))
        #
        # for agent in agents:
        #     state = gym.reset()
        #     for _ in range(100):
        #         state, reward, done, info = gym.step(agent.get_action(state.reshape(1, 320)))
        #     agent.fitness = mario.fitness

        # time.sleep(10)
        self.pyboy.stop()


def main():
    mario = AiMario()
    mario.run()


if __name__ == "__main__":
    main()

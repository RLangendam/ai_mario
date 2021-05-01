import random
from SequentialAgent import SequentialAgent
import numpy as np
from time import time
from functools import partial

from StateTransform import MarioNotFoundException


class EvolutionaryAlgorithm:
    def __init__(self):
        self.average_fitness = []
        pass

    @staticmethod
    def initialize_population(agent, size):
        return list(map(agent, range(size)))

    def run(self, agent, gym, size=100, generation_count=10, selection_factor=0.04, agent_timeout=10000,
            survival_factor=0.5, sigma=0.2):
        population = self.initialize_population(agent, size)
        for generation in range(generation_count):
            fitnesses = list(map(self.compute_fitness(gym, agent_timeout), population))
            print(np.max(fitnesses))
            self.set_average_fitness(fitnesses)
            population = self.select_fittest(zip(population, fitnesses), selection_factor * size)
            population = self.recombine(population, size, survival_factor, SequentialAgent, sigma)
            # population = self.mutate(population)
        print(self.average_fitness)

    @staticmethod
    def compute_fitness(gym, agent_timeout):
        def implementation(agent):
            state = gym.reset()
            snapshot_state = state
            snapshot_time = time()
            try:
                for _ in range(agent_timeout):
                    state, reward, done, info = gym.step(agent.get_action(state))
                    if done or gym.game_wrapper.lives_left < 2:
                        break
                    now = time()
                    if now - snapshot_time > 0.8:
                        if (state == snapshot_state).all():
                            break
                        else:
                            snapshot_time = now
                            snapshot_state = state

            except MarioNotFoundException:
                pass
            return gym.game_wrapper.level_progress

        return implementation

    @staticmethod
    def select_fittest(agent_fitnesses, selection):
        return list(
            map(lambda x: x[0], list(sorted(agent_fitnesses, key=lambda x: x[1], reverse=True))[:int(selection)]))

    def set_average_fitness(self, fitnesses):
        self.average_fitness.append(sum(fitnesses) / len(fitnesses))
        print(self.average_fitness)

    @staticmethod
    def recombine_tensor(sigma, tensors):
        if isinstance(tensors[0], np.ndarray):
            return list(map(partial(EvolutionaryAlgorithm.recombine_tensor, sigma), zip(tensors[0], tensors[1])))
        else:
            mu = (tensors[0] + tensors[1]) / 2
            # print(mu)
            result = random.gauss(mu=mu, sigma=max(abs(tensors[0] - mu) / 2, sigma))
            return result

    @staticmethod
    def recombine_layers(sigma, layers):
        left_weights, left_biases = layers[0].get_weights()
        right_weights, right_biases = layers[1].get_weights()

        return {"weights": (
            list(map(partial(EvolutionaryAlgorithm.recombine_tensor, sigma), zip(left_weights, right_weights)))),
            "biases": (
                list(map(partial(EvolutionaryAlgorithm.recombine_tensor, sigma), zip(left_biases, right_biases))))}

    @staticmethod
    def recombine_parents(agent, sigma, parents):
        return agent(
            list(map(partial(EvolutionaryAlgorithm.recombine_layers, sigma),
                     zip(parents[0].model.layers, parents[1].model.layers))))

    @staticmethod
    def shuffle_list(list_to_shuffle):
        random.shuffle(list_to_shuffle, random.random)
        return list_to_shuffle

    @staticmethod
    def recombine(population, size, survival_factor, agent, sigma):
        new_population = population[:int(len(population) * survival_factor)]

        if len(population) % 2 == 1:
            population = population[1:]

        while len(new_population) < size:
            new_population.extend(map(partial(EvolutionaryAlgorithm.recombine_parents, agent, sigma),
                                      zip(EvolutionaryAlgorithm.shuffle_list(population),
                                          EvolutionaryAlgorithm.shuffle_list(population))))

        return new_population[:size]

    @staticmethod
    def mutate(population):
        def add_in_range(val, add, minval, maxval):
            newval = val + add
            if newval < minval:
                return minval
            if newval > maxval:
                return maxval
            return newval

        def mutate_layer(layer, mutation_frequency=0.01, mutation_strength=0.01):
            weights, biases = layer.get_weights()
            for i in map(lambda _: random.randrange(0, len(weights)), range(int(len(weights) * mutation_frequency))):
                for j in map(lambda _: random.randrange(0, len(weights[0])),
                             range(int(len(weights[0]) * mutation_frequency))):
                    weights[i][j] = add_in_range(weights[i][j], random.uniform(-1.0, 1.0) * mutation_strength, 0, 1)
            for i in map(lambda _: random.randrange(0, len(biases)), range(int(len(biases) * mutation_frequency))):
                biases[i] = add_in_range(biases[i], random.uniform(-1.0, 1.0) * mutation_strength, 0, 1)
            return {"weights": weights, "biases": biases}

        def mutate_agent(agent):
            return SequentialAgent(list(map(mutate_layer, agent.model.layers)))

        return list(map(mutate_agent, population))

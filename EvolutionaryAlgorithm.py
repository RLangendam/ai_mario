import multiprocessing
import random
import numpy as np
from time import time
from functools import partial
from statistics import mean, stdev
import concurrent.futures
from itertools import cycle
from threading import local

from StateTransform import MarioNotFoundException

data = local()

class EvolutionaryAlgorithm:
    def __init__(self, gym_factory):
        self.create_gym = gym_factory
        pass

    @staticmethod
    def initialize_population(agent, size):
        return list(map(agent, range(size)))

    def run(self, agent, size=100, generation_count=10, selection_factor=0.04, agent_timeout=10000,
            survival_factor=0.5, sigma=0.005):
        population = self.initialize_population(agent, size)
        with concurrent.futures.ThreadPoolExecutor(max_workers=multiprocessing.cpu_count()) as executor:
            for generation in range(generation_count):
                future_to_fitness = {executor.submit(self.compute_fitness, agent, agent_timeout, True): agent
                                     for agent in population}
                agent_to_fitness = []
                for future in concurrent.futures.as_completed(future_to_fitness):
                    agent_to_fitness.append((future_to_fitness[future], future.result()))
                fitnesses = [pair[1] for pair in agent_to_fitness]
                print(f'mean={mean(fitnesses)}, stddev={stdev(fitnesses)}, max={np.max(fitnesses)}')
                population = self.select_fittest(agent_to_fitness, selection_factor * size)
                population = self.recombine(population, size, survival_factor, agent, sigma)
        for _ in range(10):
            self.compute_fitness(population[0], agent_timeout, False)

    def compute_fitness(self, agent, agent_timeout, quiet):
        if not hasattr(data, 'gym'):
            data.gym = self.create_gym(quiet)

        state = data.gym.reset()
        snapshot_state = state
        snapshot_time = time()
        try:
            for _ in range(agent_timeout):
                state, reward, done, info = data.gym.step(agent.get_action(state))
                if done or data.gym.game_wrapper.lives_left < 2:
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
        return data.gym.game_wrapper.level_progress

    @staticmethod
    def select_fittest(agent_fitnesses, selection):
        return list(
            map(lambda x: x[0], list(sorted(agent_fitnesses, key=lambda x: x[1], reverse=True))[:int(selection)]))

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

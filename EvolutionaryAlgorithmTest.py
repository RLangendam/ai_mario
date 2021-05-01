import unittest

import numpy as np

from EvolutionaryAlgorithm import EvolutionaryAlgorithm


class MicroMock(object):
    def __init__(self, **kwargs):
        self.__dict__.update(kwargs)


class MyTestCase(unittest.TestCase):
    def test_recombine_tensor(self):
        self.assertEqual(1, EvolutionaryAlgorithm.recombine_tensor(0, [1, 1]))
        self.assertAlmostEqual(1.5, EvolutionaryAlgorithm.recombine_tensor(0, [1, 2]), delta=1.5)
        value = np.ndarray(shape=(3,), buffer=np.array([1.0, 2.0, 3.0]), dtype=float)
        self.assertEqual([1.0, 2.0, 3.0],
                         EvolutionaryAlgorithm.recombine_tensor(0, [value, value]))

    def recombine(self, weights, biases):
        agent = MicroMock(
            model=MicroMock(layers=[MicroMock(get_weights=lambda: (weights, biases))]))
        result = EvolutionaryAlgorithm.recombine([agent, agent], 2, 0, lambda x: x, 0)
        self.assertSequenceEqual(weights.tolist(), result[0][0]['weights'])
        self.assertSequenceEqual(weights.tolist(), result[1][0]['weights'])
        self.assertSequenceEqual(biases.tolist(), result[0][0]['biases'])
        self.assertSequenceEqual(biases.tolist(), result[1][0]['biases'])

    def test_recombine(self):
        self.recombine(np.ndarray(shape=(1,), buffer=np.array([2.0])), np.ndarray(shape=(1,), buffer=np.array([3.0])))
        self.recombine(np.ndarray(shape=(2,), buffer=np.array([2.0, 3.0])),
                       np.ndarray(shape=(2,), buffer=np.array([4.0, 5.0])))
        self.recombine(np.ndarray(shape=(2, 2), buffer=np.array([[2.0, 3.0], [1.0, 4.0]])),
                       np.ndarray(shape=(2,), buffer=np.array([7.0, 8.0])))


if __name__ == '__main__':
    unittest.main()

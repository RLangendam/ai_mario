import numpy as np


# from keras.utils.np_utils import to_categorical


class MarioNotFoundException(BaseException):
    pass


def find_mario(state):
    mario = np.where(state == 1)
    if len(mario[0]) == 0 or len(mario[1]) == 0:
        raise MarioNotFoundException()
    return np.max(mario[0]), np.min(mario[1])


def transform_state(state, width=8, height=5):
    # print(state)
    mario_position_x, mario_position_y = find_mario(state)
    sliced_state = state[mario_position_x - height: mario_position_x + 3, mario_position_y: mario_position_y + width]

    # print(sliced_state)

    # 0 = free
    # 1 = mario
    # 2 = enemy
    # 3 = blocked
    # transform to
    # 0, 1 = free, blocked
    # 0, 1 = no enemy, enemy

    def blocked(cell):
        return 1 if cell == 3 else 0

    def enemy(cell):
        return 1 if cell == 2 else 0

    def hot_one(row):
        return [[blocked(cell), enemy(cell)] for cell in row]

    result = np.array([hot_one(row) for row in sliced_state])

    # print(result)

    return result

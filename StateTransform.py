import numpy as np


class MarioNotFoundException(BaseException):
    pass

def find_mario(state):
    mario = np.where(state == 1)
    if len(mario[0]) == 0 or len(mario[1]) == 0:
        raise MarioNotFoundException()
    return np.max(mario[0]), np.min(mario[1])


def transform_state(state, width=8, height=5):
    print(state)
    mario_position_x, mario_position_y = find_mario(state)

    new_state = state[mario_position_x - height: mario_position_x + 3, mario_position_y: mario_position_y + width]
    print(new_state)

    return new_state.reshape(1, 64)

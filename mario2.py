import sys
from pyboy import PyBoy

# agent = RandomAgent()
from SequentialAgent import SequentialAgent


def main():
    agent = SequentialAgent()
    agent.model.summary()
    # tf.keras.utils.plot_model(agent.model)

    rom = "Mario1.gb"
    quiet = "--quiet" in sys.argv
    pyboy = PyBoy(rom, window_type="headless" if quiet else "SDL2", window_scale=3, debug=not quiet, game_wrapper=True)
    pyboy.set_emulation_speed(0)
    assert pyboy.cartridge_title() == "SUPER MARIOLAN"

    mario = pyboy.game_wrapper()
    gym = pyboy.openai_gym(observation_type="minimal", action_type="toggle", simultaneous_actions=False)

    state = gym.reset()

    for _ in range(500):
        action = agent.get_action(state.reshape(1, 320))
        state, reward, done, info = gym.step(action)

    pyboy.stop()

if __name__ == "__main__":
    main()
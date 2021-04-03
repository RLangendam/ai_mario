import os
import sys
import urllib.request
import zipfile


def download(url, filename):
    if not os.path.exists(filename):
        urllib.request.urlretrieve(url + filename, filename)


sdl_archive = "SDL2-devel-2.0.10-VC.zip"
download("https://www.libsdl.org/release/", sdl_archive)

if not os.path.exists("SDL2-2.0.10"):
    with zipfile.ZipFile(sdl_archive, 'r') as zip_ref:
        zip_ref.extractall()

rom = "Mario1.gb"
download("https://sites.google.com/site/nosurveyemus/the-gameboy-page/", rom)

# Makes us able to import PyBoy from the directory below
file_path = os.path.dirname(os.path.realpath(__file__))
sys.path.insert(0, file_path + "/..")

os.environ["PYSDL2_DLL_PATH"] = os.path.join(file_path, "SDL2-2.0.10/lib/x64");

from pyboy import PyBoy, WindowEvent

quiet = "--quiet" in sys.argv
pyboy = PyBoy(rom, window_type="headless" if quiet else "SDL2", window_scale=3, debug=not quiet, game_wrapper=True)
pyboy.set_emulation_speed(0)
assert pyboy.cartridge_title() == "SUPER MARIOLAN"

mario = pyboy.game_wrapper()
mario.start_game()

assert mario.score == 0
assert mario.lives_left == 2
assert mario.time_left == 400
assert mario.world == (1, 1)
assert mario.fitness == 0  # A built-in fitness score for AI development
last_fitness = 0

print(mario)

pyboy.send_input(WindowEvent.PRESS_ARROW_RIGHT)
for _ in range(1000):
    assert mario.fitness >= last_fitness
    last_fitness = mario.fitness

    pyboy.tick()
    if mario.lives_left == 1:
        assert last_fitness == 27700
        assert mario.fitness == 17700  # Loosing a live, means 10.000 points in this fitness scoring
        print(mario)
        break
else:
    print("Mario didn't die?")
    exit(2)

mario.reset_game()
assert mario.lives_left == 2

pyboy.stop()

import os
import sys
import urllib.request
import zipfile


def download(url, filename):
    if not os.path.exists(filename):
        urllib.request.urlretrieve(url + filename, filename)


sdl_version = "2.0.14"
sdl_archive = f'SDL2-devel-{sdl_version}-VC.zip'
download("https://www.libsdl.org/release/", sdl_archive)

if not os.path.exists("SDL2" + sdl_version):
    with zipfile.ZipFile(sdl_archive, 'r') as zip_ref:
        zip_ref.extractall()

rom = "Mario1.gb"
download("https://sites.google.com/site/nosurveyemus/the-gameboy-page/", rom)

# Makes us able to import PyBoy from the directory below
file_path = os.path.dirname(os.path.realpath(__file__))
sys.path.insert(0, file_path + "/..")

os.environ["PYSDL2_DLL_PATH"] = os.path.join(file_path, f'SDL2-{sdl_version}/lib/x64')

from pyboy import PyBoy, WindowEvent

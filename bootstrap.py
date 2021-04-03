import os
import sys
import urllib.request
import zipfile


def bootstrap():
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

    os.environ["PYSDL2_DLL_PATH"] = os.path.join(file_path, "SDL2-2.0.10/lib/x64")
    return rom

import gdown
import pathlib
import py7zr
import shutil

gdown.download(id="1jCf-Ye7pAQLVep6MT1HA_G05gdEgWeKk", output="data.7z")
with py7zr.SevenZipFile("data.7z", mode="r") as z:
    z.extractall()
shutil.move("knapsack", "data")
pathlib.Path("data.7z").unlink()

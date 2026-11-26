import gdown
import pathlib
import py7zr
import shutil

gdown.download(id="1jCf-Ye7pAQLVep6MT1HA_G05gdEgWeKk", output="data.7z")
with py7zr.SevenZipFile("data.7z", mode="r") as z:
    z.extractall()
# data/ already exists, since it contains the hand-made test instances stored
# in the repository: merge the archive into it.
shutil.copytree("knapsack", "data", dirs_exist_ok=True)
shutil.rmtree("knapsack")
pathlib.Path("data.7z").unlink()

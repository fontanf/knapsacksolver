import gdown
import os
import pathlib


def download(id):
    gdown.download(id=id, output="data.7z")
    os.system("7z x data.7z -odata")
    pathlib.Path("data.7z").unlink()

download("1jCf-Ye7pAQLVep6MT1HA_G05gdEgWeKk")
download("1tNIfm81yBp7DU3qfQVLLnewYwKlyzsrt")

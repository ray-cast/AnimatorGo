import glob
import os
import sys

def main():
    views_files = glob.glob("../views/*.cpp")
    widget_files = glob.glob("../widgets/*.cpp")
    qml_files = glob.glob("../res/qml/*.qml")
    files = views_files + widget_files + qml_files
    langs = ["en_US", "de_DE", "fr_FR", "it_IT", "es_ES", "pt_BR", "ru_RU", "zh_CN", "ja_JP"]
    for l in langs:
        # run command to update the languages
        os.system(f"lupdate {' '.join(files)} -ts {l}.ts")

if __name__ == "__main__":
    main()
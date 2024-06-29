import glob
import os
import sys

def main():
    files = glob.glob("../views/*.cpp")
    langs = ["en_US", "de_DE", "fr_FR", "it_IT", "es_ES", "pt_BR", "ru_RU", "zh_CN", "ja_JP"]
    for l in langs:
        out_p = os.path.join("../res/languages", f"{l}.qm")
        if os.path.exists(out_p):
            os.remove(out_p)
        # run command to update the languages
        os.system(f"lconvert -o {out_p} -i ../languages/{l}.ts")

if __name__ == "__main__":
    main()
# this script will ran all the images in ../assets/images
# through ../tools/argb_converter.exe

import os
import subprocess

input_path = "../assets/images"
output_path = "../data"
tool_path = "../tools/texconv.exe"


def main():

    print("starting script...")

    images = os.listdir(input_path)
    for image in images:
        result = subprocess.run(
            [os.path.abspath(tool_path), "-pmalpha", "-m", "1", "-f", "BC3_UNORM",
             os.path.abspath(input_path) + "\\" + image],
            capture_output=True,
            cwd=os.path.abspath(output_path),
            text=True,
        )

        print("texture_converter: ", result.stdout)


if __name__ == "__main__":
    main()

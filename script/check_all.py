import os
import glob

input_folder = "data/input/"
input_file_paths = glob.glob(os.path.join(input_folder, "*"))


for input_file_path in input_file_paths:
    output_file_path = "data/solution/" + input_file_path.split("/")[-1].replace(
        "graph", "sol"
    )
    out = os.popen("./test/checker {0} {1}".format(input_file_path, output_file_path))
    print(out.read())

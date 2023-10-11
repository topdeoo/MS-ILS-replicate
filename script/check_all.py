import os
import glob

input_folder = "data/input/"
input_file_paths = glob.glob(os.path.join(input_folder, "*"))


output_folder = "data/solution/"
output_file_paths = glob.glob(os.path.join(output_folder, "*"))

n = len(input_file_paths)

for i in range(n):
    os.popen("./test/checker {0} {1}".format(input_file_paths[i], output_file_paths[i]))

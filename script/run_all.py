import os
import glob

folder_path = "data/input/"

file_paths = glob.glob(os.path.join(folder_path, "*"))

for file_path in file_paths:
    output = "data/solution/" + file_path.split("/")[-1].replace("graph", "sol")
    os.popen("./build/capmds {0} > {1}".format(file_path, output))

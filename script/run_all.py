import os
import glob

folder_path = "data/input/"

file_paths = glob.glob(os.path.join(folder_path, "*"))

# TAG here the capacity of each node is same
for file_path in file_paths:
    output = "data/solution/" + file_path.split("/")[-1].replace("graph", "sol")
    os.popen("./build/capmds {0} -t > {1}".format(file_path, output))

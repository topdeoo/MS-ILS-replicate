import sys

m = 0


def transform2int(ch):
    return ord(ch) - ord("a")


def transform2char(i):
    return chr(i + ord("a"))


def transform2graph(filename):
    with open(filename) as f:
        line = f.readline().split(" ")
        m = int(line[1])
        print("{0} {1}".format(line[0], line[1]))
        for _ in range(m):
            line = f.readline().strip().split(" ")
            print("{0} {1}".format(transform2int(line[0]), transform2int(line[1])))


def transform2txt(filename):
    with open(filename) as f:
        line = f.readline().strip().split(" ")
        for i in line:
            print(transform2char(int(i)), end="")


if __name__ == "__main__":
    mode = sys.argv[1]
    filename = sys.argv[2]
    if mode == "graph":
        transform2graph(filename)
    else:
        transform2txt(filename)

from cyaron import *
import sys


def no_weight():
    pass


if __name__ == "__main__":
    n, m = int(sys.argv[1]), int(sys.argv[2])
    connected = sys.argv[3] == "-c"
    capacity = int(sys.argv[4])
    if connected:
        graph = Graph.UDAG(
            n, m, self_loop=False, repeated_edges=False, weight_gen=no_weight
        )
    else:
        graph = Graph.graph(
            n, m, self_loop=False, repeated_edges=False, weight_gen=no_weight
        )

    with open(
        "data/input/{0}_{1}_{3}_{2}.graph".format(
            n, m, "connected" if connected else "unconnected", capacity
        ),
        "w+",
    ) as f:
        f.write(str(n) + " " + str(m) + " " + str(capacity) + "\n")
        for edge in graph.iterate_edges():
            f.write(str(edge.start - 1) + " " + str(edge.end - 1) + "\n")

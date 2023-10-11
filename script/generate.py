import os

V = [50, 100, 250, 500, 800, 1000]
E = [100, 250, 500, 1000, 2000, 5000, 10000]
capacity = [2, 5]


for v in V:
    for e in E:
        for c in capacity:
            if v <= 100 and e > 500:
                continue
            elif v <= 500 and e > 2000:
                continue
            elif v <= 1000 and e > 5000:
                continue
            else:
                if e > v:
                    os.popen(
                        "python script/data_generate.py {} {} -c {}".format(v, e, c)
                    )

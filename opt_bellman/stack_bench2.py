import subprocess
import os
import os.path

if __name__ == '__main__':

    data = []
    for i in range(1, 21):
        data.append(os.path.join(
            "external",
            "KnapsackPisingerInstances",
            "smallcoeff",
            "knapPI_3_1000_10000",
            str(i) + ".txt"))

    args = [
            os.path.join("external", "BenchTools", "bench"),
            "-e", os.path.join("opt_bellman", "list"),
            "-i"] + data + [
            "-o", "Bench.csv",
    ]

    subprocess.call(args)


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
            "knapPI_3_500_1000",
            str(i) + ".txt"))

    args = [
            os.path.join("external", "BenchTools", "bench"),
            "-e", os.path.join("DynamicProgramming", "list"),
            "-i"] + data + [
            "-o", "Bench.csv",
    ]

    subprocess.call(args)


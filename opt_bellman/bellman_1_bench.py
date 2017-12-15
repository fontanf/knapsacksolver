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
            "-e", os.path.join("opt_bellman", "main") + " -ait",
            "-i"] + data + [
            "-o", "ItBench.csv",
    ]
    subprocess.call(args)

    args = [
            os.path.join("external", "BenchTools", "bench"),
            "-e", os.path.join("opt_bellman", "main") + " -arec",
            "-i"] + data + [
            "-o", "RecBench.csv",
    ]
    subprocess.call(args)

    args = [
            os.path.join("external", "BenchTools", "bench"),
            "-e", os.path.join("opt_bellman", "main") + " -astack",
            "-i"] + data + [
            "-o", "StackBench.csv",
    ]
    subprocess.call(args)

    args = [
            os.path.join("external", "BenchTools", "bench"),
            "-e", os.path.join("opt_bellman", "main") + " -amap",
            "-i"] + data + [
            "-o", "MapBench.csv",
    ]
    subprocess.call(args)


#include "subsetsumsolver/generator.hpp"

using namespace subsetsumsolver;

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    std::mt19937_64 generator;

    // pthree.
    std::cout << "Generate pthree..." << std::endl;
    for (ItemPos n: {10, 30, 100, 300, 1000, 3000, 10000}) {
        for (int seed = 0; seed < 100; ++seed) {
            generator.seed(seed);
            Instance instance = generate_pthree(n, generator);
            std::string instance_path
                = "data/subsetsum/pthree/pthree_"
                + std::to_string(n) + "_" + std::to_string(seed);
            instance.write(instance_path);
        }
    }

    // psix.
    std::cout << "Generate psix..." << std::endl;
    for (ItemPos n: {10, 30, 100, 300, 1000, 3000}) {
        for (int seed = 0; seed < 100; ++seed) {
            generator.seed(seed);
            Instance instance = generate_psix(n, generator);
            std::string instance_path
                = "data/subsetsum/psix/psix_"
                + std::to_string(n) + "_" + std::to_string(seed);
            instance.write(instance_path);
        }
    }

    // evenodd.
    std::cout << "Generate evenodd..." << std::endl;
    for (ItemPos n: {10, 30, 100, 300, 1000, 3000, 10000}) {
        for (int seed = 0; seed < 100; ++seed) {
            generator.seed(seed);
            Instance instance = generate_evenodd(n, generator);
            std::string instance_path
                = "data/subsetsum/evenodd/evenodd_"
                + std::to_string(n) + "_" + std::to_string(seed);
            instance.write(instance_path);
        }
    }

    // avis.
    std::cout << "Generate avis..." << std::endl;
    for (ItemPos n: {10, 30, 100, 300, 1000}) {
        generator.seed(0);
        Instance instance = generate_avis(n);
        std::string instance_path
            = "data/subsetsum/avis/avis_"
            + std::to_string(n);
        instance.write(instance_path);
    }

    // todd.
    std::cout << "Generate todd..." << std::endl;
    for (ItemPos n: {10}) {
        generator.seed(0);
        Instance instance = generate_todd(n);
        std::string instance_path
            = "data/subsetsum/todd/todd_"
            + std::to_string(n);
        instance.write(instance_path);
    }

    // somatoth.
    /*
    std::cout << "Generate somatoth..." << std::endl;
    for (ItemPos n: {10, 30, 100, 300, 1000, 3000, 10000}) {
        std::cout << "n " << n << std::endl;
        for (int seed = 0; seed < 100; ++seed) {
            generator.seed(seed);
            Instance instance = generate_somatoth(n, generator);
            std::string instance_path
                = "data/subsetsum/somatoth/somatoth_"
                + std::to_string(n) + "_" + std::to_string(seed);
            instance.write(instance_path);
        }
    }
    */

    // evenodd6.
    std::cout << "Generate evenodd6..." << std::endl;
    for (ItemPos n: {1000, 3000, 5000, 8000, 10000, 20000, 30000, 40000, 50000, 60000, 70000, 80000, 90000, 100000}) {
        for (int seed = 0; seed < 1; ++seed) {
            generator.seed(seed);
            Instance instance = generate_evenodd6(n, generator);
            std::string instance_path
                = "data/subsetsum/evenodd6/evenodd6_"
                + std::to_string(n) + "_" + std::to_string(seed);
            instance.write(instance_path);
        }
    }

    // evenodd8.
    std::cout << "Generate evenodd8..." << std::endl;
    for (ItemPos n: {10, 50, 100, 150, 200, 300, 500, 1000, 5000, 10000, 50000}) {
        for (int seed = 0; seed < 1; ++seed) {
            generator.seed(seed);
            Instance instance = generate_evenodd8(n, generator);
            std::string instance_path
                = "data/subsetsum/evenodd8/evenodd8_"
                + std::to_string(n) + "_" + std::to_string(seed);
            instance.write(instance_path);
        }
    }

    // tenfive6.
    std::cout << "Generate tenfive6..." << std::endl;
    for (ItemPos n: {1000, 3000, 5000, 8000, 10000, 20000, 30000, 40000, 50000, 60000, 70000, 80000, 90000, 100000}) {
        for (int seed = 0; seed < 1; ++seed) {
            generator.seed(seed);
            Instance instance = generate_tenfive6(n, generator);
            std::string instance_path
                = "data/subsetsum/tenfive6/tenfive6_"
                + std::to_string(n) + "_" + std::to_string(seed);
            instance.write(instance_path);
        }
    }

    // tenfive8.
    std::cout << "Generate tenfive8..." << std::endl;
    for (ItemPos n: {10, 50, 100, 150, 200, 300, 500, 1000, 5000, 10000, 50000}) {
        for (int seed = 0; seed < 1; ++seed) {
            generator.seed(seed);
            Instance instance = generate_tenfive8(n, generator);
            std::string instance_path
                = "data/subsetsum/tenfive8/tenfive8_"
                + std::to_string(n) + "_" + std::to_string(seed);
            instance.write(instance_path);
        }
    }

    return 0;
}

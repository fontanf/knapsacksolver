#include "knapsacksolver/tester.hpp"

#include "knapsacksolver/solution.hpp"
#include "knapsacksolver/generator.hpp"

using namespace knapsacksolver;

bool test(const Instance& instance, std::vector<Output (*)(Instance&)> algorithms, TestType test_type)
{
    Profit opt = -1;
    for (auto f: algorithms) {
        Instance instance_tmp = instance;
        Output output = f(instance_tmp);
        if (opt == -1) {
            opt = output.lower_bound;
            continue;
        }
        if (test_type == SOPT) {
            EXPECT_EQ(output.lower_bound, opt);
            EXPECT_EQ(output.lower_bound, output.upper_bound);
            EXPECT_TRUE(output.solution.feasible());
            EXPECT_EQ(output.solution.profit(), output.lower_bound);
        } else if (test_type == OPT) {
            EXPECT_EQ(output.lower_bound, opt);
            EXPECT_EQ(output.lower_bound, output.upper_bound);
        } else if (test_type == UB) {
            EXPECT_GE(output.upper_bound, opt);
        } else if (test_type == LB) {
            EXPECT_LE(output.lower_bound, opt);
            EXPECT_TRUE(output.solution.feasible());
            EXPECT_EQ(output.solution.profit(), output.lower_bound);
        }
    }
    return true;
}

class Instances
{
public:
    virtual Instance next() = 0;
    bool end = false;
};

class TestInstances: public Instances
{

public:

    Instance next()
    {
        i++;
        if (i == 15)
            end = true;
        return test_instance(i);
    }

    Instance test_instance(Counter i)
    {
        switch (i) {
        case 0: // 0 item
            return Instance(0, {});
        case 1: // 1 item
            return Instance(8, {{6, 8}});
        case 2: // 2 items
            return Instance(10, {{9, 8}, {7, 6}});
        case 3:
            return Instance(10, {{9, 5}, {7, 6}});
        case 4:
            return Instance(10, {{5, 5}, {4, 6}});
        case 5:
            return Instance(12, {{5, 7}, {4, 5}, {3, 11}});
        case 6:
            return Instance(10, {{5, 7}, {4, 5}, {3, 11}});
        case 7:
            return Instance(10, {{1, 1}, {2, 2}, {9, 10}});
        case 8: // 4 items
            return Instance(95, {{24, 34}, {42, 52}, {32, 42}, {4, 14}});
        case 9: // 5 items
            return Instance(100, {{10, 10}, {10, 15}, {10, 5}, {10, 12}, {10, 20}});
        case 10:
            return Instance(15, {{6, 7}, {6, 7}, {5, 5}, {5, 5}, {5, 5}});
        case 11:
            return Instance(12, {{3, 3}, {2, 2}, {4, 4}, {10, 9}, {10, 8}});
        case 12: // 6 items
            return Instance(25, {{10, 100}, {6, 7}, {6, 7}, {5, 5}, {5, 5}, {5, 5}});
        case 13:
            return Instance(1884, {{829, 729}, {785, 685}, {710, 610}, {781, 681}, {128, 28}, {243, 143}});
        case 14: // 7 items
            return Instance(40, {{11, 12}, {11, 12}, {11, 12}, {10, 10}, {10, 10}, {10, 10}, {10, 10}});
        case 15:
            return Instance(9, {{2, 6}, {3, 5}, {6, 8}, {7, 9}, {5, 6}, {9, 7}, {4, 3}});
        default:
            return Instance(0, {});
        }
    }

private:

    Counter i = 0;

};

class SmallInstances: public Instances
{

public:

    Instance next()
    {
        s += 1;
        if (s > s_max) {
            s = 0;
            h += 1;
            if (h > h_max) {
                h = 1;
                r += 1;
                if (r > r_max) {
                    r = 2;
                    n += 1;
                }
            }
        }

        if (s == s_max && h == h_max && r == r_max && n == n_max)
            end = true;

        Generator data;
        data.n = n;
        data.r = r;
        data.t = "u";
        data.h = h;
        data.s = s;

        std::cout << "data " << data << std::endl;
        return data.generate();
    }

private:

    ItemIdx n_max = 10;
    Weight r_max = 10;
    Counter h_max = 100;
    Seed s_max = 7;
    ItemIdx n = 1;
    Weight r = 2;
    Counter h = 1;
    Seed s = -1;
};

class PisingerInstances: public Instances
{

public:

    PisingerInstances(
            const std::vector<ItemIdx>& ns,
            const std::vector<Weight>& rs,
            const std::vector<std::string>& ts,
            Seed s_max):
        ns(ns), rs(rs), ts(ts), s_max(s_max) {  }

    Instance next()
    {
        s += 1;
        if (s > s_max) {
            s = 0;
            h += 1;
            if (h > 100) {
                h = 1;
                ri += 1;
                if (ri >= (Counter)rs.size()) {
                    ri = 0;
                    ti += 1;
                    if (ti >= (Counter)ts.size()) {
                        ti = 0;
                        ni += 1;
                    }
                }
            }
        }

        if (s == s_max
                && h == 100
                && ri == (Counter)rs.size() - 1
                && ti == (Counter)ts.size() - 1
                && ni == (Counter)ns.size() - 1)
            end = true;

        Generator data;
        data.n = ns[ni];
        data.r = rs[ri];
        data.t = ts[ti];
        data.h = h;
        data.s = s;

        std::cout << "data " << data << std::endl;
        return data.generate();
    }

private:

    std::vector<ItemIdx> ns;
    std::vector<Weight> rs;
    std::vector<std::string> ts;
    Seed s_max = 10;
    Counter h = 1;
    Seed s = -1;
    Counter ni = 0;
    Counter ri = 0;
    Counter ti = 0;

};

void test(Instances& instances, std::vector<Output (*)(Instance&)> fs, TestType test_type)
{
    while (!instances.end) {
        Instance instance(instances.next());
        //std::cout << instance << std::endl;
        bool b = test(instance, fs, test_type);
        if (!b) {
            std::cout << "error" << std::endl;
            return;
        }
    }
}

void knapsacksolver::test(
        InstacesType instances_type,
        std::vector<Output (*)(Instance&)> algorithms,
        TestType test_type)
{
    switch (instances_type) {
    case TEST: {
        TestInstances instances;
        test(instances, algorithms, test_type);
        break;
    } case SMALL: {
        SmallInstances instances;
        test(instances, algorithms, test_type);
        break;
    } case MEDIUM: {
        PisingerInstances instances({50}, {100}, {"u", "wc", "sc", "isc", "asc", "ss"}, 3);
        test(instances, algorithms, test_type);
        break;
    } case SC: {
        PisingerInstances instances({2, 5, 10, 20, 50, 100, 200, 500}, {10, 100, 1000}, {"sc"}, 3);
        test(instances, algorithms, test_type);
        break;
    } default: {
    }
    }
}


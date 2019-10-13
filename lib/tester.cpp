#include "knapsack/lib/tester.hpp"

#include "knapsack/lib/solution.hpp"
#include "knapsack/lib/generator.hpp"

using namespace knapsack;

bool test(const Instance& ins, std::vector<Output (*)(Instance&)> fs, TestType tt)
{
    Profit opt = -1;
    for (auto f: fs) {
        Instance ins_tmp = ins;
        Output output = f(ins_tmp);
        if (opt == -1) {
            opt = output.lower_bound;
            continue;
        }
        if (tt == SOPT) {
            EXPECT_EQ(output.lower_bound, opt);
            EXPECT_EQ(output.lower_bound, output.upper_bound);
            EXPECT_TRUE(output.solution.feasible());
            EXPECT_EQ(output.solution.profit(), output.lower_bound);
        } else if (tt == OPT) {
            EXPECT_EQ(output.lower_bound, opt);
            EXPECT_EQ(output.lower_bound, output.upper_bound);
        } else if (tt == UB) {
            EXPECT_GE(output.upper_bound, opt);
        } else if (tt == LB) {
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
};

typedef std::vector<std::pair<Weight, Profit>> v;
class TestInstances: public Instances
{

public:

    Instance next()
    {
        i++;
        return test_instance(i);
    }

    Instance test_instance(Cpt i)
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

    Cpt i = 0;

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
                    if (n > n_max)
                        return Instance(0, {});
                }
            }
        }

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
    Cpt h_max = 100;
    Seed s_max = 20;
    ItemIdx n = 1;
    Weight r = 2;
    Cpt h = 1;
    Seed s = -1;
};

class PisingerInstances: public Instances
{

public:

    PisingerInstances(
            std::vector<ItemIdx> ns,
            std::vector<Weight> rs,
            std::vector<std::string> ts,
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
                if (ri >= (Cpt)rs.size()) {
                    ri = 0;
                    ti += 1;
                    if (ti >= (Cpt)ts.size()) {
                        ti = 0;
                        ni += 1;
                        if (ni >= (Cpt)ns.size())
                            return Instance(0, {});
                    }
                }
            }
        }

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
    Cpt h = 1;
    Seed s = -1;
    Cpt ni = 0;
    Cpt ri = 0;
    Cpt ti = 0;

};

void test(Instances& inss, std::vector<Output (*)(Instance&)> fs, TestType tt)
{
    for (;;) {
        Instance ins(inss.next());
        std::cout << ins << std::endl;
        bool b = test(ins, fs, tt);
        if (!b) {
            std::cout << "error" << std::endl;
            return;
        }
        if (ins.reduced_item_number() == 0)
            break;
    }
}

void knapsack::test(InstacesType it, std::vector<Output (*)(Instance&)> fs, TestType tt)
{
    if (it == TEST) {
        TestInstances ti;
        test(ti, fs, tt);
    } else if (it == SMALL) {
        SmallInstances si;
        test(si, fs, tt);
    } else if (it == MEDIUM) {
        PisingerInstances pi({50}, {100}, {"u", "wc", "sc", "isc", "asc", "ss"}, 5);
        test(pi, fs, tt);
    } else if (it == SC) {
        PisingerInstances pi({2, 5, 10, 20, 50, 100, 200, 500, 1000}, {10, 100, 1000}, {"sc"}, 5);
        test(pi, fs, tt);
    }
}


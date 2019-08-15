#include "knapsack/lib/tester.hpp"

#include "knapsack/lib/solution.hpp"
#include "knapsack/lib/generator.hpp"

using namespace knapsack;

bool test(const Instance& ins, std::vector<Profit (*)(Instance&)> fs, TestType tt)
{
    Profit opt = ins.optimum();
    for (auto f: fs) {
        Instance ins_tmp = ins;
        Profit val = f(ins_tmp);
        if (opt == -1)
            opt = val;
        if (tt == OPT) {
            EXPECT_EQ(val, opt);
            if (val != opt)
                return false;
        } else if (tt == UB) {
            EXPECT_GE(val, opt);
            if (val < opt)
                return false;
        } else if (tt == LB) {
            EXPECT_LE(val, opt);
            if (val > opt)
                return false;
        }
    }
    return true;
}

class Instances
{
public:
    virtual Instance next() = 0;
};

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
        Instance ins(0, 0);
        switch (i) {
        case 0: // 0 item
            return ins;
        case 1: // 1 item
            ins.set_capacity(8);
            ins.add_items({{6, 8}});
            return ins;
        case 2: // 2 items
            ins.set_capacity(10);
            ins.add_items({{9, 8}, {7, 6}});
            return ins;
        case 3:
            ins.set_capacity(10);
            ins.add_items({{9, 5}, {7, 6}});
            return ins;
        case 4:
            ins.set_capacity(10);
            ins.add_items({{5, 5}, {4, 6}});
            return ins;
        case 5:
            ins.set_capacity(12);
            ins.add_items({{5, 7}, {4, 5}, {3, 11}});
            return ins;
        case 6:
            ins.set_capacity(10);
            ins.add_items({{5, 7}, {4, 5}, {3, 11}});
            return ins;
        case 7:
            ins.set_capacity(10);
            ins.add_items({{1, 1}, {2, 2}, {9, 10}});
            return ins;
        case 8: // 4 items
            ins.set_capacity(95);
            ins.add_items({{24, 34}, {42, 52}, {32, 42}, {4, 14}});
            return ins;
        case 9: // 5 items
            ins.set_capacity(100);
            ins.add_items({{10, 10}, {10, 15}, {10, 5}, {10, 12}, {10, 20}});
            return ins;
        case 10:
            ins.set_capacity(15);
            ins.add_items({{6, 7}, {6, 7}, {5, 5}, {5, 5}, {5, 5}});
            return ins;
        case 11:
            ins.set_capacity(12);
            ins.add_items({{3, 3}, {2, 2}, {4, 4}, {10, 9}, {10, 8}});
            return ins;
        case 12: // 6 items
            ins.set_capacity(25);
            ins.add_items({{10, 100}, {6, 7}, {6, 7}, {5, 5}, {5, 5}, {5, 5}});
            return ins;
        case 13:
            ins.set_capacity(1884);
            ins.add_items({{829, 729}, {785, 685}, {710, 610}, {781, 681}, {128, 28}, {243, 143}});
            return ins;
        case 14: // 7 items
            ins.set_capacity(40);
            ins.add_items({{11, 12}, {11, 12}, {11, 12}, {10, 10}, {10, 10}, {10, 10}, {10, 10}});
            return ins;
        case 15:
            ins.set_capacity(9);
            ins.add_items({{2, 6}, {3, 5}, {6, 8}, {7, 9}, {5, 6}, {9, 7}, {4, 3}});
            return ins;
        default:
            return ins;
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
                        return Instance(0, 0);
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
                            return Instance(0, 0);
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

void test(Instances& inss, std::vector<Profit (*)(Instance&)> fs, TestType tt)
{
    for (;;) {
        Instance ins(inss.next());
        bool b = test(ins, fs, tt);
        if (!b) {
            std::cout << "error" << std::endl;
            return;
        }
        if (ins.item_number() == 0)
            break;
    }
    std::cout << "ok" << std::endl;
}

void knapsack::test(InstacesType it, std::vector<Profit (*)(Instance&)> fs, TestType tt)
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


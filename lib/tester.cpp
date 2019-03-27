#include "knapsack/lib/tester.hpp"

#include "knapsack/lib/solution.hpp"
#include "knapsack/lib/generator.hpp"

using namespace knapsack;

bool test(const Instance& ins, std::vector<Profit (*)(Instance&)> fs, TestType tt)
{
    std::cout << ins << std::endl;
    Profit opt = ins.optimum();
    for (auto f: fs) {
        std::cout << f << std::endl;
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
        std::cout << "i " << i << std::endl;
        return test_instance(i);
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

        GenerateData data;
        data.n = n;
        data.r = r;
        data.t = "u";
        data.h = h;
        data.s = s;

        std::cout << "data " << data << std::endl;
        return generate(data);
    }

private:

    ItemIdx n_max = 10;
    Weight r_max = 10;
    Cpt h_max = 100;
    Seed s_max = 10;
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

        GenerateData data;
        data.n = ns[ni];
        data.r = rs[ri];
        data.t = ts[ti];
        data.h = h;
        data.s = s;

        std::cout << "data " << data << std::endl;
        return generate(data);
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
    Instance ins(0, 0);
    do {
        ins = inss.next();
        bool b = test(ins, fs, tt);
        if (!b) {
            std::cout << "error" << std::endl;
            return;
        }
    } while (ins.item_number() != 0);
    std::cout << "end" << std::endl;
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


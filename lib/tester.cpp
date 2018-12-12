#include "knapsack/lib/tester.hpp"

#include "knapsack/lib/solution.hpp"
#include "knapsack/lib/generator.hpp"

#include <boost/coroutine2/all.hpp>

using namespace knapsack;

bool test(const Instance& ins, std::vector<Profit (*)(Instance&)> fs, TestType tt)
{
    std::cout << ins << std::endl;
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
    }
}


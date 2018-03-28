#include "bellman_list.hpp"

#include "../ub_dembo/dembo.hpp"

#define DBG(x)
//#define DBG(x) x

struct State
{
    Weight w;
    Profit p;
};

std::ostream& operator<<(std::ostream& os, State& s)
{
    os << "(" << s.w << "," << s.p << ")";
    return os;
}

std::ostream& operator<<(std::ostream& os, std::vector<State>& l)
{
    for (State& s: l)
        os << s << " ";
    return os;
}

Profit opt_bellman_list(const Instance& ins, Info* info)
{
    (void)info;
    Weight  c = ins.capacity();
    ItemPos n = ins.total_item_number();

    if (n == 0 || c == 0)
        return 0;

    Profit lb = 0;
    std::vector<State> l0{{0, 0}};
    for (ItemPos i=0; i<n; ++i) {
        DBG(std::cout << "I " << i << std::endl;)
        Weight wi = ins.item(i).w;
        Profit pi = ins.item(i).p;
        std::vector<State> l{{0, 0}};
        std::vector<State>::iterator it = l0.begin();
        std::vector<State>::iterator it1 = l0.begin();
        while (it != l0.end() || it1 != l0.end()) {
            if (it != l0.end() && it->w <= it1->w + wi) {
                DBG(std::cout << "STATE " << *it;)
                if (it->p > l.back().p) {
                    if (it->w == l.back().w) {
                        l.back() = *it;
                    } else {
                        l.push_back(*it);
                    }
                    DBG(std::cout << " OK" << std::endl;)
                } else {
                    DBG(std::cout << " X" << std::endl;)
                }
                ++it;
            } else {
                State s1{it1->w+wi, it1->p+pi};
                DBG(std::cout << "STATE " << *it1 << " => " << s1;)
                if (s1.w > c) {
                    DBG(std::cout << " W>C" << std::endl;)
                    break;
                }
                if (s1.p > l.back().p) {
                    if (s1.w == l.back().w) {
                        l.back() = s1;
                        DBG(std::cout << " OK" << std::endl;)
                    } else {
                        if (s1.p > lb) // Update lower bound
                            lb = s1.p;
                        Profit ub = ub_0(ins, i+1, s1.p, c-s1.w);
                        DBG(std::cout << " UB " << ub;)
                        if (ub > lb) {
                            l.push_back(s1);
                            DBG(std::cout << " OK" << std::endl;)
                        } else {
                            DBG(std::cout << " X" << std::endl;)
                        }
                    }
                } else {
                    DBG(std::cout << " X" << std::endl;)
                }
                it1++;
            }
        }
        l0 = std::move(l);
        DBG(std::cout << "L " << l0 << std::endl;)
    }

    assert(ins.check_opt(lb));
    return lb;
}

#undef DBG

/******************************************************************************/

Solution sopt_bellman_list_all(const Instance& ins, Info* info)
{
    (void)info;
    assert(false); // TODO
    return Solution(ins);
}

/******************************************************************************/

Solution sopt_bellman_list_one(const Instance& ins, Info* info)
{
    (void)info;
    assert(false); // TODO
    return Solution(ins);
}

/******************************************************************************/

Solution sopt_bellman_list_part(const Instance& ins, ItemPos k, Info* info)
{
    (void)info;
    (void)k;
    assert(false); // TODO
    return Solution(ins);
}

/******************************************************************************/

#define DBG(x)
//#define DBG(x) x

std::vector<State> opts_bellman_list(const Instance& ins,
        ItemPos n1, ItemPos n2, Weight c, Info* info = NULL)
{
    (void)info;

    DBG(std::cout << "OPTSBELLMANLIST N1 " << n1 << " N2 " << n2 << " c " << c << std::endl;)
    if (c == 0)
        return {{0, 0}};

    Profit lb = 0;
    std::vector<State> l0{{0, 0}};
    for (ItemPos i=n1; i<=n2; ++i) {
        DBG(std::cout << "I " << i << std::endl;)
        Weight wi = ins.item(i).w;
        Profit pi = ins.item(i).p;
        std::vector<State> l{{0, 0}};
        std::vector<State>::iterator it = l0.begin();
        std::vector<State>::iterator it1 = l0.begin();
        while (it != l0.end() || it1 != l0.end()) {
            if (it != l0.end() && it->w <= it1->w + wi) {
                DBG(std::cout << "STATE " << *it;)
                if (it->p > l.back().p) {
                    if (it->w == l.back().w) {
                        l.back() = *it;
                    } else {
                        l.push_back(*it);
                    }
                    DBG(std::cout << " OK" << std::endl;)
                } else {
                    DBG(std::cout << " X" << std::endl;)
                }
                ++it;
            } else {
                State s1{it1->w+wi, it1->p+pi};
                DBG(std::cout << "STATE " << *it1 << " => " << s1;)
                if (s1.w > c) {
                    DBG(std::cout << " W>C" << std::endl;)
                    break;
                }
                if (s1.p > l.back().p) {
                    if (s1.w == l.back().w) {
                        l.back() = s1;
                        DBG(std::cout << " OK" << std::endl;)
                    } else {
                        if (s1.p > lb) // Update lower bound
                            lb = s1.p;
                        Profit ub = ub_0(ins, i+1, s1.p, c-s1.w);
                        DBG(std::cout << " UB " << ub;)
                        if (ub >= lb) {
                            l.push_back(s1);
                            DBG(std::cout << " OK" << std::endl;)
                        } else {
                            DBG(std::cout << " X" << std::endl;)
                        }
                    }
                } else {
                    DBG(std::cout << " X" << std::endl;)
                }
                it1++;
            }
        }
        l0 = std::move(l);
        DBG(std::cout << "L " << l0 << std::endl;)
    }
    DBG(std::cout << "OPTSBELLMANLIST... END" << std::endl;)
    return l0;
}

void sopt_bellman_list_rec_rec(const Instance& ins,
        ItemPos n1, ItemPos n2, Weight c, Solution& sol_curr, Info* info)
{
    ItemPos k = (2*n1 + 8*n2) / 10;

    Weight w1_opt = 0;
    Weight w2_opt = 0;
    Profit p1_opt = 0;
    Profit p2_opt = 0;

    {
        std::vector<State> l1 = opts_bellman_list(ins, n1,  k,  c, info);
        std::vector<State> l2 = opts_bellman_list(ins, k+1, n2, c, info);

        Profit z_max  = 0;
        Weight i1_opt = 0;
        Weight i2_opt = 0;
        if (l1.size() > 0) {
            ItemPos i2 = l2.size()-1;
            for (Weight i1=0; i1<(Weight)l1.size(); ++i1) {
                while (l1[i1].w + l2[i2].w > c)
                    i2--;
                assert(i2 >= 0);
                Profit z = l1[i1].p + l2[i2].p;
                if (z > z_max) {
                    z_max = z;
                    i1_opt = i1;
                    i2_opt = i2;
                }
            }
        }
        if (l2.size() > 0) {
            ItemPos i1 = l1.size()-1;
            for (Weight i2=0; i2<(Weight)l2.size(); ++i2) {
                while (l2[i2].w + l1[i1].w > c)
                    i1--;
                Profit z = l1[i1].p + l2[i2].p;
                if (z > z_max) {
                    z_max = z;
                    i1_opt = i1;
                    i2_opt = i2;
                }
            }
        }

        w1_opt = l1[i1_opt].w;
        w2_opt = l2[i2_opt].w;
        p1_opt = l1[i1_opt].p;
        p2_opt = l2[i2_opt].p;
        DBG(std::cout << "Z " << z_max << std::endl;)
    }

    if (k == n1) {
        if (p1_opt == ins.item(n1).p)
            sol_curr.set(n1, true);
    } else {
        sopt_bellman_list_rec_rec(ins, n1, k, w1_opt, sol_curr, info);
    }

    if (k+1 == n2) {
        if (p2_opt == ins.item(n2).p)
            sol_curr.set(n2, true);
    } else {
        sopt_bellman_list_rec_rec(ins, k+1, n2, w2_opt, sol_curr, info);
    }
}

Solution sopt_bellman_list_rec(const Instance& ins, Info* info)
{
    ItemPos n = ins.item_number();

    if (n == 0)
        return Solution(ins);
    if (n == 1) {
        Solution solution(ins);
        solution.set(0, true);
        return solution;
    }

    Solution sol(ins);
    sopt_bellman_list_rec_rec(ins, 0, n-1, ins.capacity(), sol, info);
    assert(ins.check_sopt(sol));
    return sol;
}

#undef DBG

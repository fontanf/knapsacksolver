#include "bellman.hpp"

#include "../ub_dantzig/dantzig.hpp"
#include "../lb_greedy/greedy.hpp"

#include <map>
#include <queue>

#define INDEX(i,w) (i+1)*(c+1) + (w)

void opts_bellman(const Instance& instance, std::vector<Profit>& values,
        ItemPos n1, ItemPos n2, Weight c)
{
    for (Weight w=c+1; w-->0;)
        values[w] = 0;
    for (ItemPos i=n1; i<=n2; ++i) {
        Weight wi = instance.item(i).w;
        Profit pi = instance.item(i).p;
        for (Weight w=c+1; w-->0;)
            if (w >= wi && values[w-wi] + pi > values[w])
                values[w] = values[w-wi] + pi;
    }
}

Profit opt_bellman(const Instance& instance, Info* info)
{
    Weight  c = instance.capacity();
    ItemPos n = instance.item_number();
    std::vector<Profit> values(c+1);
    opts_bellman(instance, values, 0, n-1, c);
    assert(instance.check_opt(values[c]));
    return instance.reduced_solution()->profit() + values[c];
}

/******************************************************************************/

Solution sopt_bellman_1(const Instance& instance, Info* info)
{
    return sopt_bellman_1_it(instance, info);
}

/******************************************************************************/

Solution sopt_bellman_1_it(const Instance& instance, Info* info)
{
    // Initialize memory table
    ItemPos n = instance.item_number();
    Weight  c = instance.capacity();
    StateIdx values_size = (n+1)*(c+1);
    std::vector<Profit> values(values_size);

    // Compute optimal value
    for (Weight w=0; w<=instance.capacity(); ++w)
        values[INDEX(-1,w)] = 0;
    for (ItemPos i=0; i<instance.item_number(); ++i) {
        Weight wi = instance.item(i).w;
        for (Weight w=0; w<=instance.capacity(); ++w) {
            Profit v0 = values[INDEX(i-1,w)];
            Profit v1 = (w < wi)? 0: values[INDEX(i-1,w-wi)] + instance.item(i).p;
            values[INDEX(i,w)] = (v1 > v0)? v1: v0;
        }
    }
    Profit opt = values[values_size-1];

    // Retrieve optimal solution
    Solution solution = *instance.reduced_solution();
    ItemPos i = n-1;
    Weight  w = c;
    Profit  v = 0;
    while (v < opt) {
        Weight wi = instance.item(i).w;
        Profit pi = instance.item(i).p;
        Profit v0 = values[INDEX(i-1,w)];
        Profit v1 = (w < wi)? 0: values[INDEX(i-1,w-wi)] + pi;
        if (v1 > v0) {
            v += pi;
            w -= wi;
            solution.set(i, true);
        }
        i--;
    }
    assert(instance.check_sol(solution));
    return solution;
}

/******************************************************************************/

struct RecData
{
    RecData(const Instance& instance):
        instance(instance), i(instance.item_number()-1), w(instance.capacity())
    {
        ItemPos n = instance.item_number();
        Weight  c = instance.capacity();
        StateIdx values_size = (n+1)*(c+1);
        values = std::vector<Profit>(values_size, -1);
    }
    ~RecData() { }
    const Instance& instance;
    std::vector<Profit> values;
    ItemPos i;
    Weight w;
    size_t nodes = 0;
};

Profit sopt_bellman_1_rec_rec(RecData& d)
{
    Weight c = d.instance.capacity();

    if (d.values[INDEX(d.i,d.w)] != -1)
        return d.values[INDEX(d.i,d.w)];

    if (d.i == -1) {
        d.values[INDEX(d.i,d.w)] = 0;
        return 0;
    }

    d.i--;
    Profit v0 = sopt_bellman_1_rec_rec(d);
    Profit v1 = -1;
    if (d.w >= d.instance.item(d.i+1).w) {
        d.w -= d.instance.item(d.i+1).w;
        v1 = d.instance.item(d.i+1).p + sopt_bellman_1_rec_rec(d);
        d.w += d.instance.item(d.i+1).w;
    }
    d.i++;

    d.nodes++;

    if (v1 > v0) {
        d.values[INDEX(d.i,d.w)] = v1;
        return v1;
    } else {
        d.values[INDEX(d.i,d.w)] = v0;
        return v0;
    }
}

Solution sopt_bellman_1_rec(const Instance& instance, Info* info)
{
    ItemPos n = instance.item_number();
    Weight  c = instance.capacity();

    // Compute optimal value
    RecData data(instance);
    Profit opt = sopt_bellman_1_rec_rec(data);

    // Retrieve optimal solution
    Solution solution = *instance.reduced_solution();
    ItemPos i = n-1;
    Weight  w = c;
    Profit  v = 0;
    while (v < opt) {
        Weight wi = instance.item(i).w;
        Profit pi = instance.item(i).p;
        Profit v0 = data.values[INDEX(i-1,w)];
        Profit v1 = (w < wi)? 0: data.values[INDEX(i-1,w-wi)] + pi;
        if (v1 > v0) {
            v += pi;
            w -= wi;
            solution.set(i, true);
        }
        i--;
    }

    size_t nodes_max = (n + 1) * (c + 1);

    if (info != NULL) {
        info->pt.put("Solution.Values", data.nodes);
        info->pt.put("Solution.ValuesRatio",  (double)data.nodes / (double)nodes_max);
    }
    assert(instance.check_sol(solution));
    return solution;
}

/******************************************************************************/

struct Node
{
    ItemPos i;
    Weight  w;
    Profit  p;
    int state = 0;
    Node* child_0;
    Node* child_1;
    Node* parent;
};

Solution sopt_bellman_1_stack(const Instance& instance, Info* info)
{
    // Initialize memory table
    ItemPos n = instance.item_number();
    Weight  c = instance.capacity();
    StateIdx values_size = (n+1)*(c+1);
    std::vector<Profit> values(values_size, -1);

    std::stack<Node*> stack;
    size_t size_max = 0;
    size_t nodes    = 0;

    Node* node = new Node();
    node->i = instance.item_number()-1;
    node->w = instance.capacity();
    node->parent = NULL;
    stack.push(node);

    while (!stack.empty()) {
        if (stack.size() > size_max)
            size_max = stack.size();

        Node* node = stack.top();
        stack.pop();
        ItemPos i = node->i;
        Weight  w = node->w;
        if (node->state == 0) {
            if (values[INDEX(i,w)] != -1) {
                node->p = values[INDEX(i,w)];
                continue;
            }

            if (i == -1) {
                values[INDEX(i,w)] = 0;
                node->p      = 0;
                continue;
            }

            node->state = 1;
            stack.push(node);

            Node* n0 = new Node();
            node->child_0 = n0;
            n0->i = i-1;
            n0->w = w;
            n0->parent = node;
            stack.push(n0);

            if (w >= instance.item(i).w) {
                Node* n1 = new Node();
                node->child_1 = n1;
                n1->i = i-1;
                n1->w = w - instance.item(i).w;
                n1->parent = node;
                stack.push(n1);
            }
        } else if (node->state == 1) {
            Profit p0 = node->child_0->p;
            Profit p1 = (w < instance.item(i).w)? 0: node->child_1->p + instance.item(i).p;

            nodes++;
            values[INDEX(i,w)] = (p1 > p0)? p1: p0;
            node->p = values[INDEX(i,w)];
            delete node->child_0;
            if (w >= instance.item(i).w)
                delete node->child_1;
        }
    }
    delete node;

    Profit opt = values[values_size-1];

    // Retrieve optimal solution
    Solution solution = *instance.reduced_solution();
    ItemPos i = n-1;
    Weight  w = c;
    Profit  v = 0;
    while (v < opt) {
        Weight wi = instance.item(i).w;
        Profit pi = instance.item(i).p;
        Profit v0 = values[INDEX(i-1,w)];
        Profit v1 = (w < wi)? 0: values[INDEX(i-1,w-wi)] + pi;
        if (v1 > v0) {
            v += pi;
            w -= wi;
            solution.set(i, true);
        }
        i--;
    }

    size_t nodes_max = (n + 1) * (c + 1);

    if (info != NULL) {
        info->pt.put("Solution.StackMaxSize", size_max);
        info->pt.put("Solution.Values",       nodes);
        info->pt.put("Solution.ValuesRatio",  (double)nodes / (double)nodes_max);
    }
    assert(instance.check_sopt(solution));
    return solution;
}

/******************************************************************************/

struct MapData
{
    MapData(const Instance& instance):
        instance(instance), i(instance.item_number()-1), w(instance.capacity())
    {
        values = std::vector<std::map<Weight, Profit>>(
                instance.item_number()+1, std::map<Weight, Profit>());
    }
    const Instance& instance;
    std::vector<std::map<Weight, Profit>> values;
    ItemPos i;
    Weight w;
};

Profit sopt_bellman_1_map_rec(MapData& d)
{
    if (d.values[d.i+1].find(d.w) != d.values[d.i+1].end())
        return d.values[d.i+1][d.w];

    if (d.i == -1) {
        d.values[d.i+1][d.w] = 0;
        return 0;
    }

    d.i--;
    Profit v0 = sopt_bellman_1_map_rec(d);
    Profit v1 = -1;
    if (d.w >= d.instance.item(d.i+1).w) {
        d.w -= d.instance.item(d.i+1).w;
        v1 = d.instance.item(d.i+1).p + sopt_bellman_1_map_rec(d);
        d.w += d.instance.item(d.i+1).w;
    }
    d.i++;

    if (v1 > v0) {
        d.values[d.i+1][d.w] = v1;
        return v1;
    } else {
        d.values[d.i+1][d.w] = v0;
        return v0;
    }
}

Solution sopt_bellman_1_map(const Instance& instance, Info* info)
{
    // Compute optimal value
    MapData data(instance);
    Profit opt = sopt_bellman_1_map_rec(data);

    // Retrieve optimal solution
    Solution solution = *instance.reduced_solution();
    ItemPos i = instance.item_number()-1;
    Weight  w = instance.capacity();
    Profit  v = 0;
    while (v < opt) {
        Profit v0 = data.values[i][w];
        Profit v1 = (w < instance.item(i).w)? 0:
            data.values[i][w - instance.item(i).w] + instance.item(i).p;
        if (v1 > v0) {
            v += instance.item(i).p;
            w -= instance.item(i).w;
            solution.set(i, true);
        }
        i--;
    }

    size_t map_size = 0;
    for (ItemPos i=-1; i<instance.item_number(); ++i)
        map_size += data.values[i+1].size();
    size_t map_max_size = (instance.item_number() + 1) * (instance.capacity() + 1);
    if (info != NULL) {
        info->pt.put("Solution.MapSize", map_size);
        info->pt.put("Solution.MapRatio", (double)map_size / (double)map_max_size);
    }
    assert(instance.check_sopt(solution));
    return solution;
}

/******************************************************************************/

#define DBG(x)
//#define DBG(x) x

Solution sopt_bellman_2(const Instance& instance, Info* info)
{
    ItemPos n = instance.item_number();
    Weight  c = instance.capacity();
    Solution solution = *instance.reduced_solution();

    std::vector<Profit> values(c+1); // Initialize memory table
    ItemPos iter = 0;
    for (;;) {
        DBG(std::cout << n << " " << std::flush;)
        iter++;
        ItemPos last_item = -1;
        for (Weight w=c; w>=0; w--)
            values[w] = 0;
        for (ItemPos i=0; i<n; ++i) {
            Weight wi = instance.item(i).w;
            Profit pi = instance.item(i).p;
            if (c >= wi && values[c-wi] + pi > values[c]) {
                values[c] = values[c-wi] + pi;
                last_item = i;
            }
            for (Weight w=c; w>=0; w--)
                if (w >= wi && values[w-wi] + pi > values[w])
                    values[w] = values[w-wi] + pi;
        }

        if (last_item == -1)
            break;

        solution.set(last_item, true);
        c -= instance.item(last_item).w;
        n = last_item;
    }
    DBG(std::cout << std::endl;)

    if (info != NULL) {
        info->pt.put("Solution.Iterations", iter);
    }
    assert(instance.check_sopt(solution));
    return solution;
}

#undef DBG

/******************************************************************************/

#define DBG(x)
//#define DBG(x) x

struct RecData2
{
    RecData2(const Instance& instance):
        instance(instance), n1(0), n2(instance.item_number()-1),
        c(instance.capacity()), sol_curr(instance)
    {
        values1.resize(c+1);
        values2.resize(c+1);
    }
    const Instance& instance;
    ItemPos n1;
    ItemPos n2;
    Weight  c;
    Solution sol_curr;
    std::vector<Profit> values1;
    std::vector<Profit> values2;
};

void sopt_bellman_rec_rec(RecData2& d)
{
    DBG(std::cout << "Rec n1 " << d.n1 << " n2 " << d.n2 << " c " << d.c << std::endl;)
    DBG(std::cout << d.sol_curr << std::endl;)
    ItemPos k = (d.n1 + d.n2) / 2;
    DBG(std::cout << "k " << k << std::endl;)
    ItemPos n2 = d.n2;
    opts_bellman(d.instance, d.values1, d.n1, k, d.c);
    opts_bellman(d.instance, d.values2, k+1, d.n2, d.c);

    DBG(std::cout << "Find" << std::endl;)
    Profit z_max  = -1;
    Profit z2_opt = -1;
    Weight c1_opt = 0;
    Weight c2_opt = 0;
    for (Weight c1=0; c1<=d.c; ++c1) {
        Weight c2 = d.c - c1;
        Profit z = d.values1[c1] + d.values2[c2];
        if (z > z_max) {
            z_max = z;
            c1_opt = c1;
            c2_opt = c2;
            z2_opt = d.values2[c2];
        }
    }
    assert(z_max != -1);
    DBG(std::cout << "c1 " << c1_opt << " c2 " << c2_opt << std::endl;)

    DBG(std::cout << "Conquer" << std::endl;)
    if (k == d.n1) {
        DBG(std::cout << "Leaf" << std::endl;)
        if (d.values1[c1_opt] == d.instance.item(d.n1).p) {
            DBG(std::cout << "Set " << d.n1 << std::endl;)
            d.sol_curr.set(d.n1, true);
        }
    } else {
        DBG(std::cout << "..." << std::endl;)
        d.n2 = k;
        d.c  = c1_opt;
        sopt_bellman_rec_rec(d);
    }

    if (k+1 == n2) {
        DBG(std::cout << "Leaf" << std::endl;)
        if (z2_opt == d.instance.item(n2).p) {
            DBG(std::cout << "Set " << n2 << std::endl;)
            d.sol_curr.set(n2, true);
        }
    } else {
        DBG(std::cout << "..." << std::endl;)
        d.n1 = k+1;
        d.n2 = n2;
        d.c  = c2_opt;
        sopt_bellman_rec_rec(d);
    }
}

Solution sopt_bellman_rec(const Instance& instance, Info* info)
{
    if (instance.item_number() == 0)
        return *instance.reduced_solution();

    if (instance.item_number() == 1) {
        Solution solution = *instance.reduced_solution();
        solution.set(0, true);
        return solution;
    }

    RecData2 data(instance);
    sopt_bellman_rec_rec(data);
    DBG(std::cout << data.sol_curr << std::endl;)
    assert(instance.check_sopt(data.sol_curr));
    return data.sol_curr;
}

#undef DBG

/******************************************************************************/

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

void update_lower_bound(const Instance& instance,
        std::string lb_type, Profit& lb,
        const State& s, ItemPos i, ItemPos n1,
        Info* info)
{
    Profit lb_tmp = 0;
    if (lb_type == "none") {
        lb_tmp = s.p;
    } else if (lb_type == "greedy") {
        lb_tmp = lb_greedy_skip(instance, n1, i-1, s.p, instance.capacity() - s.w);
    } else {
        assert(false);
    }
    DBG(std::cout << " LBTMP " << lb_tmp << " LB " << lb;)
    if (lb_tmp > lb) {
        lb = lb_tmp;
        if (Info::verbose(info))
            std::cout << instance.print_lb(lb + instance.reduced_solution()->profit()) << std::endl;
    }
}

Profit upper_bound(const Instance& instance,
        std::string ub_type,
        const State& s, ItemPos i, ItemPos n1)
{
    Weight r = instance.capacity() - s.w;
    if (ub_type == "0") {
        return ub_trivial_from(instance, 0, s.p, r);
    } else if (ub_type == "trivial") {
        return (n1 == 0)?
            ub_trivial_from(instance, i, s.p, r):
            ub_trivial_from(instance, 0, s.p, r);
    } else if (ub_type == "dantzig") {
        return ub_dantzig_skip(instance, n1, i-1, s.p, instance.capacity() - s.w);
    } else {
        assert(false);
        return 0;
    }
}

/**
 * Return the list of undominated states.
 * opt == true iff lb == OPT - 1
 */
std::vector<State> opts_bellman_list(const Instance& instance,
        ItemPos n1, ItemPos n2, Weight c,
        Profit& lb, bool opt,
        std::string lb_type, std::string ub_type, Info* info)
{
    // Note that list L'i-1 is not explicitly created and that the
    // implementation requires Li-1 to be reversed (since pop_back is O(1)
    // whereas pop_front() is O(n) for std::vector).
    DBG(std::cout << "OPTSBELLMANLIST N1 " << n1 << " N2 " << n2 << " c " << c << std::endl;)
    if (c == 0)
        return {{0, 0}};

    std::vector<State> l0{{0, 0}};
    for (ItemPos i=n1; i<=n2; ++i) {
        DBG(std::cout << "I " << i << std::endl;)
        Weight wi = instance.item(i).w;
        Profit pi = instance.item(i).p;
        std::vector<State> l{{0, 0}};
        std::vector<State>::reverse_iterator it = l0.rbegin();
        while (!l0.empty()) {
            if (it != l0.rend() && it->w <= l0.back().w + wi) {
                DBG(std::cout << "STATE " << *it;)
                if (it->p > l.back().p) {
                    if (it->w == l.back().w) {
                        l.back() = *it;
                        DBG(std::cout << " OK" << std::endl;)
                    } else {
                        if (ub_type == "none") {
                            l.push_back(*it);
                        } else {
                            if (!opt)
                                update_lower_bound(instance, lb_type, lb, *it, i+1, n1, info);
                            Profit ub = upper_bound(instance, ub_type, *it, i+1, n1);
                            DBG(std::cout << " UB " << ub;)
                            if (lb <= ub) {
                                l.push_back(*it);
                                DBG(std::cout << " OK" << std::endl;)
                            } else {
                                DBG(std::cout << " X" << std::endl;)
                            }
                        }
                    }
                } else {
                    DBG(std::cout << " X" << std::endl;)
                }
                ++it;
            } else {
                State s1{l0.back().w+wi, l0.back().p+pi};
                DBG(std::cout << "STATE " << l0.back() << " => " << s1;)
                if (s1.w > c) {
                    DBG(std::cout << " W>C" << std::endl;)
                    break;
                }
                if (s1.p > l.back().p) {
                    if (s1.w == l.back().w) {
                        l.back() = s1;
                    } else {
                        if (ub_type == "none") {
                            l.push_back(s1);
                        } else {
                            if (!opt)
                                update_lower_bound(instance, lb_type, lb, s1, i+1, n1, info);
                            Profit ub = upper_bound(instance, ub_type, s1, i+1, n1);
                            DBG(std::cout << " UB " << ub;)
                            if (lb <= ub) {
                                l.push_back({l0.back().w + wi, l0.back().p + pi});
                                DBG(std::cout << " OK" << std::endl;)
                            } else {
                                DBG(std::cout << " X" << std::endl;)
                            }
                        }
                    }
                }
                l0.pop_back();
            }
        }
        std::reverse(l.begin(), l.end());
        l0 = std::move(l);
        DBG(std::cout << "L " << l0 << std::endl;)
    }
    DBG(std::cout << "OPTSBELLMANLIST... END" << std::endl;)
    return l0;
}

Profit opt_bellman_list(const Instance& instance, Profit lb, std::string lb_type, std::string ub_type, Info* info)
{
    Weight  c = instance.capacity();
    ItemPos n = instance.item_number();

    if (n == 0)
        return instance.reduced_solution()->profit();

    lb -= instance.reduced_solution()->profit();
    std::vector<State> l0 = opts_bellman_list(instance, 0, n-1, c, lb, false, lb_type, ub_type, info);
    Profit opt = (l0.size() > 0)? l0.front().p: lb;
    assert(instance.check_opt(instance.reduced_solution()->profit() + opt));
    return instance.reduced_solution()->profit() + opt;
}

#undef DBG

/******************************************************************************/

#define DBG(x)
//#define DBG(x) x

void sopt_bellman_rec_list_rec(const Instance& instance,
        ItemPos n1, ItemPos n2, Weight c, Profit lb, Solution& sol_curr,
        std::string lb_type, std::string ub_type, Info* info)
{
    ItemPos k = (2*n1 + 8*n2) / 10;

    Weight w1_opt = 0;
    Weight w2_opt = 0;
    Profit p1_opt = 0;
    Profit p2_opt = 0;

    {
        bool opt = (n1 != 0 || n2 != instance.item_number() - 1);
        std::vector<State> l1 = opts_bellman_list(instance, n1,  k,  c, lb, opt, lb_type, ub_type, info);
        std::vector<State> l2 = opts_bellman_list(instance, k+1, n2, c, lb, opt, lb_type, ub_type, info);

        Profit z_max  = 0;
        Weight i1_opt = 0;
        Weight i2_opt = 0;
        if (l1.size() > 0) {
            ItemPos i2 = 0;
            for (Weight i1=l1.size(); i1-->0;) {
                while (l1[i1].w + l2[i2].w > c) {
                    i2++;
                }
                assert(i2 < (Weight)l2.size());
                Profit z = l1[i1].p + l2[i2].p;
                if (z > z_max) {
                    z_max = z;
                    i1_opt = i1;
                    i2_opt = i2;
                }
            }
        }
        if (l2.size() > 0) {
            ItemPos i1 = 0;
            for (Weight i2=l2.size(); i2-->0;) {
                while (l2[i2].w + l1[i1].w > c)
                    i1++;
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
        if (p1_opt == instance.item(n1).p)
            sol_curr.set(n1, true);
    } else {
        sopt_bellman_rec_list_rec(instance,
                n1, k, w1_opt, p1_opt, sol_curr, lb_type, ub_type, info);
    }

    if (k+1 == n2) {
        if (p2_opt == instance.item(n2).p)
            sol_curr.set(n2, true);
    } else {
        sopt_bellman_rec_list_rec(instance,
                k+1, n2, w2_opt, p2_opt, sol_curr, lb_type, ub_type, info);
    }
}

Solution sopt_bellman_rec_list(const Instance& instance, const Solution& sol,
        std::string lb_type, std::string ub_type, Info* info)
{
    ItemPos n = instance.item_number();

    if (n == 0)
        return *instance.reduced_solution();
    if (n == 1) {
        Solution solution = *instance.reduced_solution();
        solution.set(0, true);
        return solution;
    }

    Solution sol_curr = *instance.reduced_solution();
    sopt_bellman_rec_list_rec(instance,
            0, n-1, instance.capacity(), 0, sol_curr,
            lb_type, ub_type, info);
    if (sol_curr.profit() > sol.profit()) {
        assert(instance.check_sopt(sol_curr));
        return sol_curr;
    } else {
        assert(instance.check_sopt(sol));
        return sol;
    }
}

#undef DBG

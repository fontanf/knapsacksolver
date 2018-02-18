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

Profit opt_bellman(const Instance& instance,
        boost::property_tree::ptree* pt, bool verbose)
{
    Weight  c = instance.capacity();
    ItemPos n = instance.item_number();
    std::vector<Profit> values(c+1);
    opts_bellman(instance, values, 0, n-1, c);
    assert(instance.check_opt(values[c]));
    return instance.reduced_solution()->profit() + values[c];
}

/******************************************************************************/

Solution sopt_bellman_1(const Instance& instance,
        boost::property_tree::ptree* pt, bool verbose)
{
    return sopt_bellman_1_it(instance, pt, verbose);
}

/******************************************************************************/

Solution sopt_bellman_1_it(const Instance& instance,
        boost::property_tree::ptree* pt, bool verbose)
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

Solution sopt_bellman_1_rec(const Instance& instance,
        boost::property_tree::ptree* pt, bool verbose)
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

    if (pt != NULL) {
        pt->put("Solution.Values", data.nodes);
        pt->put("Solution.ValuesRatio",  (double)data.nodes / (double)nodes_max);
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

Solution sopt_bellman_1_stack(const Instance& instance,
        boost::property_tree::ptree* pt, bool verbose)
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

    if (pt != NULL) {
        pt->put("Solution.StackMaxSize", size_max);
        pt->put("Solution.Values",       nodes);
        pt->put("Solution.ValuesRatio",  (double)nodes / (double)nodes_max);
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

Solution sopt_bellman_1_map(const Instance& instance,
        boost::property_tree::ptree* pt, bool verbose)
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
    if (pt != NULL) {
        pt->put("Solution.MapSize", map_size);
        pt->put("Solution.MapRatio", (double)map_size / (double)map_max_size);
    }
    assert(instance.check_sopt(solution));
    return solution;
}

/******************************************************************************/

#define DBG(x)
//#define DBG(x) x

Solution sopt_bellman_2(const Instance& instance,
        boost::property_tree::ptree* pt, bool verbose)
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

    if (pt != NULL) {
        pt->put("Solution.Iterations", iter);
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

Solution sopt_bellman_rec(const Instance& instance,
        boost::property_tree::ptree* pt, bool verbose)
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

std::vector<State> opts_bellman_list(const Instance& instance,
        ItemPos n1, ItemPos n2, Weight c)
{
    // Note that list L'i-1 is not explicitly created and that the
    // implementation requires Li-1 to be reversed (since pop_back is O(1)
    // whereas pop_front() is O(n) for std::vector).
    std::vector<State> l0{{0, 0}};
    for (ItemPos i=n1; i<=n2; ++i) {
        Weight wi = instance.item(i).w;
        Profit pi = instance.item(i).p;
        std::vector<State> l{{0, 0}};
        std::vector<State>::reverse_iterator it = l0.rbegin();
        while (!l0.empty()) {
            if (it != l0.rend() && it->w <= l0.back().w + wi) {
                if (it->p > l.back().p) {
                    if (it->w == l.back().w) {
                        l.back() = *it;
                    } else {
                        l.push_back(*it);
                    }
                }
                ++it;
            } else {
                if (l0.back().w + wi > c)
                    break;
                if (l0.back().p + pi > l.back().p) {
                    if (l0.back().w + wi == l.back().w) {
                        l.back() = {l0.back().w + wi, l0.back().p + pi};
                    } else {
                        l.push_back({l0.back().w + wi, l0.back().p + pi});
                    }
                }
                l0.pop_back();
            }
        }
        DBG(for (State& s: l)
                std::cout << "(" << s.w << "," << s.p << ") " << std::flush;
                std::cout << std::endl;)
            std::reverse(l.begin(), l.end());
        l0 = std::move(l);
    }
    return l0;
}

Profit opt_bellman_list(const Instance& instance,
        boost::property_tree::ptree* pt, bool verbose)
{
    Weight  c = instance.capacity();
    ItemPos n = instance.item_number();

    if (n == 0)
        return instance.reduced_solution()->profit();

    std::vector<State> l0 = opts_bellman_list(instance, 0, n-1, c);
    Profit opt = instance.reduced_solution()->profit() + l0.front().p;

    if (pt != NULL) {
        pt->put("Solution.States", l0.size());
        pt->put("Solution.StateRatio", (double)l0.size() / (double)instance.capacity());
    }
    if (verbose) {
        std::cout << "States "     << l0.size() << std::endl;
        std::cout << "StateRatio " << (double)l0.size() / (double)instance.capacity() << std::endl;
    }
    assert(instance.check_opt(opt));
    return opt;
}

#undef DBG

/******************************************************************************/

#define DBG(x)
//#define DBG(x) x

struct RecListData
{
    RecListData(const Instance& instance):
        instance(instance), n1(0), n2(instance.item_number()-1),
        c(instance.capacity()), sol_curr(instance) { }
    const Instance& instance;
    ItemPos n1;
    ItemPos n2;
    Weight  c;
    Solution sol_curr;
};

void sopt_bellman_rec_list_rec(RecListData& d)
{
    DBG(std::cout << "Rec n1 " << d.n1 << " n2 " << d.n2 << " c " << d.c << std::endl;)
    DBG(std::cout << d.sol_curr << std::endl;)
    ItemPos k = (d.n1 + d.n2) / 2;
    DBG(std::cout << "k " << k << std::endl;)
    ItemPos n2 = d.n2;

    Weight w1_opt = 0;
    Weight w2_opt = 0;
    Profit p1_opt = 0;
    Profit p2_opt = 0;

    {
        std::vector<State> l1 = opts_bellman_list(d.instance, d.n1, k, d.c);
        std::vector<State> l2 = opts_bellman_list(d.instance, k+1, d.n2, d.c);

        DBG(std::cout << "Find" << std::endl;)
        Profit z_max  = 0;
        Weight i1_opt = 0;
        Weight i2_opt = 0;
        if (l1.size() > 0) {
            ItemPos i2 = 0;
            for (Weight i1=l1.size(); i1-->0;) {
                DBG(std::cout << "i1 " << i1 << " " << std::flush;)
                DBG(std::cout << "i2 " << std::flush;)
                while (l1[i1].w + l2[i2].w > d.c) {
                    DBG(std::cout << i2 << " " << std::flush;)
                    i2++;
                }
                assert(i2 < (Weight)l2.size());
                DBG(std::cout << i2 << " " << std::flush;)
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
                while (l2[i2].w + l1[i1].w > d.c)
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
    }

    DBG(std::cout << "c1 " << w1_opt << " c2 " << w2_opt << std::endl;)

    DBG(std::cout << "Conquer" << std::endl;)
    if (k == d.n1) {
        DBG(std::cout << "Leaf" << std::endl;)
        if (p1_opt == d.instance.item(d.n1).p) {
            DBG(std::cout << "Set " << d.n1 << std::endl;)
            d.sol_curr.set(d.n1, true);
        }
    } else {
        DBG(std::cout << "..." << std::endl;)
        d.n2 = k;
        d.c  = w1_opt;
        sopt_bellman_rec_list_rec(d);
    }

    if (k+1 == n2) {
        DBG(std::cout << "Leaf" << std::endl;)
        if (p2_opt == d.instance.item(n2).p) {
            DBG(std::cout << "Set " << n2 << std::endl;)
            d.sol_curr.set(n2, true);
        }
    } else {
        DBG(std::cout << "..." << std::endl;)
        d.n1 = k+1;
        d.n2 = n2;
        d.c  = w2_opt;
        sopt_bellman_rec_list_rec(d);
    }
}

Solution sopt_bellman_rec_list(const Instance& instance,
        boost::property_tree::ptree* pt, bool verbose)
{
    if (instance.item_number() == 0)
        return *instance.reduced_solution();

    if (instance.item_number() == 1) {
        Solution solution = *instance.reduced_solution();
        solution.set(0, true);
        return solution;
    }

    RecListData data(instance);
    sopt_bellman_rec_list_rec(data);
    DBG(std::cout << data.sol_curr << std::endl;)
    assert(instance.check_sopt(data.sol_curr));
    return data.sol_curr;
}

#undef DBG

/******************************************************************************/

#define DBG(x)
//#define DBG(x) x

/**
 * Return the list of undominated states.
 * opt == true iff lb is indeed the optimal value
 */
std::vector<State> opts_bellman_ub(const Instance& instance,
        ItemPos n1, ItemPos n2, ItemPos first, ItemPos last,
        Weight c, Profit &lb, bool opt, bool verbose)
{
    DBG(std::cout << "opts_bellman_ub n1 " << n1 << " n2 " << n2 << " c " << c << std::endl;)
    if (c == 0)
        return {{0, 0}};
    std::vector<State> l0{{0, 0}};
    for (ItemPos i=n1; i<=n2; ++i) {
        Weight wi = instance.item(i).w;
        Profit pi = instance.item(i).p;
        std::vector<State> l{{0, 0}};
        std::vector<State>::reverse_iterator it = l0.rbegin();
        while (!l0.empty()) {
            if (it != l0.rend() && it->w <= l0.back().w + wi) {
                if (it->p > l.back().p) {
                    if (it->w == l.back().w) {
                        l.back() = *it;
                    } else {
                        if (!opt) {
                            Profit lb_tmp = it->p + lb_greedy_except(
                                    instance, first, n1, i, last, c-it->w);
                            if (lb_tmp > lb) {
                                lb = lb_tmp;
                                if (verbose)
                                    std::cout
                                        << "LB " << lb
                                        << " GAP " << instance.optimum() - lb
                                        << std::endl;
                            }
                        }
                        if (it->p + ub_dantzig_except(
                                    instance, first, n1, i, last, c-it->w) >= lb)
                            l.push_back(*it);
                    }
                }
                ++it;
            } else {
                if (l0.back().w + wi > c)
                    break;
                if (l0.back().p + pi > l.back().p) {
                    if (l0.back().w + wi == l.back().w) {
                        l.back() = {l0.back().w + wi, l0.back().p + pi};
                    } else {
                        if (!opt) {
                            Profit lb_tmp = l0.back().p + pi + lb_greedy_except(
                                    instance, first, n1, i, last, c-l0.back().w-wi);
                            if (lb_tmp > lb) {
                                lb = lb_tmp;
                                if (verbose)
                                    std::cout
                                        << "LB " << lb
                                        << " GAP " << instance.optimum() - lb
                                        << std::endl;
                            }
                        }
                        if (l0.back().p + pi + ub_dantzig_except(
                                    instance, first, n1, i, last, c-l0.back().w-wi) >= lb)
                            l.push_back({l0.back().w + wi, l0.back().p + pi});
                    }
                }
                l0.pop_back();
            }
        }
        std::reverse(l.begin(), l.end());
        l0 = std::move(l);
    }
    return l0;
}

Profit opt_bellman_ub(const Instance& instance,
        boost::property_tree::ptree* pt, bool verbose)
{
    Weight  c = instance.capacity();
    ItemPos n = instance.item_number();

    if (n == 0)
        return instance.reduced_solution()->profit();

    Profit lb = sol_bestgreedy(instance).profit() - 1;
    std::vector<State> l0 = opts_bellman_ub(instance, 0, n-1, 0, n-1, c, lb, false, verbose);
    assert(instance.check_opt(instance.reduced_solution()->profit() + l0.front().p));
    return instance.reduced_solution()->profit() + l0.front().p;
}

#undef DBG

/******************************************************************************/

#define DBG(x)
//#define DBG(x) x

void sopt_bellman_rec_ub_rec(const Instance& instance,
        ItemPos n1, ItemPos n2, Weight c, Profit lb, Solution& sol_curr,
        bool verbose)
{
    ItemPos k = (2*n1 + 8*n2) / 10;

    Weight w1_opt = 0;
    Weight w2_opt = 0;
    Profit p1_opt = 0;
    Profit p2_opt = 0;

    {
        bool opt = (lb != -1);
        if (!opt)
            lb = lb_greedy_from_to(instance, n1, n2, c);
        std::vector<State> l1 = opts_bellman_ub(instance, n1,  k,  n1, n2, c, lb, opt, verbose);
        std::vector<State> l2 = opts_bellman_ub(instance, k+1, n2, n1, n2, c, lb, opt, verbose);

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
        assert(!opt || z_max == lb);
    }

    if (k == n1) {
        if (p1_opt == instance.item(n1).p)
            sol_curr.set(n1, true);
    } else {
        sopt_bellman_rec_ub_rec(instance, n1, k, w1_opt, p1_opt, sol_curr, verbose);
    }

    if (k+1 == n2) {
        if (p2_opt == instance.item(n2).p)
            sol_curr.set(n2, true);
    } else {
        sopt_bellman_rec_ub_rec(instance, k+1, n2, w2_opt, p2_opt, sol_curr, verbose);
    }
}

Solution sopt_bellman_rec_ub(const Instance& instance,
        boost::property_tree::ptree* pt, bool verbose)
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
    sopt_bellman_rec_ub_rec(instance,
            0, n-1, instance.capacity(), -1, sol_curr, verbose);
    assert(instance.check_sopt(sol_curr));
    return sol_curr;
}

#undef DBG

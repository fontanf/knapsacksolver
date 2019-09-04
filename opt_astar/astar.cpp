#include "knapsack/opt_astar/astar.hpp"

#include "knapsack/lb_greedy/greedy.hpp"
#include "knapsack/lb_greedynlogn/greedynlogn.hpp"
#include "knapsack/ub_dembo/dembo.hpp"

#include <queue>
#include <set>

using namespace knapsack;

struct Node
{
    Solution sol;
    ItemPos j;
    Profit ub;
};

std::ostream& operator<<(std::ostream& os, const Node& n)
{
    os << "{id " << n.sol.to_string_items() 
       << " w " << n.sol.weight()
       << " p " << n.sol.profit()
       << " u " << n.ub
       << "}"
       << std::flush;
    return os;
}

class Compare
{
public:
    bool operator()(const Node& n1, const Node& n2)
    {
        return n1.ub <= n2.ub;
    }
};

Solution knapsack::sopt_astar(Instance& ins, Info info)
{
    VER(info, "*** babstar ***" << std::endl);

    ItemIdx n = ins.item_number();
    Solution sol(ins);
    if (n == 0) {
        LOG(info, "Empty instance.");
        return algorithm_end(sol, info);
    }

    ItemPos j_max = ins.max_efficiency_item(info);
    std::vector<Weight> min_weight = ins.min_weights();

    std::priority_queue<Node, std::vector<Node>, Compare> q;
    Cpt q_max_size = 0;
    Cpt q_average_size = 0;
    Cpt node_number = 0;

    Solution sol_best(ins);

    Node n0 = {Solution(ins), 0, 1};
    q.push(n0);
    while (!q.empty()) {
        // Update infos
        node_number++;
        if ((StateIdx)q.size() > q_max_size)
            q_max_size = q.size();
        q_average_size += q.size();

        // Debug traces
        LOG(info, std::endl
                << "node number" << node_number
                << " pbest " << sol_best.profit() << std::endl
                << "queue size " << q.size() << std::endl);

        // Get node
        Node node = q.top();
        q.pop();
        LOG(info, "node " << node << " r " << node.sol.remaining_capacity() << std::endl);

        // Update best solution
        if (node.sol.profit() > sol_best.profit())
            sol_best = node.sol;

        // Stop condition
        if (node.ub <= sol_best.profit())
            break;

        // Try to add item k for k = j+1..n-1
        for (ItemPos k=node.j; k<n; ++k) {
            LOG(info, "k " << k << " " << ins.item(k));

            // Compare remaining capacity to minimum weight of all the
            // remaining items
            if (node.sol.remaining_capacity() < min_weight[k]) {
                LOG(info, " no remaining item can fit" << std::endl);
                break;
            }

            // Check if item k fits in remaining capacity
            if (node.sol.remaining_capacity() < ins.item(k).w) {
                LOG(info, " item cannot fit" << std::endl);
                continue;
            }

            Solution sol = node.sol;
            sol.set(k, true);

            Profit ub = ub_0(ins, k+1, sol.profit(), sol.remaining_capacity(), j_max);
            if (ub < sol_best.profit()) {
                LOG(info, " ub is too small" << std::endl);
                continue;
            } else {
                LOG(info, std::endl);
            }

            // Create new node and add it to the queue
            Node n{sol, k+1, ub};
            LOG(info, "add node " << n << std::endl);
            q.push(n);
        }

    }

    PUT(info, "Algorithm", "NodeNumber", node_number);
    PUT(info, "Algorithm", "QueueMaxSize", q_max_size);
    PUT(info, "Algorithm", "QueueAverageSize", q_average_size / node_number);
    return algorithm_end(sol_best, info);
}

/*****************************************************************************/

struct NodeDP;
class NodeDPCompare
{
public:
    bool operator()(NodeDP* n1, NodeDP* n2);
};

struct NodeDP
{
    Weight w;
    Profit p;
    ItemPos j;
    Profit ub;
    NodeDP* father;
    NodeDP* brother = NULL;
    NodeDP* child = NULL;

    std::string id()
    {
        if (father != NULL) {
            return father->id() + "," + std::to_string(j);
        } else {
            return "-1";
        }
    }

    std::string to_string()
    {
        std::string s =
            "{id " + id() +
            " w " + std::to_string(w) +
            " p " + std::to_string(p) +
            " ub " + std::to_string(ub) +
            " f ";
        s += (father != NULL)? father->id(): "NULL";
        s += " b ";
        s += (brother != NULL)? brother->id(): "NULL";
        s += " c ";
        s += (child != NULL)? child->id() + "}": "NULL}";
        return s;
    }

    std::string to_string_all()
    {
        std::string s = to_string();
        if (child != NULL)
            s += "\n" + child->to_string_all();
        if (brother != NULL)
            s += "\n" + brother->to_string_all();
        return s;
    }

    ~NodeDP() { }

    void free(std::set<NodeDP*, NodeDPCompare>& q, Info& info)
    {
        if (child != NULL) {
            child->free(q, info);
            delete child;
        }
        if (brother != NULL) {
            brother->free(q, info);
            delete brother;
        }
        q.erase(this);
    }

    void cut_child(std::set<NodeDP*, NodeDPCompare>& q, Info& info)
    {
        NodeDP* child_old = child;
        child = child->brother;
        child_old->brother = NULL;
        child_old->free(q, info);
        delete child_old;
    }

    void cut_brother(std::set<NodeDP*, NodeDPCompare>& q, Info& info)
    {
        NodeDP* brother_old = brother;
        brother = brother->brother;
        brother_old->brother = NULL;
        brother_old->free(q, info);
        delete brother_old;
    }

    bool cut(Weight ww, Profit pp, ItemPos jj, std::set<NodeDP*, NodeDPCompare>& q, Info& info)
    {
        NodeDP* c = child;
        NodeDP* c_prec = this;
        while (c != NULL) {
            if (c->w <= ww && c->p >= pp && c->j <= jj) {
                return true;
            }
            if (ww < c->w && pp > c->p && jj < c->j) {
                if (c == child) {
                    cut_child(q, info);
                } else {
                    c_prec->cut_brother(q, info);
                }
            } else {
                if (c->cut(ww, pp, jj, q, info))
                    return true;
                c_prec = c;
            }
            c = c_prec->brother;
        }
        return false;
    }

};

bool NodeDPCompare::operator()(NodeDP* n1, NodeDP* n2)
{
    if (n1->ub != n2->ub)
        return n1->ub > n2->ub;
    return n1 < n2;
}

Solution knapsack::sopt_astar_dp(Instance& ins, Info info)
{
    VER(info, "*** babstar with dp ***" << std::endl);
    ItemIdx n = ins.item_number();
    Solution sol(ins);

    if (n == 0) {
        return algorithm_end(sol, info);
    }

    // Compute min weight table
    ItemPos j_max = ins.max_efficiency_item(info);
    std::vector<Weight> min_weight = ins.min_weights();

    std::set<NodeDP*, NodeDPCompare> q;
    StateIdx q_max_size = 0;
    StateIdx q_average_size = 0;
    StateIdx node_number = 0;

    Solution sol_best(ins);

    NodeDP n0;
    n0.w = 0;
    n0.p = 0;
    n0.j = -1;
    n0.ub = ub_0(ins, 0, 0, ins.total_capacity(), j_max);
    n0.father = NULL;
    q.insert(&n0);
    while (!q.empty()) {
        // Update infos
        if ((StateIdx)q.size() > q_max_size)
            q_max_size = q.size();
        q_average_size += q.size();
        node_number++;

        // Get node
        NodeDP* node = *q.begin();
        q.erase(q.begin());
        Weight r = ins.total_capacity() - node->w; // remaining capacity

        // Update best solution
        if (node->p > sol_best.profit()) {
            Solution sol(ins);
            NodeDP* node_tmp = node;
            while (node_tmp->j != -1) {
                sol.set(node_tmp->j, true);
                node_tmp = node_tmp->father;
            }
            sol_best = sol;
        }

        // Stop condition
        if (node->ub <= sol_best.profit())
            break;

        // Try to add item k for k = j+1..n-1
        for (ItemPos k=node->j+1; k<n; ++k) {
            // Compare remaining capacity to minimum weight of all the
            // remaining items
            if (r < min_weight[k]) {
                break;
            }

            // Check if item k fits in remaining capacity
            if (r < ins.item(k).w) {
                continue;
            }

            Weight w = node->w + ins.item(k).w;
            Profit p = node->p + ins.item(k).p;
            Profit ub = ub_0(ins, k+1, p, ins.total_capacity() - w, j_max);
            if (ub <= sol_best.profit()) {
                continue;
            } else {
            }

            // Is the new node dominated by another already seen node?
            // Does the new node dominate other already seen nodes?
            if (n0.cut(w, p, k, q, info))
                continue;

            // Create new node and add it to the queue
            NodeDP* n = new NodeDP();
            n->w  = w;
            n->p  = p;
            n->j  = k;
            n->ub = ub;
            n->father = node;
            if (node->child == NULL) {
                node->child = n;
            } else {
                NodeDP* tmp = node->child;
                while (tmp->brother != NULL)
                    tmp = tmp->brother;
                tmp->brother = n;
            }

            q.insert(n);
        }

    }

    n0.cut_child(q, info);

    PUT(info, "Algorithm", "NodeNumber", node_number);
    PUT(info, "Algorithm", "QueueMaxSize", q_max_size);
    PUT(info, "Algorithm", "QueueAverageSize", q_average_size / node_number);
    return algorithm_end(sol_best, info);
}

/******************************************************************************/

Solution knapsack::sopt_starknap(Instance& ins, StarknapParams params, Info info)
{
    VER(info, "*** starknap ***" << std::endl);
    Solution sol(ins);

    if (params.upper_bound == "b") {
        ins.sort_partially(info);
    } else if (params.upper_bound == "t") {
        ins.sort(info);
    } else {
        assert(false);
    }
    if (ins.break_item() == ins.last_item()+1) { // all items are in the break solution
        Solution sol = *ins.break_solution();
        return algorithm_end(sol, info);
    }

    if (params.lb_greedynlogn == 0) {
        sol = sol_greedynlogn(ins);
    } else if (params.lb_greedy == 0) {
        sol = sol_greedy(ins);
    } else {
        sol = *ins.break_solution();
    }

    // Variable reduction
    if (params.upper_bound == "b") {
        ins.reduce1(sol.profit(), info);
    } else if (params.upper_bound == "t") {
        ins.reduce2(sol.profit(), info);
    } else {
        assert(false);
    }
    if (ins.capacity() < 0) {
        // If the capacity is negative, then it means that sol was the optimal
        // solution. Note that this is not possible if opt-1 has been used as
        // lower bound for the reduction.
        return algorithm_end(sol, info);
    }

    Weight  c = ins.total_capacity();
    ItemPos f = ins.first_item();
    ItemPos n = ins.item_number();

    // Trivial cases
    if (n == 0 || c == 0) {
        if (ins.reduced_solution()->profit() > sol.profit())
            sol = *ins.reduced_solution();
        return algorithm_end(sol, info);
    } else if (n == 1) {
        Solution sol1 = *ins.reduced_solution();
        sol1.set(f, true);
        if (sol1.profit() > sol.profit())
            sol = sol1;
        return algorithm_end(sol, info);
    } else if (ins.break_item() == ins.last_item()+1) {
        if (ins.break_solution()->profit() > sol.profit())
            sol = *ins.break_solution();
        return algorithm_end(sol, info);
    }

    ItemPos b = ins.break_item();


    std::set<NodeDP*, NodeDPCompare> q;
    StateIdx q_max_size = 0;
    StateIdx q_average_size = 0;
    StateIdx node_number = 0;

    // idx[j] is the position of the jth item considered when branching.
    std::vector<ItemPos> idx;
    std::vector<ItemPos> sidx {b-1};
    std::vector<ItemPos> tidx {b+1};
    ItemPos s = b-1;
    ItemPos t = b;
    bool x = true;
    for (ItemPos j=0; j<n; ++j) {
        if ((x && t <= ins.last_item()) ||
          (!x && s < ins.first_item()) ) {
            idx.push_back(t);
            t++;
            tidx.push_back(t);
            sidx.push_back(s);
            x = false;
        } else {
            idx.push_back(s);
            s--;
            tidx.push_back(t);
            sidx.push_back(s);
            x = true;
        }
    }

    NodeDP n0;
    n0.w = ins.break_solution()->weight();
    n0.p = ins.break_solution()->profit();
    n0.j = -1;
    n0.ub = ub_dembo(ins, b, n0.p, c-n0.w);
    n0.father = NULL;
    q.insert(&n0);
    while (!q.empty()) {
        // Update infos
        if ((StateIdx)q.size() > q_max_size)
            q_max_size = q.size();
        q_average_size += q.size();
        node_number++;

        // Get node
        NodeDP* node = *q.begin();
        q.erase(q.begin());

        // Update best solution
        if (node->w <= c && node->p > sol.profit()) {
            Solution sol_tmp = *ins.break_solution();
            NodeDP* node_tmp = node;
            while (node_tmp->j != -1) {
                sol_tmp.set(idx[node_tmp->j], (idx[node_tmp->j] >= b));
                node_tmp = node_tmp->father;
            }
            sol = sol_tmp;
        }

        // Stop condition
        if (node->ub <= sol.profit())
            break;

        // Try to add item k for k = j+1..n-1
        for (ItemPos k=node->j+1; k<n; ++k) {
            ItemPos j_next = idx[k];

            Weight x = (j_next >= b)? 1: -1;
            Weight w = node->w + x*ins.item(j_next).w;
            Profit p = node->p + x*ins.item(j_next).p;
            Profit ub = 0;
            if (params.upper_bound == "b") {
                ub = (w <= c)?
                    ub_dembo(ins, b, p, c-w):
                    ub_dembo_rev(ins, b, p, c-w);
            } else if (params.upper_bound == "t") {
                ub = (w <= c)?
                    ub_dembo(ins, tidx[k], p, c-w):
                    ub_dembo_rev(ins, sidx[k], p, c-w);
            } else {
                    assert(false);
            }
            if (ub <= sol.profit()) {
                continue;
            } else {
            }

            // Is the new node dominated by another already seen node?
            // Does the new node dominate other already seen nodes?
            if (n0.cut(w, p, k, q, info))
                continue;

            // Create new node and add it to the queue
            NodeDP* n = new NodeDP();
            n->w  = w;
            n->p  = p;
            n->j  = k;
            n->ub = ub;
            n->father = node;
            if (node->child == NULL) {
                node->child = n;
            } else {
                NodeDP* tmp = node->child;
                while (tmp->brother != NULL)
                    tmp = tmp->brother;
                tmp->brother = n;
            }

            q.insert(n);
        }

    }

    LOG(info, "End of the search" << std::endl);

    if (n0.child != NULL)
        n0.cut_child(q, info);

    PUT(info, "Algorithm", "NodeNumber", node_number);
    PUT(info, "Algorithm", "QueueMaxSize", q_max_size);
    PUT(info, "Algorithm", "QueueAverageSize", q_average_size / node_number);
    return algorithm_end(sol, info);
}


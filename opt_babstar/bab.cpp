#include "knapsack/opt_babstar/bab.hpp"

#include "knapsack/ub_dembo/dembo.hpp"

#include <queue>

using namespace knapsack;

struct Node
{
    Solution sol;
    ItemPos j;
    Profit ub;
    std::string to_string()
    {
        return
            "{id " + sol.print_in() +
            " w " + std::to_string(sol.weight()) +
            " p " + std::to_string(sol.profit()) +
            " ub " + std::to_string(ub);
    }
};

class Compare
{
public:
    bool operator()(const Node& n1, const Node& n2)
    {
        return n1.ub <= n2.ub;
    }
};

Solution knapsack::sopt_babstar(Instance& ins, Info& info)
{
    info.verbose("*** babstar ***\n");

    ItemIdx n = ins.item_number();
    Solution sol(ins);
    if (n == 0) {
        info.debug("Empty instance.");
        return algorithm_end(sol, info);
    }

    // Compute min weight table
    std::vector<Weight> min_weight(n);
    min_weight[n-1] = ins.item(n-1).w;
    for (ItemIdx i=n-2; i>=0; --i)
        min_weight[i] = std::min(ins.item(i).w, min_weight[i+1]);
    if (info.debug()) {
        info.debug("min weights:");
        for (ItemIdx i=0; i<n; ++i)
            info.debug(" " + std::to_string(min_weight[i]));
        info.debug("\n");
    }

    std::priority_queue<Node, std::vector<Node>, Compare> q;
    StateIdx q_max_size = 0;
    StateIdx q_average_size = 0;
    StateIdx node_number = 0;

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
        if (info.debug()) {
            info.debug("\n");
            info.debug("Node number " + std::to_string(node_number) +
                " pbest " + std::to_string(sol_best.profit()) + "\n");
            info.debug("Queue size " + std::to_string(q.size()) + "\n");
        }

        // Get node
        Node node = q.top();
        q.pop();
        info.debug("Node " + node.to_string() + " r " + std::to_string(node.sol.remaining_capacity()) + "\n");

        // Update best solution
        if (sol_best.update(node.sol))
            info.debug("Update best solution\n");

        // Stop condition
        if (node.ub <= sol_best.profit())
            break;

        // Try to add item k for k = j+1..n-1
        for (ItemPos k=node.j; k<n; ++k) {
            info.debug(ins.item(k).to_string());

            // Compare remaining capacity to minimum weight of all the
            // remaining items
            if (node.sol.remaining_capacity() < min_weight[k]) {
                info.debug(" no remaining item can fit\n");
                break;
            }

            // Check if item k fits in remaining capacity
            if (node.sol.remaining_capacity() < ins.item(k).w) {
                info.debug( " item cannot fit\n");
                continue;
            }

            Solution sol = node.sol;
            sol.set(k, true);

            Profit ub = ub_0(ins, k+1, sol.profit(), sol.remaining_capacity());
            if (ub < sol_best.profit()) {
                info.debug( " ub is too small\n");
                continue;
            } else {
                info.debug("\n");
            }

            // Create new node and add it to the queue
            Node n{sol, k+1, ub};
            info.debug("Add node " + n.to_string() + "\n");
            q.push(n);
        }

    }

    info.verbose(
        "Node number: " + std::to_string(node_number) + "\n" +
        "Queue max size: " + std::to_string(q_max_size) + "\n" +
        "Queue average size: " + std::to_string(q_average_size / node_number) + "\n");
    info.pt.put("Algorithm.NodeNumber", node_number);
    info.pt.put("Algorithm.QueueMaxSize", q_max_size);
    info.pt.put("Algorithm.QueueAverageSize", q_average_size / node_number);

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
                info.debug("NODE IS DOMINATED BY NODE " + c->to_string() + "\n");
                return true;
            }
            if (ww < c->w && pp > c->p && jj < c->j) {
                info.debug("CUT NODE " + c->to_string() + "\n");
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

Solution knapsack::sopt_babstar_dp(Instance& ins, Info& info)
{
    info.verbose("*** babstar with dp ***\n");
    ItemIdx n = ins.item_number();
    Solution sol(ins);

    if (n == 0) {
        info.debug("Empty instance.\n");
        return algorithm_end(sol, info);
    }

    // Compute min weight table
    std::vector<Weight> min_weight(n);
    min_weight[n-1] = ins.item(n-1).w;
    for (ItemIdx i=n-2; i>=0; --i)
        min_weight[i] = std::min(ins.item(i).w, min_weight[i+1]);
    if (info.debug()) {
        info.debug("min weights: ");
        for (ItemIdx i=0; i<n; ++i)
            info.debug(" " + std::to_string(min_weight[i]));
        info.debug("\n");
    }

    std::set<NodeDP*, NodeDPCompare> q;
    StateIdx q_max_size = 0;
    StateIdx q_average_size = 0;
    StateIdx node_number = 0;

    Solution sol_best(ins);

    NodeDP n0;
    n0.w = 0;
    n0.p = 0;
    n0.j = -1;
    n0.ub = ub_0(ins, 0, 0, ins.capacity());
    n0.father = NULL;
    q.insert(&n0);
    while (!q.empty()) {
        // Update infos
        if ((StateIdx)q.size() > q_max_size)
            q_max_size = q.size();
        q_average_size += q.size();
        node_number++;

        // Debug traces
        if (info.debug()) {
            info.debug("\n");
            info.debug("Node number " + std::to_string(node_number) +
                " pbest " + std::to_string(sol_best.profit()) + "\n");
            info.debug("Tree\n" + n0.to_string_all() + "\n");
            info.debug("Queue size " + std::to_string(q.size()) + " - q:\n");
            for (NodeDP* n: q)
                info.debug(n->to_string() + "\n");
        }

        // Get node
        NodeDP* node = *q.begin();
        q.erase(q.begin());
        Weight r = ins.capacity() - node->w; // remaining capacity
        info.debug("Node " + node->to_string() + " r " + std::to_string(r) + "\n");

        // Update best solution
        if (node->p > sol_best.profit()) {
            info.debug("Update best solution\n");
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
            info.debug(ins.item(k).to_string());

            // Compare remaining capacity to minimum weight of all the
            // remaining items
            if (r < min_weight[k]) {
                info.debug(" no remaining item can fit\n");
                break;
            }

            // Check if item k fits in remaining capacity
            if (r < ins.item(k).w) {
                info.debug( " item cannot fit\n");
                continue;
            }

            Weight w = node->w + ins.item(k).w;
            Profit p = node->p + ins.item(k).p;
            Profit ub = ub_0(ins, k+1, p, ins.capacity() - w);
            info.debug(" w " + std::to_string(w) + " p " + std::to_string(p) + " ub " + std::to_string(ub));
            if (ub <= sol_best.profit()) {
                info.debug( " ub is too small\n");
                continue;
            } else {
                info.debug("\n");
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

            info.debug("Add node " + n->to_string() + "\n");
            q.insert(n);
        }

    }

    info.debug("End of the search.\n");

    n0.cut_child(q, info);

    info.verbose(
        "Node number: " + std::to_string(node_number) + "\n" +
        "Queue max size: " + std::to_string(q_max_size) + "\n" +
        "Queue average size: " + std::to_string(q_average_size / node_number) + "\n");
    info.pt.put("Algorithm.NodeNumber", node_number);
    info.pt.put("Algorithm.QueueMaxSize", q_max_size);
    info.pt.put("Algorithm.QueueAverageSize", q_average_size / node_number);

    assert(ins.check_sopt(sol_best));
    return algorithm_end(sol_best, info);
}

#undef DBG

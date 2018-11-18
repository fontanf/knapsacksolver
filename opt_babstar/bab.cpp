#include "knapsack/opt_babstar/bab.hpp"

#include "knapsack/ub_dembo/dembo.hpp"

#include <queue>

using namespace knapsack;

struct Node
{
    Solution sol;
    ItemPos j;
    Profit ub;
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
    ItemIdx n = ins.item_number();
    Solution sol_curr(ins);
    if (n == 0)
        return algorithm_end(sol_curr, info);

    // Compute min weight table
    std::vector<Weight> min_weight(n);
    min_weight[n-1] = ins.item(n-1).w;
    for (ItemIdx i=n-2; i>=0; --i)
        min_weight[i] = std::min(ins.item(i).w, min_weight[i+1]);

    std::priority_queue<Node, std::vector<Node>, Compare> q;
    StateIdx q_max_size = 0;
    StateIdx q_average_size = 0;

    Solution sol_best(ins);
    Node n0 = {Solution(ins), 0, 1};
    q.push(n0);
    StateIdx node_number = 0;
    while (!q.empty()) {
        // Update infos
        node_number++;
        if ((StateIdx)q.size() > q_max_size)
            q_max_size = q.size();
        q_average_size += q.size();

        // Get node
        Node node = q.top();
        q.pop();

        // Update best solution
        sol_best.update(node.sol);

        // Stop condition
        if (node.ub <= sol_best.profit())
            break;

        for (ItemPos k=node.j; k<n; ++k) {
            if (node.sol.remaining_capacity() < min_weight[k])
                break;
            if (node.sol.remaining_capacity() < ins.item(k).w)
                continue;
            Solution sol = node.sol;
            sol.set(k, true);
            Profit ub = ub_0(ins, k+1, sol.profit(), sol.remaining_capacity());
            if (ub < sol_best.profit())
                continue;
            q.push({sol, k+1, ub});
        }

    }

    if (info.verbose()) {
        std::cout << "NODE NUMBER " << node_number << std::endl;
        std::cout << "QUEUE MAX SIZE " << q_max_size << std::endl;
        std::cout << "QUEUE AVERAGE SIZE " << q_average_size / node_number << std::endl;
    }
    info.pt.put("Algorithm.NodeNumber", node_number);
    info.pt.put("Algorithm.QueueMaxSize", q_max_size);
    info.pt.put("Algorithm.QueueAverageSize", q_average_size / node_number);

    assert(ins.check_sopt(sol_best));
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
            "{ID " + id() +
            " W " + std::to_string(w) +
            " P " + std::to_string(p) +
            " UB " + std::to_string(ub) +
            " F ";
        s += (father != NULL)? father->id(): "NULL";
        s += " B ";
        s += (brother != NULL)? brother->id(): "NULL";
        s += " C ";
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
    ItemIdx n = ins.item_number();
    Solution sol_curr(ins);

    if (n == 0) {
        info.debug("INSTANCE CONTAINS NO ITEMS\n");
        return algorithm_end(sol_curr, info);
    }

    // Compute min weight table
    std::vector<Weight> min_weight(n);
    min_weight[n-1] = ins.item(n-1).w;
    for (ItemIdx i=n-2; i>=0; --i)
        min_weight[i] = std::min(ins.item(i).w, min_weight[i+1]);

    if (info.debug()) {
        info.debug("MINWEIGHTS ");
        for (ItemIdx i=0; i<n; ++i)
            info.debug(std::to_string(min_weight[i]) + " ");
        info.debug("\n");
    }

    std::set<NodeDP*, NodeDPCompare> q;
    StateIdx q_max_size = 0;
    StateIdx q_average_size = 0;

    Solution sol_best(ins);

    NodeDP n0;
    n0.w = 0;
    n0.p = 0;
    n0.j = -1;
    n0.ub = ub_0(ins, 0, 0, ins.capacity());
    n0.father = NULL;

    q.insert(&n0);
    StateIdx node_number = 0;
    while (!q.empty()) {
        // Update infos
        if ((StateIdx)q.size() > q_max_size)
            q_max_size = q.size();
        q_average_size += q.size();
        node_number++;

        // Debug traces
        if (info.debug()) {
            info.debug("\n");
            info.debug("NODENUMBER " + std::to_string(node_number) +
                " PBEST " + std::to_string(sol_best.profit()) + "\n");
            info.debug("TREE\n" + n0.to_string_all() + "\n");
            info.debug("QSIZE " + std::to_string(q.size()) + " Q\n");
            for (NodeDP* n: q)
                info.debug(n->to_string() + "\n");
        }

        // Get node
        NodeDP* node = *q.begin();
        q.erase(q.begin());
        Weight r = ins.capacity() - node->w; // remaining capacity
        info.debug("NODE " + node->to_string() + " R " + std::to_string(r) + "\n");

        // Update best solution
        if (node->p > sol_best.profit()) {
            info.debug("UPDATE BEST SOLUTION\n");
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
            info.debug(
                    "K " + std::to_string(k) +
                    " WK " + std::to_string(ins.item(k).w) +
                    " PK " + std::to_string(ins.item(k).p));

            // Compare remaining capacity to minimum weight of all the
            // remaining items
            if (r < min_weight[k]) {
                info.debug(" NO REMAINING ITEM CAN FIT\n");
                break;
            }

            // Check if item k fits in remaining capacity
            if (r < ins.item(k).w) {
                info.debug( " ITEM CANNOT FIT\n");
                continue;
            }

            Weight w = node->w + ins.item(k).w;
            Profit p = node->p + ins.item(k).p;
            Profit ub = ub_0(ins, k+1, p, ins.capacity() - w);
            info.debug(" W " + std::to_string(w) + " P " + std::to_string(p) + " UB " + std::to_string(ub));
            if (ub <= sol_best.profit()) {
                info.debug( " UB is too small\n");
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

            info.debug("ADD NODE " + n->to_string() + "\n");
            q.insert(n);
        }

    }

    info.debug("END OF THE SEARCH\n");

    n0.cut_child(q, info);

    if (info.verbose()) {
        std::cout << "NODE NUMBER " << node_number << std::endl;
        std::cout << "QUEUE MAX SIZE " << q_max_size << std::endl;
        std::cout << "QUEUE AVERAGE SIZE " << q_average_size / node_number << std::endl;
    }
    info.pt.put("Algorithm.NodeNumber", node_number);
    info.pt.put("Algorithm.QueueMaxSize", q_max_size);
    info.pt.put("Algorithm.QueueAverageSize", q_average_size / node_number);

    assert(ins.check_sopt(sol_best));
    return algorithm_end(sol_best, info);
}

#undef DBG

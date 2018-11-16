#include "knapsack/opt_babstar/bab.hpp"

#include "knapsack/ub_dembo/dembo.hpp"

#include <queue>

using namespace knapsack;

#define DBG(x)
//#define DBG(x) x

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
        return sol_curr;

    // Compute min weight table
    std::vector<Weight> min_weight(n);
    min_weight[n-1] = ins.item(n-1).w;
    for (ItemIdx i=n-2; i>=0; --i)
        min_weight[i] = std::min(ins.item(i).w, min_weight[i+1]);

    std::priority_queue<Node, std::vector<Node>, Compare> q;
    Solution sol_best(ins);
    Node n0 = {Solution(ins), 0, 1};
    q.push(n0);
    StateIdx node_number = 0;
    while (!q.empty()) {
        node_number++;
        Node node = q.top();
        //std::cout << node_number << " " << q.size() << " " << node.sol.print_bin() << " " << node.j << " " << node.ub << " " << sol_best.profit() << std::endl;
        q.pop();
        sol_best.update(node.sol); // Update best solution
        if (node.ub <= sol_best.profit())
            break;

        for (ItemPos k=node.j; k<n; ++k) {
            if (node.sol.remaining_capacity() < min_weight[k])
                break;
            if (node.sol.remaining_capacity() >= ins.item(k).w) {
                Solution sol = node.sol;
                sol.set(k, true);
                Profit ub = ub_0(ins, k+1, sol.profit(), sol.remaining_capacity());
                q.push({sol, k+1, ub});
            }
        }

    }

    if (info.verbose)
        //std::cout << "NODES " << std::scientific << (double)node_number << std::endl;
        std::cout << "NODES " << node_number << std::endl;

    assert(ins.check_sopt(sol_best));
    return sol_best;
}

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
            return father->id() + std::to_string(j);
        } else {
            return "";
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
        if (brother != NULL)
            s += "\n" + brother->to_string();
        if (child != NULL)
            s += "\n" + child->to_string();
        return s;
    }

    ~NodeDP() { }

    void free(std::set<NodeDP*, NodeDPCompare> q)
    {
        if (child != NULL) {
            child->free(q);
            q.erase(child);
            delete child;
        }
        if (brother != NULL) {
            brother->free(q);
            q.erase(brother);
            delete brother;
        }
    }

    void cut(std::set<NodeDP*, NodeDPCompare> q)
    {
        if (child != NULL)
            child->free(q);
        q.erase(this);
        delete this;
    }

    bool cut(Weight ww, Profit pp, ItemPos jj, std::set<NodeDP*, NodeDPCompare> q, Info& info)
    {
        if (w <= ww && p >= pp && j <= jj) {
            info.debug("NODE IS DOMINATED BY NODE " + to_string() + "\n");
            return true;
        }
        if (ww < w && pp > p && jj < j) {
            info.debug("CUT NODE " + to_string() + "\n");
            cut(q);
        }
        if (child != NULL)
            if (child->cut(ww, pp, jj, q, info))
                return true;
        if (brother != NULL)
            if (brother->cut(ww, pp, jj, q, info))
                return true;
        return false;
    }
};

bool NodeDPCompare::operator()(NodeDP* n1, NodeDP* n2)
{
    return n1->ub <= n2->ub;
}

Solution knapsack::sopt_babstar_dp(Instance& ins, Info& info)
{
    ItemIdx n = ins.item_number();
    Solution sol_curr(ins);

    if (n == 0) {
        info.debug("INSTANCE CONTAINS NO ITEMS\n");
        return sol_curr;
    }

    // Compute min weight table
    std::vector<Weight> min_weight(n);
    min_weight[n-1] = ins.item(n-1).w;
    for (ItemIdx i=n-2; i>=0; --i)
        min_weight[i] = std::min(ins.item(i).w, min_weight[i+1]);

    if (info.dbg) {
        info.debug("MINWEIGHTS ");
        for (ItemIdx i=0; i<n; ++i)
            info.debug(std::to_string(min_weight[i]) + " ");
        info.debug("\n");
    }

    std::set<NodeDP*, NodeDPCompare> q;

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
        // Get node
        NodeDP* node = *q.begin();
        q.erase(q.begin());
        node_number++;
        Weight r = ins.capacity() - node->w; // remaining capacity

        // Debug traces
        if (info.dbg) {
            info.debug("TREE\n" + n0.to_string_all() + "\n");
            info.debug("QSIZE " + std::to_string(q.size()) + " Q\n");
            for (NodeDP* n: q)
                info.debug(n->to_string() + "\n");
            info.debug(
                "NODENUMBER " + std::to_string(node_number) +
                " NODE " + node->to_string() +
                " R " + std::to_string(r) +
                " PBEST " + std::to_string(sol_best.profit()) + "\n");
        }

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
        bool first = true;
        NodeDP* n_prec = NULL;
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
                info.debug( " ITEM NOT CAN FIT\n");
                continue;
            }

            Weight w = node->w + ins.item(k).w;
            Profit p = node->p + ins.item(k).p;
            info.debug(" W " + std::to_string(w) + " P " + std::to_string(p) + "\n");

            // Is the new node dominated by another already seen node?
            // Does the new node dominate other already seen nodes?
            if (n0.cut(w, p, k, q, info))
                continue;

            // Create new node and add it to the queue
            NodeDP* n = new NodeDP();
            n->w = w;
            n->p = p;
            n->j = k;
            n->ub = ub_0(ins, k+1, n->p, ins.capacity() - n->w);
            n->father = node;
            if (first) {
                first = false;
                node->child = n;
            } else {
                n_prec->brother = n;
            }
            n_prec = n;

            info.debug("ADD NODE " + n->to_string() + "\n");
            q.insert(n);
        }

    }

    info.debug("END OF THE SEARCH\n");

    n0.child->cut(q);

    if (info.verbose)
        std::cout << "NODES " << node_number << std::endl;

    assert(ins.check_sopt(sol_best));
    return sol_best;
}

#undef DBG

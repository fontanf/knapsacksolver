#include "knapsack/lib/solution.hpp"

#include "knapsack/lib/part_solution_1.hpp"
#include "knapsack/lib/part_solution_2.hpp"

using namespace knapsack;

Solution::Solution(const Instance& instance): instance_(instance),
    x_(std::vector<int>(instance.total_item_number(), 0)) { }

Solution::Solution(const Solution& solution):
    instance_(solution.instance()), k_(solution.item_number()),
    p_(solution.profit()), w_(solution.weight()), x_(solution.data()) { }

Solution& Solution::operator=(const Solution& solution)
{
    if (this != &solution) {
        if (&this->instance() == &solution.instance()) {
            k_ = solution.item_number();
            p_ = solution.profit();
            w_ = solution.weight();
            x_ = solution.data();
        } else {
            // Used to convert a solution of a surrogate instance to a
            // solution of its original instance.
            p_ = 0;
            w_ = 0;
            k_ = solution.item_number();
            x_ = solution.data();
            for (ItemPos j=0; j<instance().total_item_number(); ++j) {
                if (contains(j)) {
                    p_ += instance().item(j).p;
                    w_ += instance().item(j).w;
                }
            }
        }
    }
    return *this;
}

int Solution::contains(ItemPos j) const
{
    assert(j >= 0 && j < instance().total_item_number());
    assert(instance().item(j).j >= 0 && instance().item(j).j < instance().total_item_number());
    return x_[instance().item(j).j];
}

int Solution::contains_idx(ItemIdx j) const
{
    assert(j >= 0 && j < instance().total_item_number());
    return x_[j];
}

void Solution::set(ItemPos j, int b)
{
    assert(b == 0 || b == 1);
    assert(j >= 0 && j < instance().total_item_number());
    assert(instance().item(j).j >= 0 && instance().item(j).j < instance().total_item_number());
    if (contains(j) == b)
        return;
    if (b) {
        p_ += instance().item(j).p;
        w_ += instance().item(j).w;
        k_++;
    } else {
        p_ -= instance().item(j).p;
        w_ -= instance().item(j).w;
        k_--;
    }
    x_[instance().item(j).j] = b;
}

void Solution::clear()
{
    k_ = 0;
    p_ = 0;
    w_ = 0;
    std::fill(x_.begin(), x_.end(), 0);
}

bool Solution::update(const Solution& sol)
{
    if (sol.profit() <= profit() || sol.remaining_capacity() < 0)
        return false;
    *this = sol;
    return true;
}

void Solution::update_from_partsol(const PartSolFactory1& bsolf, PartSol1 bsol)
{
    for (ItemPos j=bsolf.x1(); j<=bsolf.x2(); ++j)
        set(j, bsolf.contains(bsol, j));
}

void Solution::update_from_partsol(const PartSolFactory2& psolf, PartSol2 psol)
{
    for (ItemPos j=0; j<psolf.size(); ++j)
        set(psolf.indices()[j], psolf.contains(psol, j));
}

void Solution::write_cert(std::string file)
{
    if (file != "") {
        std::ofstream cert;
        cert.open(file);
        cert << *this;
        cert.close();
    }
}

std::ostream& knapsack::operator<<(std::ostream& os, const Solution& solution)
{
    const Instance& instance = solution.instance();
    for (ItemPos j=0; j<instance.total_item_number(); ++j)
        os << solution.data()[j] << std::endl;
    return os;
}

std::string Solution::print_bin() const
{
    std::string s = "";
    for (ItemPos j=0; j<instance().total_item_number(); ++j)
        s += std::to_string(contains(j));
    return s;
}

std::string Solution::print_in() const
{
    std::string s = "";
    for (ItemPos j=0; j<instance().total_item_number(); ++j)
        if (contains(j))
            s += std::to_string(j);
    return s;
}


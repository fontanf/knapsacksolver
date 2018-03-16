#include "solution.hpp"

#include "binary_solution.hpp"

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
            for (ItemPos i=0; i<instance().total_item_number(); ++i) {
                if (contains(i)) {
                    p_ += instance().item(i).p;
                    w_ += instance().item(i).w;
                }
            }
        }
    }
    return *this;
}

int Solution::contains(ItemPos i) const
{
    assert(i >= 0 && i < instance().total_item_number());
    assert(instance().item(i).i >= 0 && instance().item(i).i < instance().total_item_number());
    return x_[instance().item(i).i];
}

int Solution::contains_idx(ItemIdx i) const
{
    assert(i >= 0 && i < instance().total_item_number());
    return x_[i];
}

void Solution::set(ItemPos i, int b)
{
    assert(b == 0 || b == 1);
    assert(i >= 0 && i < instance().total_item_number());
    assert(instance().item(i).i >= 0 && instance().item(i).i < instance().total_item_number());
    if (contains(i) == b)
        return;
    if (b) {
        p_ += instance().item(i).p;
        w_ += instance().item(i).w;
        k_++;
    } else {
        p_ -= instance().item(i).p;
        w_ -= instance().item(i).w;
        k_--;
    }
    x_[instance().item(i).i] = b;
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
    if (sol.profit() <= profit())
        return false;
    *this = sol;
    return true;
}

void Solution::update_from_binary(const BSolFactory& bsolf, BSol bsol)
{
    for (ItemPos i=bsolf.x1(); i<=bsolf.x2(); ++i)
        set(i, bsolf.contains(bsol, i));
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

std::ostream& operator<<(std::ostream& os, const Solution& solution)
{
    const Instance& instance = solution.instance();
    for (ItemPos i=0; i<instance.total_item_number(); ++i)
        os << solution.data()[i] << std::endl;
    return os;
}

std::string Solution::print_bin() const
{
    std::string s = "";
    for (ItemPos i=0; i<instance().total_item_number(); ++i)
        s += std::to_string(contains(i));
    return s;

}

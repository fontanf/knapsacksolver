#include "knapsacksolver/sort.hpp"

#include "knapsacksolver/upper_bound.hpp"

using namespace knapsacksolver;

FullSort::FullSort(const Instance& instance):
    instance_(&instance),
    break_solution_(instance),
    sorted_items_(instance.number_of_items(), 0)
{
    if (instance.total_item_weight() <= instance.capacity()) {
        throw std::invalid_argument("");
    }

    // Sort items.
    std::iota(sorted_items_.begin(), sorted_items_.end(), 0);
    std::sort(
            sorted_items_.begin(),
            sorted_items_.end(),
            [&instance](ItemId item_id_1, ItemId item_id_2) {
                const Item& item_1 = instance.item(item_id_1);
                const Item& item_2 = instance.item(item_id_2);
                return item_1.profit * item_2.weight > item_2.profit * item_1.weight;
            });

    // Compute break solution.
    for (ItemPos item_pos = 0;
            item_pos < instance.number_of_items();
            ++item_pos) {
        ItemId item_id = sorted_items_[item_pos];
        const Item& item = instance.item(item_id);
        if (break_solution_.weight() + item.weight > instance.capacity()) {
            break_item_pos_ = item_pos;
            break_item_id_ = item_id;
            break;
        }
        break_solution_.add(item_id);
    }
}

PartialSort::PartialSort(
        const Instance& instance):
    instance_(&instance),
    break_solution_(instance),
    mandatory_items_(instance),
    sorted_items_(instance.number_of_items(), 0)
{
    if (instance.total_item_weight() <= instance.capacity()) {
        throw std::invalid_argument("");
    }

    if (std::numeric_limits<Profit>::max() / instance.highest_item_weight()
            <= instance.highest_item_profit()) {
        throw std::invalid_argument("Too high profits and weights.");
    }

    // Initialize sorted_items_.
    std::iota(sorted_items_.begin(), sorted_items_.end(), 0);

    // Quick sort like algorithm.
    ItemPos f = 0;
    ItemPos l = instance.number_of_items() -1;
    Weight capacity_cur = instance.capacity();
    while (f < l) {

        std::pair<ItemPos, ItemPos> fl = partition(f, l);
        Weight weight = 0;
        for (ItemPos item_pos = f; item_pos < fl.first; ++item_pos) {
            ItemId item_id = sorted_items_[item_pos];
            weight += instance.item(item_id).weight;
        }

        if (weight > capacity_cur) {
            if (fl.second + 1 <= l) {
                intervals_right_.push_back({fl.second + 1, l});
            }
            intervals_right_.push_back({fl.first, fl.second});
            l = fl.first - 1;
            continue;
        }

        for (ItemPos item_pos = fl.first; item_pos <= fl.second; ++item_pos) {
            ItemId item_id = sorted_items_[item_pos];
            weight += instance.item(item_id).weight;
        }

        if (weight > capacity_cur) {
            break;
        } else {
            capacity_cur -= weight;
            if (f <= fl.first - 1) {
                intervals_left_.push_back({f, fl.first - 1});
            }
            intervals_left_.push_back({fl.first, fl.second});
            f = fl.second + 1;
        }
    }

    // Compute break item.
    compute_break_solution();

    if (f <= break_item_pos_ - 1)
        intervals_left_.push_back({f, break_item_pos_ - 1});
    if (l >= break_item_pos_ + 1)
        intervals_right_.push_back({break_item_pos_ + 1, l});
    first_sorted_item_pos_ = break_item_pos_;
    last_sorted_item_pos_ = break_item_pos_;
    initial_core_last_item_pos_ = break_item_pos_;
    initial_core_first_item_pos_ = break_item_pos_;

    check();
}

void PartialSort::compute_break_solution()
{
    for (ItemPos item_pos = 0;
            item_pos < instance().number_of_items();
            ++item_pos) {
        ItemId item_id = sorted_items_[item_pos];
        const Item& item = instance().item(item_id);
        if (break_solution_.weight() + item.weight > instance().capacity()) {
            break_item_pos_ = item_pos;
            break_item_id_ = item_id;
            break;
        }
        break_solution_.add(item_id);
    }
}

std::pair<ItemPos, ItemPos> PartialSort::partition(
        ItemPos f,
        ItemPos l)
{
    // Select pivot.
    ItemPos pivot_item_pos = f + 1 + (l - f) / 2;
    ItemId pivot_item_id = sorted_items_[pivot_item_pos];
    const Item& pivot_item = instance().item(pivot_item_id);

    // Partition
    std::swap(sorted_items_[pivot_item_pos], sorted_items_[l]);
    ItemPos item_pos = f;
    while (item_pos <= l) {
        ItemId item_id = sorted_items_[item_pos];
        const Item& item = instance().item(item_id);
        if (item.profit * pivot_item.weight > pivot_item.profit * item.weight) {
            std::swap(sorted_items_[item_pos], sorted_items_[f]);
            f++;
            item_pos++;
        } else if (item.profit * pivot_item.weight < pivot_item.profit * item.weight) {
            std::swap(sorted_items_[item_pos], sorted_items_[l]);
            l--;
        } else {
            item_pos++;
        }
    }

    // | | | | | | | | | | | | | | | | | | | | |
    //          f       j           l
    // -------|                       |-------
    // > pivot                         < pivot

    return {f, l};
}

void PartialSort::sort_next_left_interval(
        Profit lower_bound)
{
    Interval interval = intervals_left_.back();
    intervals_left_.pop_back();
    ItemPos k = first_sorted_item_pos_;
    for (ItemPos item_pos = interval.last;
            item_pos >= interval.first;
            --item_pos) {
        ItemId item_id = sorted_items_[item_pos];
        const Item& item = instance().item(item_id);
        Profit profit = break_solution().profit() - item.profit;
        Weight weight = break_solution().weight() - item.weight;
        Profit upper_bound_curr = upper_bound(
                instance(),
                profit,
                weight,
                break_item_id());
        if (upper_bound_curr > lower_bound) {
            k--;
            std::swap(sorted_items_[k], sorted_items_[item_pos]);
        } else {
            mandatory_items_.add(item_id);
        }
    }
    std::sort(
            sorted_items_.begin() + k,
            sorted_items_.begin() + first_sorted_item_pos_,
            [this](ItemId item_id_1, ItemId item_id_2) {
                const Item& item_1 = instance().item(item_id_1);
                const Item& item_2 = instance().item(item_id_2);
                return item_1.profit * item_2.weight > item_2.profit * item_1.weight;
            });
    first_sorted_item_pos_ = k;
}

void PartialSort::sort_next_right_interval(
        Profit lower_bound)
{
    Interval interval = intervals_right_.back();
    intervals_right_.pop_back();
    ItemPos k = last_sorted_item_pos_;
    const Item& break_item = instance().item(break_item_id());
    for (ItemPos item_pos = interval.first;
            item_pos <= interval.last;
            ++item_pos) {
        ItemId item_id = sorted_items_[item_pos];
        const Item& item = instance().item(item_id);
        Profit profit = break_solution().profit() + break_item.profit + item.profit;
        Weight weight = break_solution().weight() + break_item.weight + item.weight;
        Profit upper_bound_curr = upper_bound_reverse(
                instance(),
                profit,
                weight,
                break_item_id());
        if (upper_bound_curr > lower_bound) {
            k++;
            std::swap(sorted_items_[k], sorted_items_[item_pos]);
        } else {
        }
    }
    std::sort(
            sorted_items_.begin() + last_sorted_item_pos_ + 1,
            sorted_items_.begin() + k + 1,
            [this](ItemId item_id_1, ItemId item_id_2) {
                const Item& item_1 = instance().item(item_id_1);
                const Item& item_2 = instance().item(item_id_2);
                return item_1.profit * item_2.weight > item_2.profit * item_1.weight;
            });
    last_sorted_item_pos_ = k;
}

ItemId PartialSort::bound_item_left(
        ItemPos item_pos,
        Profit lower_bound)
{
    while (item_pos < first_sorted_item_pos_
            && intervals_left_.size() > 0) {
        sort_next_left_interval(lower_bound);
    }

    if (item_pos < first_sorted_item_pos_) {
        return -1;
    } else if (item_pos >= initial_core_first_item_pos_) {
        return break_item_id();
    } else {
        return item_id(item_pos);
    }
}

ItemId PartialSort::bound_item_right(
        ItemPos item_pos,
        Profit lower_bound)
{
    while (item_pos > last_sorted_item_pos_
            && intervals_right_.size() > 0) {
        sort_next_right_interval(lower_bound);
    }

    if (item_pos >= last_sorted_item_pos_ + 1) {
        return -1;
    } else if (item_pos <= initial_core_last_item_pos_) {
        return break_item_id();
    } else {
        return item_id(item_pos);
    }
}

void PartialSort::move_item_to_core(
        ItemPos item_pos,
        ItemPos new_item_pos)
{
    if (item_pos < 0) {
        throw std::invalid_argument("item_pos < 0.");
    }

    if (item_pos >= instance().number_of_items()) {
        throw std::invalid_argument(
                "item_pos >= instance().number_of_items().");
    }

    if (item_pos < break_item_pos_
            && new_item_pos >= break_item_pos_) {
        throw std::invalid_argument("");
    }

    if (item_pos > break_item_pos_
            && new_item_pos <= break_item_pos_) {
        throw std::invalid_argument("");
    }

    if (new_item_pos < first_sorted_item_pos_) {
        throw std::invalid_argument("new_item_pos < first_sorted_item_pos_");
    }
    if (new_item_pos > last_sorted_item_pos_) {
        throw std::invalid_argument("new_item_pos > last_sorted_item_pos_");
    }

    ItemId item_id = sorted_items_[item_pos];

    if (item_pos < break_item_pos_) {
        if (item_pos < first_reduced_item_pos()) {
            for (auto interval = intervals_left_.begin();
                    interval != intervals_left_.end();) {
                if (interval->last < item_pos) {
                    interval++;
                    continue;
                }
                sorted_items_[item_pos] = sorted_items_[interval->last];
                item_pos = interval->last;
                interval->last--;
                if (std::next(interval) != intervals_left_.end())
                    std::next(interval)->first--;
                if (interval->first > interval->last) {
                    interval = intervals_left_.erase(interval);
                } else {
                    interval++;
                }
            }
            first_sorted_item_pos_--;
        }

        if (item_pos < first_sorted_item_pos_) {
            sorted_items_[item_pos] = sorted_items_[first_sorted_item_pos_];
            item_pos = first_sorted_item_pos_;
        }

        while (item_pos != new_item_pos) {
            sorted_items_[item_pos] = sorted_items_[item_pos + 1];
            item_pos++;
        }

        sorted_items_[new_item_pos] = item_id;

        initial_core_first_item_pos_--;
    } else {
        if (item_pos > last_reduced_item_pos()) {
            for (auto interval = intervals_right_.begin();
                    interval != intervals_right_.end();) {
                if (interval->first > item_pos) {
                    interval++;
                    continue;
                }
                sorted_items_[item_pos] = sorted_items_[interval->first];
                item_pos = interval->first;
                interval->first++;
                if (std::next(interval) != intervals_right_.end())
                    std::next(interval)->last++;
                if (interval->first > interval->last) {
                    interval = intervals_right_.erase(interval);
                } else {
                    interval++;
                }
            }
            last_sorted_item_pos_++;
        }

        if (item_pos > last_sorted_item_pos_) {
            sorted_items_[item_pos] = sorted_items_[last_sorted_item_pos_];
            item_pos = last_sorted_item_pos_;
        }

        while (item_pos != new_item_pos) {
            sorted_items_[item_pos] = sorted_items_[item_pos - 1];
            item_pos--;
        }

        sorted_items_[new_item_pos] = item_id;

        initial_core_last_item_pos_++;
    }
}

bool PartialSort::check() const
{
    if (break_item_id() < 0) {
        std::stringstream ss;
        format(ss);
        throw std::runtime_error(ss.str());
        return false;
    }

    if (break_item_id() >= instance().number_of_items()) {
        std::stringstream ss;
        format(ss);
        throw std::runtime_error(ss.str());
        return false;
    }

    if (break_solution().weight() > instance().capacity()) {
        std::stringstream ss;
        format(ss);
        throw std::runtime_error(ss.str());
        return false;
    }

    const Item& break_item = instance().item(break_item_id());
    if (break_solution().weight() + break_item.weight <= instance().capacity()) {
        std::stringstream ss;
        format(ss);
        throw std::runtime_error(ss.str());
        return false;
    }

    for (ItemPos item_pos = 0; item_pos < break_item_pos_; ++item_pos) {
        ItemId item_id = sorted_items_[item_pos];
        const Item& item = instance().item(item_id);
        if (item.profit * break_item.weight < break_item.profit * item.weight) {
            std::stringstream ss;
            format(ss);
            throw std::runtime_error(ss.str());
            return false;
        }
    }
    for (ItemPos item_pos = break_item_pos() + 1;
            item_pos < instance().number_of_items();
            ++item_pos) {
        ItemId item_id = sorted_items_[item_pos];
        const Item& item = instance().item(item_id);
        if (item.profit * break_item.weight > break_item.profit * item.weight) {
            std::stringstream ss;
            format(ss);
            throw std::runtime_error(ss.str());
            return false;
        }
    }

    if (intervals_left_.size() != 0) {
        if (intervals_left_.back().last > first_sorted_item_pos_ - 1) {
            std::stringstream ss;
            format(ss);
            throw std::runtime_error(ss.str());
            return false;
        }

        for (auto it = intervals_left_.begin();
                it != std::prev(intervals_left_.end());
                ++it) {
            if (it->last != std::next(it)->first - 1) {
                std::stringstream ss;
                format(ss);
                throw std::runtime_error(ss.str());
                return false;
            }
        }

        double emin_prev = std::numeric_limits<double>::infinity();
        for (auto interval: intervals_left_) {

            if (interval.first > interval.last) {
                std::stringstream ss;
                format(ss);
                throw std::runtime_error(ss.str());
                return false;
            }

            double emax = 0;
            double emin = std::numeric_limits<double>::infinity();
            for (ItemPos item_pos = interval.first;
                    item_pos <= interval.last;
                    ++item_pos) {
                ItemId item_id = sorted_items_[item_pos];
                const Item& item = instance().item(item_id);
                if (emax < item.efficiency)
                    emax = item.efficiency;
                if (emin > item.efficiency)
                    emin = item.efficiency;
            }
            if (emax > emin_prev) {
                std::stringstream ss;
                format(ss);
                throw std::runtime_error(ss.str());
                return false;
            }
            emin_prev = emin;
        }
    }

    if (intervals_right_.size() != 0) {

        if (intervals_right_.back().first < last_sorted_item_pos_ + 1) {
            std::stringstream ss;
            format(ss);
            throw std::runtime_error(ss.str());
            return false;
        }

        for (auto it = intervals_right_.rbegin();
                it != std::prev(intervals_right_.rend());
                ++it) {
            if (it->last != std::next(it)->first - 1) {
                std::stringstream ss;
                format(ss);
                throw std::runtime_error(ss.str());
                return false;
            }
        }

        double emax_prev = 0;
        for (auto interval: intervals_right_) {
            if (interval.first > interval.last) {
                std::stringstream ss;
                format(ss);
                throw std::runtime_error(ss.str());
                return false;
            }
            double emax = 0;
            double emin = std::numeric_limits<double>::infinity();
            for (ItemPos item_pos = interval.first;
                    item_pos <= interval.last;
                    ++item_pos) {
                ItemId item_id = sorted_items_[item_pos];
                const Item& item = instance().item(item_id);
                if (emax < item.efficiency)
                    emax = item.efficiency;
                if (emin > item.efficiency)
                    emin = item.efficiency;
            }
            if (emin < emax_prev) {
                std::stringstream ss;
                format(ss);
                throw std::runtime_error(ss.str());
                return false;
            }
            emax_prev = emax;
        }
    }
    return true;
}

void PartialSort::format(std::ostream& os) const
{
    os << "Intervals left:" << std::endl;
    for (auto interval: intervals_left_) {
        double emax = 0;
        double emin = std::numeric_limits<double>::infinity();
        for (ItemPos item_pos = interval.first;
                item_pos <= interval.last;
                ++item_pos) {
            ItemId item_id = sorted_items_[item_pos];
            const Item& item = instance().item(item_id);
            if (emax < item.efficiency)
                emax = item.efficiency;
            if (emin > item.efficiency)
                emin = item.efficiency;
        }
        os
            << " [" << interval.first << ", " << interval.last << "]"
            << " efficiencies [" << emin << ", " << emax << "]"
            << std::endl;
    }
    os << "Intervals right:" << std::endl;
    for (auto interval: intervals_right_) {
        double emax = 0;
        double emin = std::numeric_limits<double>::infinity();
        for (ItemPos item_pos = interval.first;
                item_pos <= interval.last;
                ++item_pos) {
            ItemId item_id = sorted_items_[item_pos];
            const Item& item = instance().item(item_id);
            if (emax < item.efficiency)
                emax = item.efficiency;
            if (emin > item.efficiency)
                emin = item.efficiency;
        }
        os
            << " [" << interval.first << ", " << interval.last << "]"
            << " efficiencies [" << emax << ", " << emin << "]"
            << std::endl;
    }
    const Item& break_item = instance().item(break_item_id());
    os << "Break item" << std::endl
        << "* position: " << break_item_pos() << std::endl
        << "* id: " << break_item_id() << std::endl
        << "* efficiency: " << break_item.efficiency << std::endl
        << std::endl;
        ;
}

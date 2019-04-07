#include "knapsack/opt_minknap/minknap.hpp"

#include "knapsack/lb_greedy/greedy.hpp"
#include "knapsack/lb_greedynlogn/greedynlogn.hpp"
#include "knapsack/ub_dembo/dembo.hpp"
#include "knapsack/ub_dantzig/dantzig.hpp"
#include "knapsack/ub_surrogate/surrogate.hpp"

using namespace knapsack;

std::ostream& knapsack::operator<<(std::ostream& os, const MinknapState& s)
{
    os << "(" << s.w << " " << s.p << ")";
    return os;
}

void Minknap::add_item(Info& info)
{
    LOG_FOLD_START(info, "add_item s " << s_ << " t " << t_ << " lb " << lb_ << std::endl);
    const Instance& ins = instance_;
    psolf_.add_item(t_ - 1);
    best_state_.sol = psolf_.remove(best_state_.sol);
    Weight c = ins.total_capacity();
    Weight wt = ins.item(t_ - 1).w;
    Profit pt = ins.item(t_ - 1).p;
    auto x = instance_.bound_items(s_, t_, lb_, info);
    Profit ub_max = -1;

    l_.clear();
    std::vector<MinknapState>::iterator it = l0_.begin();
    std::vector<MinknapState>::iterator it1 = l0_.begin();
    while (it != l0_.end() || it1 != l0_.end()) {
        if (it == l0_.end() || it->w > it1->w + wt) {
            MinknapState s1{it1->w + wt, it1->p + pt, psolf_.add(it1->sol)};
            LOG(info, "state " << *it1 << " => " << s1);
            if (l_.empty() || s1.p > l_.back().p) {
                if (s1.w <= c && s1.p > lb_) { // Update lower bound
                    std::stringstream ss;
                    ss << "s " << s_ << " t " << t_ << " (lb)";
                    update_lb(lb_, ub_, s1.p, ss, info);
                    best_state_ = s1;
                    assert(lb_ <= ub_);
                    if (lb_ == ub_) {
                        LOG_FOLD_END(info, " lb == ub");
                        return;
                    }
                }
                if (!l_.empty() && s1.w == l_.back().w) {
                    l_.back() = s1;
                    distinct_state_number_++;
                    LOG(info, " ok" << std::endl);
                } else {
                    Profit ub = (s1.w <= c)?
                        ub_dembo(ins, x.second, s1.p, c - s1.w):
                        ub_dembo_rev(ins, x.first, s1.p, c - s1.w);
                    LOG(info, " ub " << ub << " lb " << lb_);
                    if (ub > lb_) {
                        if (ub_max < ub)
                            ub_max = ub;
                        l_.push_back(s1);
                        distinct_state_number_++;
                        LOG(info, " ok" << std::endl);
                    } else {
                        LOG(info, " ×" << std::endl);
                    }
                }
            } else {
                LOG(info, " ×" << std::endl);
            }
            it1++;
        } else {
            assert(it != l0_.end());
            LOG(info, "state " << *it);
            it->sol = psolf_.remove(it->sol);
            if (l_.empty() || it->p > l_.back().p) {
                if (!l_.empty() && it->w == l_.back().w) {
                    l_.back() = *it;
                    LOG(info, " ok" << std::endl);
                } else {
                    Profit ub = (it->w <= c)?
                        ub_dembo(ins, x.second, it->p, c - it->w):
                        ub_dembo_rev(ins, x.first, it->p, c - it->w);
                    LOG(info, " ub " << ub << " lb " << lb_);
                    if (ub > lb_) {
                        if (ub_max < ub)
                            ub_max = ub;
                        l_.push_back(*it);
                        LOG(info, " ok" << std::endl);
                    } else {
                        LOG(info, " ×" << std::endl);
                    }
                }
            } else {
                LOG(info, " ×" << std::endl);
            }
            ++it;
        }
    }
    if (ub_max != -1 && ub_ < ub_max) {
        std::stringstream ss;
        ss << "s " << s_ << " t " << t_ << " (ub)";
        update_ub(lb_, ub_, ub_max, ss, info);
    }
    l0_.swap(l_);
    LOG_FOLD_END(info, "add_item");
}

void Minknap::remove_item(Info& info)
{
    LOG_FOLD_START(info, "remove_item s " << s_ << " t " << t_ << " lb " << lb_ << std::endl);
    const Instance& ins = instance_;
    psolf_.add_item(s_ + 1);
    best_state_.sol = psolf_.add(best_state_.sol);
    Weight c = ins.total_capacity();
    Weight ws = ins.item(s_ + 1).w;
    Profit ps = ins.item(s_ + 1).p;
    auto x = instance_.bound_items(s_, t_, lb_, info);
    Profit ub_max = -1;

    l_.clear();
    std::vector<MinknapState>::iterator it = l0_.begin();
    std::vector<MinknapState>::iterator it1 = l0_.begin();
    while (it != l0_.end() || it1 != l0_.end()) {
        if (it1 == l0_.end() || it->w <= it1->w - ws) {
            LOG(info, "state " << *it);
            it->sol = psolf_.add(it->sol);
            if (l_.empty() || it->p > l_.back().p) {
                if (!l_.empty() && it->w == l_.back().w) {
                    l_.back() = *it;
                    LOG(info, " ok" << std::endl);
                } else {
                    Profit ub = (it->w <= c)?
                        ub_dembo(ins, x.second, it->p, c - it->w):
                        ub_dembo_rev(ins, x.first, it->p, c - it->w);
                    LOG(info, " ub " << ub << " lb " << lb_);
                    if (ub > lb_) {
                        if (ub_max < ub)
                            ub_max = ub;
                        l_.push_back(*it);
                        LOG(info, " ok" << std::endl);
                    } else {
                        LOG(info, " ×" << std::endl);
                    }
                }
            } else {
                LOG(info, " ×" << std::endl);
            }
            ++it;
        } else {
            MinknapState s1{it1->w - ws, it1->p - ps, psolf_.remove(it1->sol)};
            LOG(info, "state " << *it1 << " => " << s1);
            if (l_.empty() || s1.p > l_.back().p) {
                if (s1.w <= c && s1.p > lb_) { // Update lower bound
                    std::stringstream ss;
                    ss << "s " << s_ << " t " << t_ << " (lb)";
                    update_lb(lb_, ub_, s1.p, ss, info);
                    best_state_ = s1;
                    assert(lb_ <= ub_);
                    if (lb_ == ub_) {
                        LOG_FOLD_END(info, " lb == ub");
                        return;
                    }
                }
                if (!l_.empty() && s1.w == l_.back().w) {
                    l_.back() = s1;
                    distinct_state_number_++;
                    LOG(info, " ok" << std::endl);
                } else {
                    Profit ub = (s1.w <= c)?
                        ub_dembo(ins, x.second, s1.p, c - s1.w):
                        ub_dembo_rev(ins, x.first, s1.p, c - s1.w);
                    LOG(info, " ub " << ub << " lb " << lb_);
                    if (ub > lb_) {
                        if (ub_max < ub)
                            ub_max = ub;
                        l_.push_back(s1);
                        distinct_state_number_++;
                        LOG(info, " ok" << std::endl);
                    } else {
                        LOG(info, " ×" << std::endl);
                    }
                }
            } else {
                LOG(info, " ×" << std::endl);
            }
            it1++;
        }
    }
    if (ub_max != -1 && ub_ < ub_max) {
        std::stringstream ss;
        ss << "s " << s_ << " t " << t_ << " (ub)";
        update_ub(lb_, ub_, ub_max, ss, info);
    }
    l0_.swap(l_);
    LOG_FOLD_END(info, "remove_item");
}

void Minknap::update_bounds(Info& info)
{
    if (params_.ub_surrogate >= 0 && params_.ub_surrogate <= (StateIdx)l0_.size()) {
        params_.ub_surrogate = -1;
        std::function<Solution (Instance&, Info, bool*)> func
            = [this](Instance& ins, Info info, bool* end) {
                return Minknap(ins, params_, end).run(info); };
        threads_.push_back(std::thread(ub_solvesurrelax, SurrelaxData{
                    .ins      = Instance::reset(instance_),
                    .lb       = lb_,
                    .sol_best = sol_best_,
                    .ub       = ub_,
                    .func     = func,
                    .end      = end_,
                    .info     = Info(info, true, "surrelax")}));
    }
    if (params_.lb_greedynlogn >= 0 && params_.lb_greedynlogn <= (StateIdx)l0_.size()) {
        params_.lb_greedynlogn = -1;
        Solution sol = sol_greedynlogn(instance_);
        if (sol_best_.profit() < sol.profit()) {
            info.output->mutex_sol.lock();
            sol_best_ = sol;
            info.output->mutex_sol.unlock();
            update_lb(lb_, ub_, sol.profit(), std::stringstream("greedynlogn"), info);
        }
    }
}

Solution Minknap::run(Info info)
{
    LOG_FOLD_START(info, "minknap k " << params_.k
            << " combo_core " << params_.combo_core
            << std::endl);
    LOG_FOLD(info, instance_);
    VER(info, "*** minknap ***" << std::endl);
    Instance& ins = instance_;
    Weight  c = ins.capacity();
    ItemPos n = ins.item_number();

    // Trivial cases
    if (n == 0 || c == 0) {
        LOG_FOLD_END(info, "no item or capacity null" << std::endl);
        sol_best_ = (ins.reduced_solution() == NULL)? Solution(ins): *ins.reduced_solution();
        return algorithm_end(sol_best_, info);
    } else if (n == 1) {
        Solution sol_tmp = (ins.reduced_solution() == NULL)? Solution(ins): *ins.reduced_solution();
        sol_tmp.set(ins.first_item(), true);
        LOG_FOLD_END(info, "1 item" << std::endl);
        if (sol_tmp.profit() > sol_best_.profit())
            sol_best_ = sol_tmp;
        return algorithm_end(sol_best_, info);
    }

    // Sort partially
    ins.sort_partially(info);
    if (ins.break_item() == ins.last_item() + 1) {
        if (ins.break_solution()->profit() > sol_best_.profit())
            sol_best_ = *ins.break_solution();
        LOG_FOLD_END(info, "all items fit in the knapsack");
        return algorithm_end(sol_best_, info);
    }
    if (lb_ != ub_ - 1 && params_.combo_core) {
        ins.init_combo_core(info);
        LOG_FOLD(info, ins);
    }

    // Compute initial lower bound
    Solution sol_tmp(ins);
    if (params_.lb_greedynlogn == 0) {
        params_.lb_greedynlogn = -1;
        sol_tmp = sol_greedynlogn(ins);
    } else if (params_.lb_greedy == 0) {
        sol_tmp = sol_greedy(ins);
    } else {
        sol_tmp = *ins.break_solution();
    }
    if (sol_tmp.profit() > lb_) {
        sol_best_ = sol_tmp;
        lb_ = sol_best_.profit();
    }

    // Compute initial upper bound
    Profit ub_tmp = ub_dantzig(ins);
    if (ub_ == -1 || ub_tmp < ub_)
        ub_ = ub_tmp;

    init_display(lb_, ub_, info);
    if (lb_ == ub_) {
        LOG_FOLD_END(info, "lower bound is optimal" << std::endl);
        return algorithm_end(sol_best_, info);
    }

    // Recursion
    Weight w_bar = ins.break_solution()->weight();
    Profit p_bar = ins.break_solution()->profit();
    l0_ = {{.w = w_bar, .p = p_bar, .sol = 0}};
    s_ = ins.break_item() - 1;
    t_ = ins.break_item();
    best_state_ = l0_.front();
    distinct_state_number_++;
    state_number_++;
    LOG_FOLD(info, ins);
    while (!l0_.empty()) {
        if (t_ <= ins.last_item()) {
            if (!info.check_time())
                break;
            if (*end_)
                return algorithm_end(Solution(ins), info);
            update_bounds(info); // Update bounds
            LOG(info, "f " << ins.first_item() << " s' " << ins.first_sorted_item()
                    << " s " << s_ << " t " << t_
                    << " t' " << ins.last_sorted_item() << " l " << ins.last_item()
                    << std::endl);
            ++t_;
            add_item(info);
            state_number_ += l0_.size();
            if (lb_ == ub_)
                break;
        }

        if (s_ >= ins.first_item()) {
            if (!info.check_time())
                break;
            if (*end_)
                return algorithm_end(Solution(ins), info);
            update_bounds(info); // Update bounds
            LOG(info, "f " << ins.first_item() << " s' " << ins.first_sorted_item()
                    << " s " << s_ << " t " << t_
                    << " t' " << ins.last_sorted_item() << " l " << ins.last_item()
                    << std::endl);
            --s_;
            remove_item(info);
            state_number_ += l0_.size();
            if (lb_ == ub_)
                break;
        }
    }
    if (lb_ != ub_)
        update_ub(lb_, ub_, lb_, std::stringstream("tree search completed"), info);

    if (!sur_)
        *end_ = true;
    LOG(info, "end" << std::endl);
    for (std::thread& thread: threads_)
        thread.join();
    threads_.clear();
    LOG(info, "end2" << std::endl);
    if (!sur_)
        *end_ = false;

    VER(info, "Total state number: " << state_number_ << std::endl);
    VER(info, "Distinct state number: " << distinct_state_number_ << std::endl);
    PUT(info, "Algorithm.TotalStateNumber", state_number_);
    PUT(info, "Algorithm.DistinctStateNumber", distinct_state_number_);

    if (best_state_.p <= sol_best_.profit())
        return algorithm_end(sol_best_, info);

    assert(best_state_.p >= lb_);

    LOG_FOLD(info, instance_);
    lb_  = best_state_.p - 1;
    ub_  = best_state_.p;
    ins.set_first_item(s_ + 1, info);
    ins.set_last_item(t_ - 1);
    ins.fix(info, psolf_.vector(best_state_.sol));

    run(Info(info, false, ""));
    LOG_FOLD_END(info, "minknap");
    return algorithm_end(sol_best_, info);
}

Profit knapsack::opt_minknap(Instance& ins, Info info)
{
    return Minknap(ins, MinknapParams()).run(info).profit();
}

Profit knapsack::opt_minknap(Instance& ins)
{
    return Minknap(ins, MinknapParams()).run().profit();
}


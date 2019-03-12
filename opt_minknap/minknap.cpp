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
    psolf_.add_item(t_);
    best_state_.sol = psolf_.remove(best_state_.sol);
    Weight c = ins.total_capacity();
    Weight wt = ins.item(t_).w;
    Profit pt = ins.item(t_).p;
    auto x = ins.bound_items_add(s_, t_, info);

    std::vector<MinknapState> l;
    std::vector<MinknapState>::iterator it = l0_.begin();
    std::vector<MinknapState>::iterator it1 = l0_.begin();
    while (it != l0_.end() || it1 != l0_.end()) {
        if (it == l0_.end() || it->w > it1->w + wt) {
            MinknapState s1{it1->w + wt, it1->p + pt, psolf_.add(it1->sol)};
            LOG(info, "state " << *it1 << " => " << s1);
            if (l.empty() || s1.p > l.back().p) {
                if (s1.w <= c && s1.p > lb_) { // Update lower bound
                    lb_ = s1.p;
                    best_state_ = s1;
                    assert(lb_ <= ub_);
                    if (lb_ == ub_)
                        return;
                }
                if (!l.empty() && s1.w == l.back().w) {
                    l.back() = s1;
                    distinct_state_number_++;
                    LOG(info, " ok" << std::endl);
                } else {
                    Profit ub = (s1.w <= c)?
                        ub_dembo(ins, x.second, s1.p, c - s1.w):
                        ub_dembo_rev(ins, x.first, s1.p, c - s1.w);
                    LOG(info, " ub " << ub << " lb " << lb_);
                    if (ub > lb_) {
                        l.push_back(s1);
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
            if (l.empty() || it->p > l.back().p) {
                if (!l.empty() && it->w == l.back().w) {
                    l.back() = *it;
                    LOG(info, " ok" << std::endl);
                } else {
                    Profit ub = (it->w <= c)?
                        ub_dembo(ins, x.second, it->p, c - it->w):
                        ub_dembo_rev(ins, x.first, it->p, c - it->w);
                    LOG(info, " ub " << ub << " lb " << lb_);
                    if (ub > lb_) {
                        l.push_back(*it);
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
    l0_ = std::move(l);
    LOG_FOLD_END(info, "add_item");
}

void Minknap::remove_item(Info& info)
{
    LOG_FOLD_START(info, "remove_item s " << s_ << " t " << t_ << " lb " << lb_ << std::endl);
    const Instance& ins = instance_;
    psolf_.add_item(s_);
    best_state_.sol = psolf_.add(best_state_.sol);
    Weight c = ins.total_capacity();
    Weight ws = ins.item(s_).w;
    Profit ps = ins.item(s_).p;
    auto x = ins.bound_items_rem(s_, t_, info);

    std::vector<MinknapState> l;
    std::vector<MinknapState>::iterator it = l0_.begin();
    std::vector<MinknapState>::iterator it1 = l0_.begin();
    while (it != l0_.end() || it1 != l0_.end()) {
        if (it1 == l0_.end() || it->w <= it1->w - ws) {
            LOG(info, "state " << *it);
            it->sol = psolf_.add(it->sol);
            if (l.empty() || it->p > l.back().p) {
                if (!l.empty() && it->w == l.back().w) {
                    l.back() = *it;
                    LOG(info, " ok" << std::endl);
                } else {
                    Profit ub = (it->w <= c)?
                        ub_dembo(ins, x.second, it->p, c - it->w):
                        ub_dembo_rev(ins, x.first, it->p, c - it->w);
                    LOG(info, " ub " << ub << " lb " << lb_);
                    if (ub > lb_) {
                        l.push_back(*it);
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
            if (l.empty() || s1.p > l.back().p) {
                if (s1.w <= c && s1.p > lb_) { // Update lower bound
                    lb_ = s1.p;
                    best_state_ = s1;
                    assert(lb_ <= ub_);
                    if (lb_ == ub_)
                        return;
                }
                if (!l.empty() && s1.w == l.back().w) {
                    l.back() = s1;
                    distinct_state_number_++;
                    LOG(info, " ok" << std::endl);
                } else {
                    Profit ub = (s1.w <= c)?
                        ub_dembo(ins, x.second, s1.p, c - s1.w):
                        ub_dembo_rev(ins, x.first, s1.p, c - s1.w);
                    LOG(info, " ub " << ub<< " lb " << lb_);
                    if (ub > lb_) {
                        l.push_back(s1);
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
    l0_ = std::move(l);
    LOG_FOLD_END(info, "remove_item");
}

Solution Minknap::run(Info& info)
{
    LOG_FOLD_START(info, "minknap k " << params_.k
            << " combo_core " << params_.combo_core
            << " opt " << opt_
            << std::endl);
    LOG_FOLD(info, instance_);
    if (opt_ == -1)
        VER(info, "*** minknap ***" << std::endl);
    Instance& ins = instance_;
    Solution sol(ins);

    if (opt_ != -1)
        lb_--;

    // Sort partially
    ins.sort_partially(info);
    if (ins.break_item() == ins.last_item() + 1) {
        if (ins.break_solution()->profit() > sol.profit())
            sol = *ins.break_solution();
        LOG_FOLD_END(info, "all items fit in the knapsack");
        return algorithm_end(sol, info);
    }
    if (opt_ == -1 && params_.combo_core) {
        ins.init_combo_core(info);
        LOG_FOLD(info, ins);
    }

    // Compute initial lower bound
    Solution sol_tmp(ins);
    if (params_.lb_greedynlogn == 0) {
        params_.lb_greedynlogn = -1;
        Info info_tmp(info.logger);
        sol_tmp = sol_greedynlogn(ins, info_tmp);
    } else if (params_.lb_greedy == 0) {
        Info info_tmp(info.logger);
        sol_tmp = sol_greedy(ins, info_tmp);
    } else {
        sol_tmp = *ins.break_solution();
    }
    if (sol_tmp.profit() > lb_) {
        sol = sol_tmp;
        lb_ = sol.profit();
    }
    LOG(info, "initial lb " << lb_ << std::endl);
    if (lb_ == ub_) {
        LOG_FOLD_END(info, "lower bound is optimal" << std::endl);
        return algorithm_end(sol, info);
    }

    Weight  c = ins.capacity();
    ItemPos n = ins.item_number();

    // Trivial cases
    if (n == 0 || c == 0) {
        LOG_FOLD_END(info, "no item or capacity null" << std::endl);
        if (ins.reduced_solution()->profit() > sol.profit()) {
            return algorithm_end(*ins.reduced_solution(), info);
        } else {
            return algorithm_end(sol, info);
        }
    } else if (n == 1) {
        Solution sol_tmp = *ins.reduced_solution();
        sol_tmp.set(ins.first_item(), true);
        LOG_FOLD_END(info, "1 item" << std::endl);
        if (sol_tmp.profit() > sol.profit()) {
            return algorithm_end(sol_tmp, info);
        } else {
            return algorithm_end(sol, info);
        }
    } else if (ins.break_item() == ins.last_item() + 1) {
        LOG_FOLD_END(info, "all items fit in the knapsack");
        if (ins.break_solution()->profit() > sol.profit()) {
            return algorithm_end(*ins.reduced_solution(), info);
        } else {
            return algorithm_end(sol, info);
        }
    }

    Weight w_bar = ins.break_solution()->weight();
    Profit p_bar = ins.break_solution()->profit();

    // Compute initial upper bound
    Info info_tmp(info.logger);
    Profit ub_tmp = ub_dantzig(ins, info_tmp);
    if (ub_ == -1 || ub_tmp < ub_)
        ub_ = ub_tmp;
    if (lb_ == ub_) {
        LOG_FOLD_END(info, "lower bound is optimal" << std::endl);
        return algorithm_end(sol, info);
    }

    // Create memory table
    l0_ = {{.w = w_bar, .p = p_bar, .sol = 0}};

    s_ = ins.break_item() - 1;
    t_ = ins.break_item();
    best_state_ = l0_.front();
    distinct_state_number_++;
    state_number_++;
    while (!l0_.empty()) {
        LOG(info, "f " << ins.first_item() << " s' " << ins.first_sorted_item()
                << " s " << s_ << " t " << t_
                << " t' " << ins.last_sorted_item() << " l " << ins.last_item()
                << std::endl);

        if (ins.int_right_size() > 0 && t_ + 1 > ins.last_sorted_item())
            ins.sort_right(lb_);
        if (t_ <= ins.last_sorted_item() && t_ <= ins.last_item()) {
            add_item(info);
            state_number_ += l0_.size();
            ++t_;
        }
        if (lb_ == ub_)
            break;

        LOG(info, "f " << ins.first_item() << " s' " << ins.first_sorted_item()
                << " s " << s_ << " t " << t_
                << " t' " << ins.last_sorted_item() << " l " << ins.last_item()
                << std::endl);

        if (ins.int_left_size() > 0 && s_ - 1 < ins.first_sorted_item())
            ins.sort_left(lb_);
        if (s_ >= ins.first_sorted_item() && s_ >= ins.first_item()) {
            remove_item(info);
            state_number_ += l0_.size();
            --s_;
        }
        if (lb_ == ub_)
            break;
    }

    VER(info, "Total state number: " << state_number_ << std::endl);
    VER(info, "Distinct state number: " << distinct_state_number_ << std::endl);
    PUT(info, "Algorithm.TotalStateNumber", state_number_);
    PUT(info, "Algorithm.DistinctStateNumber", distinct_state_number_);

    if (best_state_.p <= sol.profit())
        return algorithm_end(sol, info);
    assert(best_state_.p >= lb_);

    lb_  = best_state_.p;
    opt_ = best_state_.p;
    ins.set_first_item(s_ + 1, info);
    ins.set_last_item(t_ - 1);
    ins.fix(info, psolf_.vector(best_state_.sol));

    Info info_tmp2(info.logger);
    sol = run(info_tmp2);
    LOG_FOLD_END(info, "minknap");
    return algorithm_end(sol, info);
}


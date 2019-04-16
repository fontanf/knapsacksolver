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
    ItemPos sx = instance_.bound_item_left(s_, lb_, info);
    ItemPos tx = instance_.bound_item_right(t_, lb_, info);
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
                    ss << "it " << t_ - s_ << " (lb)";
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
                        ub_dembo(ins, tx, s1.p, c - s1.w):
                        ub_dembo_rev(ins, sx, s1.p, c - s1.w);
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
                        ub_dembo(ins, tx, it->p, c - it->w):
                        ub_dembo_rev(ins, sx, it->p, c - it->w);
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
        ss << "it " << t_ - s_ << " (ub)";
        update_ub(lb_, ub_, ub_max, ss, info);
    }
    l0_.swap(l_);
    state_number_ += l0_.size();
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
    ItemPos sx = instance_.bound_item_left(s_, lb_, info);
    ItemPos tx = instance_.bound_item_right(t_, lb_, info);
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
                        ub_dembo(ins, tx, it->p, c - it->w):
                        ub_dembo_rev(ins, sx, it->p, c - it->w);
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
                    ss << "it " << t_ - s_ << " (lb)";
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
                        ub_dembo(ins, tx, s1.p, c - s1.w):
                        ub_dembo_rev(ins, sx, s1.p, c - s1.w);
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
        ss << "it " << t_ - s_ << " (ub)";
        update_ub(lb_, ub_, ub_max, ss, info);
    }
    l0_.swap(l_);
    state_number_ += l0_.size();
    LOG_FOLD_END(info, "remove_item");
}

ItemPos Minknap::find_state(bool right, Info& info)
{
    LOG_FOLD_START(info, "find_state" << std::endl);
    Instance& ins = instance_;
    Profit lb0 = 0;
    ItemIdx j = -1;
    ItemPos first = (right)? t_: ins.first_item();
    ItemPos last  = (right)? ins.last_item(): s_;
    for (ItemPos t=first; t<=last; ++t) {
        if (ins.s_second() <= t && t < ins.s_prime())
            continue;
        if (ins.t_prime() < t && t <= ins.t_second())
            continue;
        LOG(info, "t " << t << std::endl);
        Weight w = (right)?
            ins.total_capacity() - ins.item(t).w:
            ins.total_capacity() + ins.item(t).w;
        if (l0_.front().w > w)
            continue;
        ItemPos f = 0;
        ItemPos l = l0_.size() - 1; // l0_[l] > w
        while (f + 1 < l) {
            LOG(info, "f " << f << " l " << l << std::endl);
            ItemPos m = (f + l) / 2;
            if (l0_[m].w >= w) {
                assert(l != m);
                l = m;
            } else {
                assert(f != m);
                f = m;
            }
        }
        LOG(info, "f " << f << " l " << l << std::endl);
        if (f != (StateIdx)l0_.size() - 1 && l0_[f + 1].w <= w)
            f++;
        LOG(info, "f " << f << " l " << l << std::endl);
        assert(f < (StateIdx)l0_.size());
        assert(l0_[f].w <= w);
        assert(f == (StateIdx)l0_.size() - 1 || l0_[f + 1].w > w);
        Profit lb = (right)?
            l0_[f].p + ins.item(t).p:
            l0_[f].p - ins.item(t).p;
        if (lb0 < lb) {
            j = t;
            lb0 = lb;
        }
    }
    LOG_FOLD_END(info, "find_state");
    return j;
}

void Minknap::update_bounds(Info& info)
{
    if (params_.surrogate >= 0 && params_.surrogate <= (StateIdx)l0_.size()) {
        params_.surrogate = -1;
        std::function<Solution (Instance&, Info, bool*)> func
            = [this](Instance& ins, Info info, bool* end) {
                return Minknap(ins, params_, end).run(info); };
        threads_.push_back(std::thread(ub_solvesurrelax, SurrelaxData{
                    .ins      = Instance::reset(instance_),
                    .lb       = &lb_,
                    .sol_best = &sol_best_,
                    .ub       = ub_,
                    .func     = func,
                    .end      = end_,
                    .info     = Info(info, true, "surrelax")}));
    }
    if (params_.greedynlogn >= 0 && params_.greedynlogn <= (StateIdx)l0_.size()) {
        params_.greedynlogn = -1;
        Solution sol = sol_greedynlogn(instance_);
        if (sol_best_.profit() < sol.profit())
            update_sol(&sol_best_, &lb_, ub_, sol, std::stringstream("greedynlogn"), info);
    }
    if (params_.pairing >= 0 && params_.pairing <= (StateIdx)l0_.size()) {
        LOG_FOLD_START(info, "pairing" << std::endl);
        params_.pairing *= 10;
        Instance& ins = instance_;

        if (t_ <= ins.last_item()) {
            ItemPos j = find_state(true, info);
            if (j != -1) {
                ins.add_item_to_core(s_, t_, j, info);
                ++t_;
                add_item(info);
                if (lb_ == ub_ || !info.check_time() || *end_)
                    return;
            }
        }

        if (s_ >= ins.first_item()) {
            ItemPos j = find_state(false, info);
            if (j != -1) {
                ins.add_item_to_core(s_, t_, j, info);
                --s_;
                remove_item(info);
            }
        }
        LOG_FOLD_END(info, "pairing");
    }
}

Solution Minknap::run(Info info)
{
    LOG_FOLD_START(info, "minknap k " << params_.k
            << " combo_core " << params_.combo_core
            << std::endl);
    LOG_FOLD(info, instance_);

    VER(info, "*** minknap");
    if (params_.k != 64)
        VER(info, " k " << params_.k);
    if (params_.surrogate != -1)
        VER(info, " s " << params_.surrogate);
    if (params_.pairing != -1)
        VER(info, " p " << params_.pairing);
    if (params_.combo_core)
        VER(info, " cc");
    VER(info, " ***" << std::endl);

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
    if (params_.greedynlogn == 0) {
        params_.greedynlogn = -1;
        sol_tmp = sol_greedynlogn(ins);
    } else if (params_.greedy == 0) {
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
        update_bounds(info); // Update bounds
        if (!info.check_time())
            break;
        if (sur_ && *end_) {
            LOG_FOLD_END(info, "end");
            return algorithm_end(Solution(ins), info);
        }
        if (lb_ == ub_)
            break;

        if (t_ <= ins.last_item()) {
            LOG(info, "f " << ins.first_item()
                    << " s'' " << ins.s_second()
                    << " s' " << ins.s_prime()
                    << " s " << s_
                    << " b " << ins.break_item()
                    << " t " << t_
                    << " t' " << ins.t_prime()
                    << " t'' " << ins.t_second()
                    << " l " << ins.last_item()
                    << std::endl);
            ++t_;
            add_item(info);
            if (!info.check_time())
                break;
            if (sur_ && *end_) {
                LOG_FOLD_END(info, "end");
                return algorithm_end(Solution(ins), info);
            }
            if (lb_ == ub_)
                break;
        }

        if (s_ >= ins.first_item()) {
            LOG(info, "f " << ins.first_item()
                    << " s'' " << ins.s_second()
                    << " s' " << ins.s_prime()
                    << " s " << s_
                    << " b " << ins.break_item()
                    << " t " << t_
                    << " t' " << ins.t_prime()
                    << " t'' " << ins.t_second()
                    << " l " << ins.last_item()
                    << std::endl);
            --s_;
            remove_item(info);
            if (!info.check_time())
                break;
            if (sur_ && *end_) {
                LOG_FOLD_END(info, "end");
                return algorithm_end(Solution(ins), info);
            }
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

    PUT(info, "Algorithm.TotalStateNumber", state_number_);
    PUT(info, "Algorithm.DistinctStateNumber", distinct_state_number_);

    if (lb_ == sol_best_.profit())
        return algorithm_end(sol_best_, info);

    //assert(best_state_.p >= lb_);
    LOG_FOLD(info, instance_);
    ins.set_first_item(s_ + 1, info);
    ins.set_last_item(t_ - 1);
    ins.fix(info, psolf_.vector(best_state_.sol));
    assert(ins.capacity() >= 0);

    lb_--;
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


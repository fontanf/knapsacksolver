#include "knapsack/opt_balknap/balknap.hpp"

#include "knapsack/lib/part_solution_1.hpp"
#include "knapsack/lb_greedy/greedy.hpp"
#include "knapsack/lb_greedynlogn/greedynlogn.hpp"
#include "knapsack/ub_dembo/dembo.hpp"
#include "knapsack/ub_dantzig/dantzig.hpp"
#include "knapsack/ub_surrogate/surrogate.hpp"

#include <bitset>

using namespace knapsack;

struct BalknapState
{
    Weight mu;
    Profit pi;
    bool operator()(const BalknapState& s1, const BalknapState& s2)
    {
        if (s1.mu != s2.mu)
            return s1.mu < s2.mu;
        if (s1.pi != s2.pi)
            return s1.pi < s2.pi;
        return false;
    }
};

struct BalknapValue
{
    ItemPos a;
    ItemPos a_prec;
    PartSol1 sol;
};

std::ostream& operator<<(std::ostream& os, const std::pair<BalknapState, BalknapValue>& s)
{
    os
        << "(mu " << s.first.mu
        << " pi " << s.first.pi
        << " a " << s.second.a
        << " ap " << s.second.a_prec
        //<< " s " << std::bitset<16>(s.second.sol)
        << ")";
    return os;
}

class Balknap
{

public:

    Balknap(Instance& ins, BalknapParams p, bool* end = NULL):
        instance_(ins),
        params_(p),
        sol_best_(ins),
        end_(end)
    {
        sur_ = (end_ != NULL);
        if (end_ == NULL)
            end_ = new bool(false);
    }

    ~Balknap()
    {
        if (!sur_)
            delete end_;
    }

    Solution run(Info info = Info());

private:

    ItemPos find_state(bool left, Info& info);

    void update_bounds(Info& info);

    Instance& instance_;
    BalknapParams params_;

    Profit lb_ = 0;
    Profit ub_ = -1;
    Solution sol_best_;

    ItemPos t_;
    std::map<BalknapState, BalknapValue, BalknapState> map_;

    StateIdx state_number_ = 0;
    StateIdx distinct_state_number_ = 0;

    bool sur_ = false;
    bool* end_ = NULL;
    std::vector<std::thread> threads_;

};

void Balknap::update_bounds(Info& info)
{
    if (params_.surrogate >= 0 && params_.surrogate <= (StateIdx)map_.size()) {
        params_.surrogate = -1;
        std::function<Solution (Instance&, Info, bool*)> func
            = [this](Instance& ins, Info info, bool* end) {
                return Balknap(ins, params_, end).run(info); };
        threads_.push_back(std::thread(ub_solvesurrelax, SurrelaxData{
                    .ins      = Instance::reset(instance_),
                    .lb       = &lb_,
                    .sol_best = &sol_best_,
                    .ub       = ub_,
                    .func     = func,
                    .end      = end_,
                    .info     = Info(info, true, "surrelax")}));
    }
    if (params_.greedynlogn >= 0 && params_.greedynlogn <= (StateIdx)map_.size()) {
        params_.greedynlogn = -1;
        Solution sol = sol_greedynlogn(instance_);
        if (sol_best_.profit() < sol.profit())
            update_sol(&sol_best_, &lb_, ub_, sol, std::stringstream("greedynlogn"), info);
    }
}

Solution Balknap::run(Info info)
{
    LOG_FOLD_START(info, "balknap k " << params_.k << std::endl);
    VER(info, "**** balknap ***" << std::endl;);
    Instance& ins = instance_;

    if (ins.item_number() == 0 || ins.capacity() == 0) {
        LOG_FOLD_END(info, "no item or null capacity");
        sol_best_ = (ins.reduced_solution() == NULL)? Solution(ins): *ins.reduced_solution();
        return algorithm_end(sol_best_, info);
    } else if (ins.item_number() == 1) {
        Solution sol_tmp = (ins.reduced_solution() == NULL)? Solution(ins): *ins.reduced_solution();
        sol_tmp.set(ins.first_item(), true);
        LOG_FOLD_END(info, "1 item" << std::endl);
        if (sol_tmp.profit() > sol_best_.profit())
            sol_best_ = sol_tmp;
        return algorithm_end(sol_best_, info);
    }

    // Sorting
    if (params_.ub == 'b') {
        ins.sort_partially(info);
    } else if (params_.ub == 't') {
        ins.sort(info);
    }
    if (ins.break_item() == ins.last_item()+1) {
        if (ins.break_solution()->profit() > sol_best_.profit())
            sol_best_ = *ins.break_solution();
        LOG_FOLD_END(info, "all items fit in the knapsack");
        return algorithm_end(sol_best_, info);
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
        LOG(info, "lb " << lb_ << std::endl);
    }

    // Variable reduction
    // If we already know the optimal value, we can use opt-1 as lower bound
    // for the reduction.
    if (params_.ub == 'b') {
        ins.reduce1(lb_, info);
    } else if (params_.ub == 't') {
        ins.reduce2(lb_, info);
    }
    if (ins.capacity() < 0) {
        LOG_FOLD_END(info, "negative capacity");
        return algorithm_end(sol_best_, info);
    }
    if (lb_ < ins.break_solution()->profit()) {
        sol_best_ = *ins.break_solution();
        lb_ = sol_best_.profit();
    }
    Weight  c = ins.total_capacity();
    ItemPos f = ins.first_item();
    ItemPos l = ins.last_item();
    ItemPos n = ins.item_number();

    // Trivial cases
    if (n == 0 || ins.capacity() == 0) {
        LOG_FOLD_END(info, "no item or null capacity");
        if (sol_best_.profit() < ins.reduced_solution()->profit())
            sol_best_ = *ins.reduced_solution();
        return algorithm_end(sol_best_, info);
    } else if (n == 1) {
        Solution sol_tmp = (ins.reduced_solution() == NULL)? Solution(ins): *ins.reduced_solution();
        sol_tmp.set(ins.first_item(), true);
        LOG_FOLD_END(info, "1 item" << std::endl);
        if (sol_best_.profit() < sol_tmp.profit())
            sol_best_ = sol_tmp;
        return algorithm_end(sol_best_, info);
    } else if (ins.break_item() == ins.last_item()+1) {
        if (sol_best_.profit() < ins.break_solution()->profit())
            sol_best_ = *ins.break_solution();
        LOG_FOLD_END(info, "all items fit in the knapsack");
        return algorithm_end(sol_best_, info);
    }

    ItemPos b    = ins.break_item();
    Weight w_bar = ins.break_solution()->weight();
    Profit p_bar = ins.break_solution()->profit();

    // Compute initial upper bound
    Profit ub_tmp = ub_dantzig(ins);
    if (ub_ == -1 || ub_tmp < ub_)
        ub_ = ub_tmp;

    init_display(lb_, ub_, info);
    if (lb_ == ub_) {
        LOG_FOLD_END(info, "lower bound is optimal" << std::endl);
        return algorithm_end(sol_best_, info);
    }

    // Initialization
    // Create first partial solution centered on the break item.
    map_.clear();
    PartSolFactory1 psolf(ins, params_.k, b, f, l);
    PartSol1 psol_init = 0;
    for (ItemPos j=f; j<b; ++j)
        psol_init = psolf.add(psol_init, j);
    // s(w_bar,p_bar) = b
    map_.insert({{w_bar,p_bar},{b,f,psol_init}});

    // Best state. Note that it is not a pointer
    std::pair<BalknapState, BalknapValue> best_state = {map_.begin()->first, map_.begin()->second};
    // Also keep last added item to improve the variable reduction at the end.
    ItemPos last_item = b-1;

    // Recursion
    for (ItemPos t=b; t<=l; ++t) {
        update_bounds(info);
        if (lb_ == ub_)
            goto end;
        if (!info.check_time())
            goto end;
        if (sur_ && *end_)
            return algorithm_end(Solution(ins), info);

        LOG(info, "t " << t << " (" << ins.item(t) << ")" << std::endl);
        Weight wt = ins.item(t).w;
        Profit pt = ins.item(t).p;

        // Bounding
        LOG(info, "bound" << std::endl);
        Profit ub_t = 0;
        for (auto s = map_.begin(); s != map_.end() && s->first.mu <= c;) {
            Profit pi = s->first.pi;
            Weight mu = s->first.mu;
            Profit ub_local = 0;
            if (params_.ub == 'b') {
                ub_local = (mu <= c)?
                    ub_dembo(ins, b, pi, c-mu):
                    ub_dembo_rev(ins, b, pi, c-mu);
            } else if (params_.ub == 't') {
                ub_local = (mu <= c)?
                    ub_dembo(ins, t, pi, c-mu):
                    ub_dembo_rev(ins, s->second.a, pi, c-mu);
            }
            if (ub_local < lb_) {
                LOG(info, "remove " << *s << std::endl);
                map_.erase(s++);
            } else {
                if (ub_t < ub_local)
                    ub_t = ub_local;
                s++;
            }
        }
        if (ub_t < ub_) {
            std::stringstream ss;
            ss << "it " << t - b << " (ub)";
            update_ub(lb_, ub_, ub_t, ss, info);
            if (lb_ == ub_)
                goto end;
        }

        // If there is no more states, the stop
        if (map_.size() == 0)
            break;
        if (lb_ == ub_)
            goto end;
        if (sur_ && *end_)
            return algorithm_end(Solution(ins), info);

        // Add item t
        LOG(info, "add" << std::endl);
        auto s = map_.upper_bound({c+1,0});
        auto hint = s;
        hint--;
        while (s != map_.begin() && (--s)->first.mu <= c) {
            std::pair<BalknapState, BalknapValue> s1 = {
                {s->first.mu + wt, s->first.pi + pt},
                {s->second.a, f, psolf.add(s->second.sol, t)}};
            LOG(info, s1);
            Weight mu_ = s1.first.mu;
            Profit pi_ = s1.first.pi;

            // Update LB
            if (mu_ <= c && pi_ > lb_) {
                std::stringstream ss;
                ss << "it " << t - b << " (lb)";
                update_lb(lb_, ub_, pi_, ss, info);
                best_state = s1;
                last_item = t;
                if (lb_ == ub_)
                    goto end;
            }

            // Bounding
            Profit ub_local = 0;
            if (params_.ub == 'b') {
                ub_local = (mu_ <= c)?
                    ub_dembo(ins, b, pi_, c-mu_):
                    ub_dembo_rev(ins, b, pi_, c-mu_);
            } else if (params_.ub == 't') {
                ub_local = (mu_ <= c)?
                    ub_dembo(ins, t + 1, pi_, c-mu_):
                    ub_dembo_rev(ins, s->second.a - 1, pi_, c-mu_);
            }
            if (ub_local <= lb_) {
                LOG(info, " ×" << std::endl);
                continue;
            }

            LOG(info, " ok" << std::endl);
            hint = map_.insert(hint, s1);
            if (hint->second.a < s->second.a) {
                hint->second.a = s->second.a;
                hint->second.sol = psolf.add(s->second.sol, t);
            }
            hint--;
        }

        // Remove previously added items
        LOG(info, "remove" << std::endl);
        for (auto s = map_.rbegin(); s != map_.rend() && s->first.mu > c; ++s) {
            if (s->first.mu > c + wt)
                continue;
            LOG(info, *s << std::endl);

            update_bounds(info);
            if (!info.check_time())
                goto end;
            if (lb_ == ub_)
                goto end;
            if (sur_ && *end_)
                return algorithm_end(Solution(ins), info);

            for (ItemPos j = s->second.a_prec; j < s->second.a; ++j) {
                LOG(info, "j " << j);
                Weight mu_ = s->first.mu - ins.item(j).w;
                Profit pi_ = s->first.pi - ins.item(j).p;
                std::pair<BalknapState, BalknapValue> s1 = {
                    {mu_, pi_},
                    {j, f, psolf.remove(s->second.sol, j)}};

                // Update LB
                if (mu_ <= c && pi_ > lb_) {
                    std::stringstream ss;
                    ss << "it " << t - b << " (lb)";
                    update_lb(lb_, ub_, pi_, ss, info);
                    best_state = s1;
                    last_item = t;
                    if (lb_ == ub_)
                        goto end;
                }

                // Bounding
                Profit ub_local = 0;
                if (params_.ub == 'b') {
                    ub_local = (mu_ <= c)?
                        ub_dembo(ins, b, pi_, c-mu_):
                        ub_dembo_rev(ins, b, pi_, c-mu_);
                } else if (params_.ub == 't') {
                    ub_local = (mu_ <= c)?
                        ub_dembo(ins, t + 1, pi_, c-mu_):
                        ub_dembo_rev(ins, j - 1, pi_, c-mu_);
                }
                if (ub_local <= lb_) {
                    LOG(info, " ×" << std::endl);
                    continue;
                }

                LOG(info, " ok" << std::endl);
                auto res = map_.insert(s1);
                if (!res.second) {
                    if (res.first->second.a < j) {
                        res.first->second.a = j;
                        res.first->second.sol = psolf.remove(s->second.sol, j);
                    }
                }
            }
            s->second.a_prec = s->second.a;
        }

    }
end:
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

    LOG(info, "best_state " << best_state << std::endl);
    LOG(info, "partial sol " << std::bitset<64>(best_state.second.sol) << std::endl);

    // Reduce instance to items from best_state.second.a to last_item and remove
    // the items from the partial solution from the instance.
    // Then run the algorithm again.
    ins.set_first_item(best_state.second.a, info);
    ins.set_last_item(last_item);
    ins.fix(info, psolf.vector(best_state.second.sol));

    lb_--;
    run(Info(info, false, ""));
    LOG_FOLD_END(info, "balknap");
    return algorithm_end(sol_best_, info);
}

Solution knapsack::sopt_balknap(Instance& ins, BalknapParams p, Info info)
{
    return Balknap(ins, p).run(info);
}


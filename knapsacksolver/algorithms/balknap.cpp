#include "knapsacksolver/algorithms/balknap.hpp"

#include "knapsacksolver/part_solution_1.hpp"
#include "knapsacksolver/algorithms/greedy.hpp"
#include "knapsacksolver/algorithms/greedynlogn.hpp"
#include "knapsacksolver/algorithms/dembo.hpp"
#include "knapsacksolver/algorithms/dantzig.hpp"
#include "knapsacksolver/algorithms/surrelax.hpp"

#include <bitset>

using namespace knapsacksolver;

void balknap_main(
        Instance& instance,
        BalknapOptionalParameters& parameters,
        BalknapOutput& output);

BalknapOutput knapsacksolver::balknap(
        Instance& instance,
        BalknapOptionalParameters parameters)
{
    init_display(instance, parameters.info);
    FFOT_VER(parameters.info,
               "Algorithm" << std::endl
            << "---------" << std::endl
            << "Balknap" << std::endl
            << std::endl
            << "Parameters" << std::endl
            << "----------" << std::endl
            << "Upper bound:            " << parameters.ub << std::endl
            << "Greedy:                 " << parameters.greedy << std::endl
            << "n log(n) Greedy:        " << parameters.greedynlogn << std::endl
            << "Surrogate relaxation:   " << parameters.surrelax << std::endl
            << "Partial solution size:  " << parameters.partial_solution_size << std::endl
            << std::endl);

    FFOT_LOG_FOLD_START(parameters.info, "*** balknap"
            << " -k " << parameters.partial_solution_size
            << ((parameters.greedy)? " -g": "")
            << " -n " << parameters.greedynlogn
            << " -s " << parameters.surrelax
            << " ***" << std::endl);

    bool end = false;
    if (parameters.end == NULL)
        parameters.end = &end;

    BalknapOutput output(instance, parameters.info);
    balknap_main(instance, parameters, output);

    FFOT_LOG_FOLD_END(parameters.info, "balknap");
    return output.algorithm_end(parameters.info);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

struct BalknapState
{
    Weight mu;
    Profit pi;

    bool operator()(const BalknapState& s1, const BalknapState& s2) const
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

std::ostream& operator<<(
        std::ostream& os,
        const std::pair<BalknapState, BalknapValue>& s)
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

struct BalknapInternalData
{
    BalknapInternalData(
            Instance& instance,
            BalknapOptionalParameters& parameters,
            BalknapOutput& output):
        instance(instance),
        parameters(parameters),
        output(output) { }

    Instance& instance;
    BalknapOptionalParameters& parameters;
    BalknapOutput& output;
    std::map<BalknapState, BalknapValue, BalknapState> map;
    std::vector<std::thread> threads;
};

void balknap_update_bounds(BalknapInternalData& d);

void balknap_main(
        Instance& instance,
        BalknapOptionalParameters& parameters,
        BalknapOutput& output)
{
    Info& info = parameters.info;
    output.number_of_recursive_calls++;
    FFOT_LOG_FOLD_START(info, "balknap_main"
            << " number_of_recursive_calls " << output.number_of_recursive_calls
            << std::endl);

    // Trivial cases
    if (instance.reduced_number_of_items() == 0
            || instance.reduced_capacity() == 0) {
        Solution sol_tmp = (instance.reduced_solution() == NULL)?
            Solution(instance):
            *instance.reduced_solution();
        output.update_solution(
                sol_tmp,
                std::stringstream("no item of null capacity (lb)"),
                parameters.info);
        output.update_upper_bound(
                output.lower_bound,
                std::stringstream("no item of null capacity (ub)"),
                parameters.info);
        FFOT_LOG_FOLD_END(info, "no item or null capacity");
        return;
    } else if (instance.reduced_number_of_items() == 1) {
        Solution sol_tmp = (instance.reduced_solution() == NULL)?
            Solution(instance):
            *instance.reduced_solution();
        sol_tmp.set(instance.first_item(), true);
        output.update_solution(
                sol_tmp,
                std::stringstream("one item (lb)"),
                parameters.info);
        output.update_upper_bound(
                output.lower_bound,
                std::stringstream("one item (ub)"),
                parameters.info);
        FFOT_LOG_FOLD_END(parameters.info, "one item");
        return;
    }

    // Sorting
    if (parameters.ub == 'b') {
        instance.sort_partially(FFOT_DBG(info));
    } else if (parameters.ub == 't') {
        instance.sort(FFOT_DBG(info));
    }
    if (instance.break_item() == instance.last_item() + 1) {
        output.update_solution(
                *instance.break_solution(),
                std::stringstream("all items fit in the knapsack (lb)"),
                parameters.info);
        output.update_upper_bound(
                output.lower_bound,
                std::stringstream("all items fit in the knapsack (ub)"),
                parameters.info);
        FFOT_LOG_FOLD_END(parameters.info, "all items fit in the knapsack");
        return;
    }

    // Compute initial lower bound
    Solution sol_tmp(instance);
    if (parameters.greedy) {
        auto g_output = greedy(instance);
        sol_tmp = g_output.solution;
    } else {
        sol_tmp = *instance.break_solution();
    }
    if (output.lower_bound < sol_tmp.profit()) {
        output.update_solution(
                sol_tmp,
                std::stringstream("initial solution"),
                parameters.info);
    }

    // Variable reduction
    // If we already know the optimal value, we can use opt-1 as lower bound
    // for the reduction.
    Profit lb_red = (output.number_of_recursive_calls == 1)?
        output.lower_bound:
        output.lower_bound - 1;
    if (parameters.ub == 'b') {
        instance.reduce1(lb_red, info);
    } else if (parameters.ub == 't') {
        instance.reduce2(lb_red, info);
    }
    if (instance.reduced_capacity() < 0) {
        output.update_upper_bound(
                output.lower_bound,
                std::stringstream("negative capacity after reduction"),
                info);
        FFOT_LOG_FOLD_END(info, "c < 0");
        return;
    }

    if (output.solution.profit() < instance.break_solution()->profit()) {
        output.update_solution(
                *instance.break_solution(),
                std::stringstream("break solution after reduction"),
                parameters.info);
    }

    Weight  c = instance.capacity();
    ItemPos f = instance.first_item();
    ItemPos l = instance.last_item();
    ItemPos n = instance.reduced_number_of_items();

    // Trivial cases
    if (n == 0 || instance.reduced_capacity() == 0) {
        Solution sol_tmp = (instance.reduced_solution() == NULL)?
            Solution(instance):
            *instance.reduced_solution();
        output.update_solution(
                sol_tmp,
                std::stringstream("no item or null capacity after reduction (lb)"),
                parameters.info);
        output.update_upper_bound(
                output.lower_bound,
                std::stringstream("no item of null capacity after reduction (ub)"),
                parameters.info);
        FFOT_LOG_FOLD_END(parameters.info, "no item or null capacity after reduction");
        return;
    } else if (n == 1) {
        Solution sol_tmp = (instance.reduced_solution() == NULL)?
            Solution(instance):
            *instance.reduced_solution();
        sol_tmp.set(instance.first_item(), true);
        output.update_solution(
                sol_tmp,
                std::stringstream("one item after reduction (lb)"),
                parameters.info);
        output.update_upper_bound(
                output.lower_bound,
                std::stringstream("one item after reduction (ub)"),
                parameters.info);
        FFOT_LOG_FOLD_END(parameters.info, "one item after reduction");
        return;
    } else if (instance.break_item() == instance.last_item() + 1) {
        output.update_solution(
                *instance.break_solution(),
                std::stringstream("all items fit in the knapsack after reduction (lb)"),
                parameters.info);
        output.update_upper_bound(
                output.lower_bound,
                std::stringstream("all items fit in the knapsack after reduction (ub)"),
                parameters.info);
        FFOT_LOG_FOLD_END(parameters.info, "all items fit in the knapsack after reduction");
        return;
    }

    ItemPos b    = instance.break_item();
    Weight w_bar = instance.break_solution()->weight();
    Profit p_bar = instance.break_solution()->profit();

    // Compute initial upper bound
    Profit ub_tmp = std::max(ub_dantzig(instance), output.lower_bound);
    output.update_upper_bound(
            ub_tmp,
            std::stringstream("dantzig upper bound"),
            parameters.info);

    if (output.solution.profit() == output.upper_bound) {
        FFOT_LOG_FOLD_END(parameters.info, "lower bound == upper bound");
        return;
    }

    // Initialization
    // Create first partial solution centered on the break item.
    BalknapInternalData d(instance, parameters, output);
    PartSolFactory1 psolf(instance, parameters.partial_solution_size, b, f, l);
    PartSol1 psol_init = 0;
    for (ItemPos j = f; j < b; ++j)
        psol_init = psolf.add(psol_init, j);
    // s(w_bar,p_bar) = b
    d.map.insert({{w_bar, p_bar},{b, f, psol_init}});

    // Best state. Note that it is not a pointer
    std::pair<BalknapState, BalknapValue> best_state = {d.map.begin()->first, d.map.begin()->second};
    // Also keep last added item to improve the variable reduction at the end.
    ItemPos last_item = b-1;

    Profit lb = (d.output.number_of_recursive_calls == 1)?
        d.output.lower_bound:
        d.output.lower_bound - 1;

    // Recursion
    for (ItemPos t = b; t <= l; ++t) {
        balknap_update_bounds(d);
        if (!parameters.info.check_time()) {
            if (parameters.set_end)
                *(parameters.end) = true;
            for (std::thread& thread: d.threads)
                thread.join();
            d.threads.clear();
            return;
        }
        if (parameters.stop_if_end && *(parameters.end)) {
            FFOT_LOG_FOLD_END(parameters.info, "end");
            return;
        }
        if (output.solution.profit() == output.upper_bound
                || best_state.first.pi == output.upper_bound)
            break;

        FFOT_LOG(info, "t " << t << " (" << instance.item(t) << ")" << std::endl);
        Weight wt = instance.item(t).w;
        Profit pt = instance.item(t).p;

        // Bounding
        FFOT_LOG(info, "bound" << std::endl);
        Profit ub_t = -1;
        for (auto s = d.map.begin(); s != d.map.end() && s->first.mu <= c;) {
            Profit pi = s->first.pi;
            Weight mu = s->first.mu;
            Profit ub_local = 0;
            if (d.parameters.ub == 'b') {
                ub_local = (mu <= c)?
                    ub_dembo(instance, b, pi, c-mu):
                    ub_dembo_rev(instance, b, pi, c-mu);
            } else if (d.parameters.ub == 't') {
                ub_local = (mu <= c)?
                    ub_dembo(instance, t, pi, c-mu):
                    ub_dembo_rev(instance, s->second.a, pi, c-mu);
            }
            if (ub_local < lb) {
                FFOT_LOG(info, "remove " << *s << std::endl);
                d.map.erase(s++);
            } else {
                if (ub_t < ub_local)
                    ub_t = ub_local;
                s++;
            }
        }
        if (ub_t != -1 && output.upper_bound > ub_t) {
            std::stringstream ss;
            ss << "it " << t - b << " (ub)";
            output.update_upper_bound(ub_t, ss, info);
            if (output.solution.profit() == output.upper_bound
                    || best_state.first.pi == output.upper_bound)
                goto end;
        }

        // If there is no more states, the stop
        if (d.map.size() == 0)
            break;
        if (best_state.first.pi == output.upper_bound)
            goto end;
        if (parameters.stop_if_end && *(parameters.end)) {
            FFOT_LOG_FOLD_END(parameters.info, "end");
            return;
        }

        // Add item t
        FFOT_LOG(info, "add" << std::endl);
        auto s = d.map.upper_bound({c+1,0});
        auto hint = s;
        hint--;
        while (s != d.map.begin() && (--s)->first.mu <= c) {
            std::pair<BalknapState, BalknapValue> s1 = {
                {s->first.mu + wt, s->first.pi + pt},
                {s->second.a, f, psolf.add(s->second.sol, t)}};
            FFOT_LOG(info, s1);
            Weight mu_ = s1.first.mu;
            Profit pi_ = s1.first.pi;

            // Update LB
            if (mu_ <= c && pi_ > lb) {
                if (d.output.number_of_recursive_calls == 1) {
                    std::stringstream ss;
                    ss << "it " << t - b << " (lb)";
                    output.update_lower_bound(pi_, ss, info);
                    lb = pi_;
                }
                best_state = s1;
                last_item = t;
                if (best_state.first.pi == output.upper_bound)
                    goto end;
            }

            // Bounding
            Profit ub_local = 0;
            if (d.parameters.ub == 'b') {
                ub_local = (mu_ <= c)?
                    ub_dembo(instance, b, pi_, c-mu_):
                    ub_dembo_rev(instance, b, pi_, c-mu_);
            } else if (d.parameters.ub == 't') {
                ub_local = (mu_ <= c)?
                    ub_dembo(instance, t + 1, pi_, c-mu_):
                    ub_dembo_rev(instance, s->second.a - 1, pi_, c-mu_);
            }
            if (ub_local <= lb) {
                FFOT_LOG(info, " ×" << std::endl);
                continue;
            }

            FFOT_LOG(info, " ok" << std::endl);
            hint = d.map.insert(hint, s1);
            if (hint->second.a < s->second.a) {
                hint->second.a = s->second.a;
                hint->second.sol = psolf.add(s->second.sol, t);
            }
            hint--;
        }

        // Remove previously added items
        FFOT_LOG(info, "remove" << std::endl);
        for (auto s = d.map.rbegin(); s != d.map.rend() && s->first.mu > c; ++s) {
            if (s->first.mu > c + wt)
                continue;
            FFOT_LOG(info, *s << std::endl);

            balknap_update_bounds(d);
            if (!parameters.info.check_time()) {
                if (parameters.set_end)
                    *(parameters.end) = true;
                for (std::thread& thread: d.threads)
                    thread.join();
                d.threads.clear();
                return;
            }
            if (parameters.stop_if_end && *(parameters.end)) {
                FFOT_LOG_FOLD_END(parameters.info, "end");
                return;
            }
            if (output.solution.profit() == output.upper_bound
                    || best_state.first.pi == output.upper_bound)
                break;

            for (ItemPos j = s->second.a_prec; j < s->second.a; ++j) {
                FFOT_LOG(info, "j " << j);
                Weight mu_ = s->first.mu - instance.item(j).w;
                Profit pi_ = s->first.pi - instance.item(j).p;
                std::pair<BalknapState, BalknapValue> s1 = {
                    {mu_, pi_},
                    {j, f, psolf.remove(s->second.sol, j)}};

                // Update LB
                if (mu_ <= c && pi_ > lb) {
                    if (d.output.number_of_recursive_calls == 1) {
                        std::stringstream ss;
                        ss << "it " << t - b << " (lb)";
                        output.update_lower_bound(pi_, ss, info);
                        lb = pi_;
                    }
                    best_state = s1;
                    last_item = t;
                    if (best_state.first.pi == output.upper_bound)
                        goto end;
                }

                // Bounding
                Profit ub_local = 0;
                if (d.parameters.ub == 'b') {
                    ub_local = (mu_ <= c)?
                        ub_dembo(instance, b, pi_, c-mu_):
                        ub_dembo_rev(instance, b, pi_, c-mu_);
                } else if (d.parameters.ub == 't') {
                    ub_local = (mu_ <= c)?
                        ub_dembo(instance, t + 1, pi_, c-mu_):
                        ub_dembo_rev(instance, j - 1, pi_, c-mu_);
                }
                if (ub_local <= lb) {
                    FFOT_LOG(info, " ×" << std::endl);
                    continue;
                }

                FFOT_LOG(info, " ok" << std::endl);
                auto res = d.map.insert(s1);
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
    output.update_upper_bound(
            output.lower_bound,
            std::stringstream("tree search completed"),
            parameters.info);

    if (parameters.set_end)
        *(parameters.end) = true;
    FFOT_LOG(parameters.info, "end" << std::endl);
    for (std::thread& thread: d.threads)
        thread.join();
    d.threads.clear();
    FFOT_LOG(parameters.info, "end2" << std::endl);

    if (output.lower_bound == output.solution.profit())
        return;

    FFOT_LOG(info, "best_state " << best_state << std::endl);
    FFOT_LOG(info, "partial sol " << std::bitset<64>(best_state.second.sol) << std::endl);

    // Reduce instance to items from best_state.second.a to last_item and remove
    // the items from the partial solution from the instance.
    // Then run the algorithm again.
    instance.set_first_item(best_state.second.a FFOT_DBG(FFOT_COMMA info));
    instance.set_last_item(last_item);
    instance.fix(psolf.vector(best_state.second.sol) FFOT_DBG(FFOT_COMMA info));

    FFOT_LOG_FOLD_END(info, "balknap_main");
    balknap_main(instance, parameters, output);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void balknap_update_bounds(BalknapInternalData& d)
{
    Instance& instance = d.instance;
    Info& info = d.parameters.info;

    if (d.parameters.surrelax >= 0 && d.parameters.surrelax <= (StateIdx)d.map.size()) {
        d.parameters.surrelax = -1;
        std::function<Output (Instance&, Info, bool*)> func
            = [&d](Instance& instance, Info info, bool* end)
            {
                BalknapOptionalParameters parameters;
                parameters.info = info;
                parameters.partial_solution_size = d.parameters.partial_solution_size;
                parameters.greedy = d.parameters.greedy;
                parameters.greedynlogn = d.parameters.greedynlogn;
                parameters.surrelax = -1;
                parameters.end = end;
                parameters.stop_if_end = true;
                parameters.set_end = false;
                return balknap(instance, parameters);
            };
        d.threads.push_back(std::thread(
                    solvesurrelax,
                    Instance::reset(instance),
                    std::ref(d.output),
                    func,
                    d.parameters.end,
                    Info(info, true, "surrelax")));
    }
    if (d.parameters.greedynlogn >= 0
            && d.parameters.greedynlogn <= (StateIdx)d.map.size()) {
        d.parameters.greedynlogn = -1;
        auto gn_output = greedynlogn(d.instance);
        d.output.update_solution(
                gn_output.solution,
                std::stringstream("greedynlogn"),
                d.parameters.info);
    }
}


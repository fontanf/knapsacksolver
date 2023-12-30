#pragma once

#include "knapsacksolver/multiplechoicesubsetsum/instance.hpp"

#include "optimizationtools/utils/utils.hpp"

#include <iomanip>

namespace knapsacksolver
{
namespace multiplechoicesubsetsum
{

/**
 * Solution group for a multiple-choice subset sum problem.
 */
class Solution
{

public:

    /*
     * Constructors and destructor
     */

    /** Create an empty solution. */
    Solution(const Instance& instance);

    /** Create a solution from a file. */
    Solution(const Instance& instance, std::string certificate_path);

    /** Add an item to the solution. */
    void add(ItemId item_id);

    /*
     * Getters
     */

    /** Get the instance. */
    inline const Instance& instance() const { return *instance_; }

    /** Get the number of items in the solution. */
    inline ItemPos number_of_items() const { return number_of_items_; }

    /** Get the total weight of the solution. */
    inline Weight weight() const { return weight_; }

    /** Return 'true' iff the solution contains item 'j'. */
    int8_t contains_item(ItemId item_id) const { return contains_item_[item_id]; }

    /** Return 'true' iff the solution contains an item from group 'group_id'. */
    int8_t contains_group(GroupId group_id) const { return contains_group_[group_id]; }

    /** Return 'true' iff the solution is feasible. */
    bool feasible() const { return weight_ <= instance().capacity(); }

    /** Get the total cost of the solution. */
    inline Weight objective_value() const { return weight(); }

    /*
     * Export
     */

    /** Write the solution to a file. */
    void write(std::string filepath) const;

    /** Export solution characteristics to a JSON structure. */
    nlohmann::json to_json() const;

    /** Write a formatted output of the instance to a stream. */
    void format(
            std::ostream& os,
            int verbosity_level = 1) const;

private:

    /** Instance. */
    const Instance* instance_;

    /** Number of items in the solution. */
    ItemPos number_of_items_ = 0;

    /** Weight of the solution. */
    Weight weight_ = 0;

    /** 'contains_item_[j] == true' iff the solution contains item 'j'. */
    std::vector<int8_t> contains_item_;

    /**
     * 'contains_group_[group_id] == true' iff the solution contains an item
     * from group 'group_id'.
     */
    std::vector<int8_t> contains_group_;

};

/** Stream insertion operator. */
std::ostream& operator<<(std::ostream& os, const Solution& solution);

////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// Output ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

inline optimizationtools::ObjectiveDirection objective_direction()
{
    return optimizationtools::ObjectiveDirection::Maximize;
}

/**
 * Output structure for a set covering problem.
 */
struct Output: optimizationtools::Output
{
    /** Constructor. */
    Output(const Instance& instance):
        solution(instance),
        bound(instance.capacity())
    { }


    /** Solution. */
    Solution solution;

    /** Value. */
    Weight value = 0;

    /** Bound. */
    Weight bound = -1;

    /** Elapsed time. */
    double time = 0.0;


    std::string solution_value() const
    {
        return optimizationtools::solution_value(
            objective_direction(),
            solution.feasible(),
            value);
    }

    double absolute_optimality_gap() const
    {
        return optimizationtools::absolute_optimality_gap(
                objective_direction(),
                solution.feasible(),
                value,
                bound);
    }

    double relative_optimality_gap() const
    {
       return optimizationtools::relative_optimality_gap(
            objective_direction(),
            solution.feasible(),
            value,
            bound);
    }

    bool has_solution() const { return solution.feasible() && solution.objective_value() == value; }

    virtual nlohmann::json to_json() const
    {
        return nlohmann::json {
            {"Solution", solution.to_json()},
            {"HasSolution", has_solution()},
            {"Value", value},
            {"Bound", bound},
            {"AbsoluteOptimalityGap", absolute_optimality_gap()},
            {"RelativeOptimalityGap", relative_optimality_gap()},
            {"Time", time}
        };
    }

    virtual int format_width() const { return 30; }

    virtual void format(std::ostream& os) const
    {
        int width = format_width();
        os
            << std::setw(width) << std::left << "Value: " << value << std::endl
            << std::setw(width) << std::left << "Has solution: " << has_solution() << std::endl
            << std::setw(width) << std::left << "Bound: " << bound << std::endl
            << std::setw(width) << std::left << "Absolute optimality gap: " << absolute_optimality_gap() << std::endl
            << std::setw(width) << std::left << "Relative optimality gap (%): " << relative_optimality_gap() * 100 << std::endl
            << std::setw(width) << std::left << "Time (s): " << time << std::endl
            ;
    }
};

using NewSolutionCallback = std::function<void(const Output&)>;

struct Parameters: optimizationtools::Parameters
{
    /** Callback function called when a new best solution is found. */
    NewSolutionCallback new_solution_callback = [](const Output&) { };


    virtual nlohmann::json to_json() const override
    {
        nlohmann::json json = optimizationtools::Parameters::to_json();
        json.merge_patch(
            {});
        return json;
    }

    virtual int format_width() const override { return 23; }

    virtual void format(std::ostream& os) const override
    {
        optimizationtools::Parameters::format(os);
        //int width = format_width();
        //os
        //    << std::setw(width) << std::left << "    Enable: " << reduction_parameters.reduce << std::endl
        //    ;
    }
};

}
}

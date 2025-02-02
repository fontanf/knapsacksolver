#include "knapsacksolver/subset_sum/algorithms/dynamic_programming_bellman.hpp"

#include "knapsacksolver/subset_sum/algorithm_formatter.hpp"

using namespace knapsacksolver::subset_sum;

////////////////////////////////////////////////////////////////////////////////
////////////////////// dynamic_programming_bellman_array ///////////////////////
////////////////////////////////////////////////////////////////////////////////

DynamicProgrammingBellmanArrayOutput knapsacksolver::subset_sum::dynamic_programming_bellman_array(
        const Instance& instance,
        const Parameters& parameters)
{
    DynamicProgrammingBellmanArrayOutput output(instance);
    AlgorithmFormatter algorithm_formatter(parameters, output);
    algorithm_formatter.start("Dynamic programming - Bellman - array");
    algorithm_formatter.print_header();

    output.values = std::vector<ItemId>(instance.capacity() + 1, -1);
    output.values[0] = -2;
    Weight weight_sum = 0;
    for (ItemPos item_id = 0; item_id < instance.number_of_items(); ++item_id) {
        // Check time
        if (parameters.timer.needs_to_end()) {
            algorithm_formatter.end();
            return output;
        }

        // Update DP table
        weight_sum += instance.weight(item_id);
        for (Weight weight = std::min(instance.capacity(), weight_sum);
                weight >= instance.weight(item_id);
                weight--) {
            if (output.values[weight] == -1
                    && output.values[weight - instance.weight(item_id)] != -1) {
                output.values[weight] = item_id;
            }
        }

        //for (Weight w = 0; w < c; ++w)
        //    std::cout << (values[w] >= 0);
        //std::cout << std::endl;

        // If optimum reached, stop.
        if (output.values[instance.capacity()] != -1)
            break;
    }

    Solution solution(instance);
    Weight weight_cur = 0;
    for (Weight weight = instance.capacity(); weight >= 0; weight--) {
        if (output.values[weight] != -1) {
            weight_cur = weight;
            break;
        }
    }
    while (weight_cur > 0) {
        ItemId item_id = output.values[weight_cur];
        solution.add(item_id);
        weight_cur -= instance.weight(item_id);
    }

    // Update value.
    algorithm_formatter.update_solution(
            solution,
            "algorithm end (solution)");
    // Update bound.
    algorithm_formatter.update_bound(
            solution.weight(),
            "algorithm end (bound)");

    algorithm_formatter.end();
    return output;
}

////////////////////////////////////////////////////////////////////////////////
/////////////////////// dynamic_programming_bellman_list ///////////////////////
////////////////////////////////////////////////////////////////////////////////

Output knapsacksolver::subset_sum::dynamic_programming_bellman_list(
        const Instance& instance,
        const Parameters& parameters)
{
    Output output(instance);
    AlgorithmFormatter algorithm_formatter(parameters, output);
    algorithm_formatter.start("Dynamic programming - Bellman - states");
    algorithm_formatter.print_header();

    std::vector<Weight> l0{0};
    std::vector<Weight> l;
    for (ItemId item_id = 0; item_id < instance.number_of_items(); ++item_id) {
        // Check time
        if (parameters.timer.needs_to_end()) {
            algorithm_formatter.end();
            return output;
        }

        auto it  = l0.begin();
        auto it1 = l0.begin();
        while (it != l0.end() || it1 != l0.end()) {
            if (it1 != l0.end() && (it == l0.end() || *it > *it1 + instance.weight(item_id))) {
                Weight w = *it1 + instance.weight(item_id);
                if (w > instance.capacity())
                    break;
                if (l.empty() || w != l.back())
                    l.push_back(w);
                it1++;
            } else {
                assert(it != l0.end());
                if (l.empty() || *it != l.back())
                    l.push_back(*it);
                it++;
            }
        }
        l0 = std::move(l);
        l.clear();

        // Update value.
        if (output.value < l0.back()) {
            std::stringstream ss;
            ss << "iteration " << item_id;
            algorithm_formatter.update_value(l0.back(), ss.str());
        }

        if (output.value == output.bound)
            break;
    }

    // Update bound.
    algorithm_formatter.update_bound(
            l0.back(),
            "algorithm end (bound)");

    algorithm_formatter.end();
    return output;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////// dynamic_programming_bellman_word_ram /////////////////////
////////////////////////////////////////////////////////////////////////////////

DynamicProgrammingBellmanWordRamOutput knapsacksolver::subset_sum::dynamic_programming_bellman_word_ram(
        const Instance& instance,
        const Parameters& parameters)
{
    DynamicProgrammingBellmanWordRamOutput output(instance);
    AlgorithmFormatter algorithm_formatter(parameters, output);
    algorithm_formatter.start("Dynamic programming - Bellman - word RAM");
    algorithm_formatter.print_header();

    Weight capacity_number_of_words = (instance.capacity() >> 6);
    Weight capacity_number_of_bits_to_shift = instance.capacity() - (capacity_number_of_words << 6);
    output.values = std::vector<uint64_t>(capacity_number_of_words + 1, 0);
    output.values[0] = 1;

    Weight weight_sum = 0;

    for (ItemPos item_id = 0; item_id < instance.number_of_items(); ++item_id) {
        // Check time
        if (parameters.timer.needs_to_end()) {
            algorithm_formatter.end();
            return output;
        }

        if (instance.weight(item_id) > instance.capacity())
            continue;
        Weight number_of_words_to_shift = (instance.weight(item_id) >> 6);
        Weight number_of_bits_to_shift_left = instance.weight(item_id) - (number_of_words_to_shift << 6);
        Weight number_of_bits_to_shift_right = 64 - number_of_bits_to_shift_left;
        weight_sum += instance.weight(item_id);
        Weight word_read = std::min(capacity_number_of_words, (weight_sum >> 6));
        Weight word_write = word_read - number_of_words_to_shift;
        if (number_of_bits_to_shift_left != 0) {
            while (word_write > 0) {
                output.values[word_read] |= (output.values[word_write] << number_of_bits_to_shift_left);
                output.values[word_read] |= (output.values[word_write - 1] >> number_of_bits_to_shift_right);
                word_read--;
                word_write--;
            }
            output.values[word_read] |= (output.values[word_write] << number_of_bits_to_shift_left);
        } else {
            while (word_write >= 0) {
                output.values[word_read] |= output.values[word_write];
                word_read--;
                word_write--;
            }
        }

        //for (Weight word = 0; word < number_of_words; ++word)
        //    for (int bit = 0; bit < 64; ++bit)
        //        std::cout << ((values[word] >> bit) & 1);
        //std::cout << std::endl;

        if (((output.values[capacity_number_of_words] >> capacity_number_of_bits_to_shift) & 1) == 1)
            break;
    }

    Weight optimal_value = 0;
    for (Weight word = capacity_number_of_words; optimal_value == 0; --word) {
        if (output.values[word] == 0)
            continue;
        for (int bit = 63; bit >= 0; --bit) {
            Weight weight = 64 * word + bit;
            if (weight <= instance.capacity()
                    && ((output.values[word] >> bit) & 1) == 1) {
                optimal_value = weight;
                break;
            }
        }
    }
    // Update bound.
    algorithm_formatter.update_value(
            optimal_value,
            "algorithm end (value)");
    // Update bound.
    algorithm_formatter.update_bound(
            optimal_value,
            "algorithm end (bound)");

    algorithm_formatter.end();
    return output;
}

////////////////////////////////////////////////////////////////////////////////
/////////////////// dynamic_programming_bellman_word_ram_rec ///////////////////
////////////////////////////////////////////////////////////////////////////////

std::vector<uint64_t> dynamic_programming_bellman_word_ram_rec_opts(
        const Instance& instance,
        ItemPos item_pos_1,
        ItemPos item_pos_2,
        Weight capacity,
        const Parameters& parameters)
{
    Weight capacity_number_of_words = (capacity >> 6);
    Weight capacity_number_of_bits_to_shift = capacity - (capacity_number_of_words << 6);
    std::vector<uint64_t> values(capacity_number_of_words + 1, 0);
    values[0] = 1;

    Weight weight_sum = 0;

    for (ItemPos item_id = item_pos_1; item_id < item_pos_2; ++item_id) {
        // Check time
        if (parameters.timer.needs_to_end())
            return {};

        if (instance.weight(item_id) > capacity)
            continue;
        Weight number_of_words_to_shift = (instance.weight(item_id) >> 6);
        Weight number_of_bits_to_shift_left = instance.weight(item_id) - (number_of_words_to_shift << 6);
        Weight number_of_bits_to_shift_right = 64 - number_of_bits_to_shift_left;
        weight_sum += instance.weight(item_id);
        Weight word_read = std::min(capacity_number_of_words, (weight_sum >> 6));
        Weight word_write = word_read - number_of_words_to_shift;
        //std::cout << "weight_sum " << weight_sum << std::endl;
        //std::cout << "capacity_number_of_words " << capacity_number_of_words << std::endl;
        //std::cout << "number_of_words_to_shift " << number_of_words_to_shift << std::endl;
        //std::cout << "word_read " << word_read << std::endl;
        //std::cout << "word_write " << word_write << std::endl;
        assert(word_read >= 0);
        assert(word_write >= 0);
        if (number_of_bits_to_shift_left != 0) {
            while (word_write > 0) {
                values[word_read] |= (values[word_write] << number_of_bits_to_shift_left);
                values[word_read] |= (values[word_write - 1] >> number_of_bits_to_shift_right);
                word_read--;
                word_write--;
            }
            //std::cout << word_read << " " << word_write << " " << capacity_number_of_words << std::endl;
            values[word_read] |= (values[word_write] << number_of_bits_to_shift_left);
        } else {
            while (word_write >= 0) {
                values[word_read] |= values[word_write];
                word_read--;
                word_write--;
            }
        }

        if (((values[capacity_number_of_words] >> capacity_number_of_bits_to_shift) & 1) == 1)
            break;
    }

    return values;
}

void dynamic_programming_bellman_word_ram_rec_rec(
        const Instance& instance,
        ItemPos item_pos_1,
        ItemPos item_pos_2,
        Weight capacity,
        Solution& solution,
        const Parameters& parameters)
{
    ItemPos k = (item_pos_1 + item_pos_2 - 1) / 2 + 1;

    auto values_1 = dynamic_programming_bellman_word_ram_rec_opts(
            instance,
            item_pos_1,
            k,
            capacity,
            parameters);
    if (parameters.timer.needs_to_end())
        return;
    auto values_2 = dynamic_programming_bellman_word_ram_rec_opts(
            instance,
            k,
            item_pos_2,
            capacity,
            parameters);
    if (parameters.timer.needs_to_end())
        return;

    Weight capacity_1_best = 0;
    Weight capacity_2_best = 0;
    Weight capacity_1 = 0;
    Weight capacity_2 = capacity + 1;
    for (;;) {
        if (capacity_1 + capacity_2 > capacity) {
            // Decrease capacity_2.
            for (;;) {
                capacity_2--;
                Weight word_2 = (capacity_2 >> 6);
                Weight bits_2 = capacity_2 - (word_2 << 6);
                if (capacity_2 < 0 || ((values_2[word_2] >> bits_2) & 1) == 1)
                    break;
            }
        } else {
            // Increase capacity_1.
            for (;;) {
                capacity_1++;
                Weight word_1 = (capacity_1 >> 6);
                Weight bits_1 = capacity_1 - (word_1 << 6);
                if (capacity_1 > capacity || ((values_1[word_1] >> bits_1) & 1) == 1)
                    break;
            }
        }
        if (capacity_2 < 0 || capacity_1 > capacity)
            break;
        //std::cout << "capacity_1 " << capacity_1 << " capacity_2 " << capacity_2 << std::endl;
        if (capacity_1 + capacity_2 <= capacity
                && capacity_1 + capacity_2 > capacity_1_best + capacity_2_best) {
            capacity_1_best = capacity_1;
            capacity_2_best = capacity_2;
        }
    }
    //std::cout
    //    << "item_pos_1 " << item_pos_1
    //    << " k " << k
    //    << " item_pos_2 " << item_pos_2
    //    << " c " << capacity
    //    << " capacity_1_best " << capacity_1_best
    //    << " capacity_2_best " << capacity_2_best
    //    << " capacity_1_best + capacity_2_best " << capacity_1_best + capacity_2_best
    //    << std::endl;

    if (item_pos_1 == k - 1)
        if (capacity_1_best == instance.weight(item_pos_1))
            solution.add(item_pos_1);
    if (k == item_pos_2 - 1)
        if (capacity_2_best == instance.weight(k))
            solution.add(k);

    if (item_pos_1 != k - 1) {
        dynamic_programming_bellman_word_ram_rec_rec(
                instance,
                item_pos_1,
                k,
                capacity_1_best,
                solution,
                parameters);
    }
    if (k != item_pos_2 - 1) {
        dynamic_programming_bellman_word_ram_rec_rec(
                instance,
                k,
                item_pos_2,
                capacity_2_best,
                solution,
                parameters);
    }
}

Output knapsacksolver::subset_sum::dynamic_programming_bellman_word_ram_rec(
        const Instance& instance,
        const Parameters& parameters)
{
    Output output(instance);
    AlgorithmFormatter algorithm_formatter(parameters, output);
    algorithm_formatter.start("Dynamic programming - Bellman - word RAM - recursive scheme");
    algorithm_formatter.print_header();

    Solution solution(instance);
    dynamic_programming_bellman_word_ram_rec_rec(
        instance,
        0,
        instance.number_of_items(),
        instance.capacity(),
        solution,
        parameters);
    if (parameters.timer.needs_to_end()) {
        algorithm_formatter.end();
        return output;
    }

    // Update value.
    algorithm_formatter.update_solution(
            solution,
            "algorithm end (solution)");
    // Update bound.
    algorithm_formatter.update_bound(
            solution.weight(),
            "algorithm end (bound)");

    algorithm_formatter.end();
    return output;
}

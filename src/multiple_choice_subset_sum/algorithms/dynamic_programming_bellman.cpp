#include "knapsacksolver/multiple_choice_subset_sum/algorithms/dynamic_programming_bellman.hpp"

#include "knapsacksolver/multiple_choice_subset_sum/algorithm_formatter.hpp"

#include <bitset>

using namespace knapsacksolver::multiple_choice_subset_sum;

////////////////////////////////////////////////////////////////////////////////
////////////////////// dynamic_programming_bellman_array ///////////////////////
////////////////////////////////////////////////////////////////////////////////

const Output knapsacksolver::multiple_choice_subset_sum::dynamic_programming_bellman_array(
        const Instance& instance,
        const Parameters& parameters)
{
    Output output(instance);
    AlgorithmFormatter algorithm_formatter(parameters, output);
    algorithm_formatter.start("Dynamic programming - Bellman - array");
    algorithm_formatter.print_header();

    std::vector<ItemId> values_prev(instance.capacity() + 1, -1);
    std::vector<ItemId> values_next(instance.capacity() + 1, -1);
    values_next[0] = -2;
    Weight weight_sum = 0;
    for (GroupId group_id = 0; group_id < instance.number_of_groups(); ++group_id) {
        // Check time
        if (parameters.timer.needs_to_end()) {
            algorithm_formatter.end();
            return output;
        }

        // Update DP table
        values_prev = values_next;
        Weight weight_max = 0;
        Weight weight_min = instance.capacity();
        for (ItemId item_id: instance.group(group_id).item_ids) {
            weight_max = std::max(weight_max, instance.item(item_id).weight);
            weight_min = std::min(weight_min, instance.item(item_id).weight);
        }
        weight_sum += weight_max;
        for (Weight weight = std::min(instance.capacity(), weight_sum);
                weight >= weight_min;
                weight--) {
            if (values_prev[weight] != -1)
                continue;
            for (ItemId item_id: instance.group(group_id).item_ids) {
                const Item& item = instance.item(item_id);
                if (weight - item.weight >= 0
                        && values_prev[weight - item.weight] != -1) {
                    values_next[weight] = item_id;
                    break;
                }
            }
        }

        //for (Weight w = 0; w < c; ++w)
        //    std::cout << (values[w] >= 0);
        //std::cout << std::endl;

        if (values_next[instance.capacity()] != -1)
            break;
    }

    Solution solution(instance);
    Weight weight_cur = 0;
    for (Weight weight = instance.capacity(); weight >= 0; weight--) {
        if (values_next[weight] != -1) {
            weight_cur = weight;
            break;
        }
    }
    //std::cout << "retrieve" << std::endl;
    while (weight_cur > 0) {
        ItemId item_id = values_next[weight_cur];
        solution.add(item_id);
        weight_cur -= instance.item(item_id).weight;
        //std::cout << "j " << j
        //    << " group " << instance.item(j).group_id
        //    << " w " << instance.item(j).weight
        //    << std::endl;
    }

    // Update solution.
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
///////////////////// dynamic_programming_bellman_word_ram /////////////////////
////////////////////////////////////////////////////////////////////////////////

const Output knapsacksolver::multiple_choice_subset_sum::dynamic_programming_bellman_word_ram(
        const Instance& instance,
        const Parameters& parameters)
{
    Output output(instance);
    AlgorithmFormatter algorithm_formatter(parameters, output);
    algorithm_formatter.start("Dynamic programming - Bellman - word RAM");
    algorithm_formatter.print_header();

    Weight capacity_number_of_words = (instance.capacity() >> 6);
    Weight capacity_number_of_bits_to_shift = instance.capacity() - (capacity_number_of_words << 6);
    std::vector<uint64_t> values_prev(capacity_number_of_words + 1, 0);
    std::vector<uint64_t> values_next(capacity_number_of_words + 1, 0);
    values_next[0] = 1;

    Weight weight_sum = 0;
    std::vector<Weight> number_of_words_to_shift;
    std::vector<Weight> number_of_bits_to_shift_left;
    std::vector<Weight> number_of_bits_to_shift_right;
    std::vector<Weight> word_write;
    for (GroupId group_id = 0; group_id < instance.number_of_groups(); ++group_id) {
        // Check time
        if (parameters.timer.needs_to_end()) {
            algorithm_formatter.end();
            return output;
        }

        Weight weight_max = 0;
        for (ItemId item_id: instance.group(group_id).item_ids)
            weight_max = std::max(weight_max, instance.item(item_id).weight);
        weight_sum += weight_max;

        values_prev = values_next;

        for (ItemId item_id: instance.group(group_id).item_ids) {
            const Item& item = instance.item(item_id);
            if (item.weight > instance.capacity())
                continue;
            Weight number_of_words_to_shift = (item.weight >> 6);
            Weight number_of_bits_to_shift_left = item.weight - (number_of_words_to_shift << 6);
            Weight number_of_bits_to_shift_right = 64 - number_of_bits_to_shift_left;
            weight_sum += item.weight;
            Weight word_read = std::min(capacity_number_of_words, (weight_sum >> 6));
            Weight word_write = word_read - number_of_words_to_shift;
            if (number_of_bits_to_shift_left != 0) {
                while (word_write > 0) {
                    values_next[word_read] |= (values_prev[word_write] << number_of_bits_to_shift_left);
                    values_next[word_read] |= (values_prev[word_write - 1] >> number_of_bits_to_shift_right);
                    word_read--;
                    word_write--;
                }
                values_next[word_read] |= (values_prev[word_write] << number_of_bits_to_shift_left);
            } else {
                while (word_write >= 0) {
                    values_next[word_read] |= values_prev[word_write];
                    word_read--;
                    word_write--;
                }
            }
        }

        //for (Weight word = 0; word < number_of_words; ++word)
        //    for (int bit = 0; bit < 64; ++bit)
        //        std::cout << ((values[word] >> bit) & 1);
        //std::cout << std::endl;

        if (((values_next[capacity_number_of_words] >> capacity_number_of_bits_to_shift) & 1) == 1)
            break;
    }

    Weight opt = -1;
    for (Weight word = capacity_number_of_words; opt == -1; --word) {
        //std::cout << "word " << word << " / " << capacity_number_of_words << std::endl;
        if (values_next[word] == 0)
            continue;
        for (int bit = 63; bit >= 0; --bit) {
            Weight w = 64 * word + bit;
            //std::cout << "w " << w << " val " << ((values_next[word] >> bit) & 1) << std::endl;
            if (w <= instance.capacity()
                    && ((values_next[word] >> bit) & 1) == 1) {
                opt = w;
                break;
            }
        }
    }

    // Update value.
    algorithm_formatter.update_value(opt, "algorithm end (value)");
    // Update bound.
    algorithm_formatter.update_bound(opt, "algorithm end (bound)");

    algorithm_formatter.end();
    return output;
}

////////////////////////////////////////////////////////////////////////////////
/////////////////// dynamic_programming_bellman_word_ram_rec ///////////////////
////////////////////////////////////////////////////////////////////////////////

std::vector<uint64_t> dynamic_programming_bellman_word_ram_rec_opts(
        const Instance& instance,
        GroupId group_id_1,
        GroupId group_id_2,
        Weight capacity,
        const Parameters& parameters)
{
    Weight capacity_number_of_words = (capacity >> 6);
    Weight capacity_number_of_bits_to_shift = capacity - (capacity_number_of_words << 6);
    std::vector<uint64_t> values(capacity_number_of_words + 1, 0);
    values[0] = 1;

    Weight weight_sum = 0;
    std::vector<Weight> number_of_words_to_shift;
    std::vector<Weight> number_of_bits_to_shift_left;
    std::vector<Weight> number_of_bits_to_shift_right;
    std::vector<Weight> word_write;
    for (GroupId group_id = group_id_1; group_id < group_id_2; ++group_id) {
        ItemPos group_size = instance.number_of_items(group_id);

        // Check time
        if (parameters.timer.needs_to_end())
            return {};

        Weight weight_max = 0;
        for (ItemId item_id: instance.group(group_id).item_ids)
            weight_max = std::max(weight_max, instance.item(item_id).weight);
        weight_sum += weight_max;

        Weight word_read = std::min(capacity_number_of_words, (weight_sum >> 6));
        number_of_words_to_shift.clear();
        number_of_bits_to_shift_left.clear();
        number_of_bits_to_shift_right.clear();
        word_write.clear();
        number_of_words_to_shift.resize(group_size);
        number_of_bits_to_shift_left.resize(group_size);
        number_of_bits_to_shift_right.resize(group_size);
        word_write.resize(group_size);
        for (ItemPos pos = 0; pos < group_size; ++pos) {
            ItemId item_id = instance.group(group_id).item_ids[pos];
            const Item& item = instance.item(item_id);
            number_of_words_to_shift[pos] = (item.weight >> 6);
            number_of_bits_to_shift_left[pos] = item.weight - (number_of_words_to_shift[pos] << 6);
            number_of_bits_to_shift_right[pos] = 64 - number_of_bits_to_shift_left[pos];
            word_write[pos] = word_read - number_of_words_to_shift[pos];
        }

        while (word_read >= 0) {
            for (ItemPos pos = 0; pos < group_size; ++pos) {
                if (number_of_bits_to_shift_left[pos] != 0) {
                    if (word_write[pos] > 0) {
                        values[word_read] |= (values[word_write[pos]] << number_of_bits_to_shift_left[pos]);
                        values[word_read] |= (values[word_write[pos] - 1] >> number_of_bits_to_shift_right[pos]);
                    } else if (word_write[pos] == 0) {
                        values[word_read] = (values[word_write[pos]] << number_of_bits_to_shift_left[pos]);
                    }
                } else {
                    if (word_write[pos] >= 0) {
                        values[word_read] |= values[word_write[pos]];
                    }
                }
                word_write[pos]--;
            }
            word_read--;
        }

        if (((values[capacity_number_of_words] >> capacity_number_of_bits_to_shift) & 1) == 1)
            break;
    }

    return values;
}

void dynamic_programming_bellman_word_ram_rec_rec(
        const Instance& instance,
        GroupId group_id_1,
        GroupId group_id_2,
        Weight capacity,
        Solution& solution,
        const Parameters& parameters)
{
    ItemPos k = (group_id_1 + group_id_2 - 1) / 2 + 1;

    auto values_1 = dynamic_programming_bellman_word_ram_rec_opts(
            instance,
            group_id_1,
            k,
            capacity,
            parameters);
    if (parameters.timer.needs_to_end())
        return;
    auto values_2 = dynamic_programming_bellman_word_ram_rec_opts(
            instance,
            k,
            group_id_2,
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
    //    << "n1 " << n1
    //    << " k " << k
    //    << " n2 " << n2
    //    << " c " << capacity
    //    << " capacity_1_best " << capacity_1_best
    //    << " capacity_2_best " << capacity_2_best
    //    << " capacity_1_best + capacity_2_best " << capacity_1_best + capacity_2_best
    //    << std::endl;

    if (group_id_1 == k - 1) {
        for (ItemId item_id: instance.group(group_id_1).item_ids) {
            if (capacity_1_best == instance.item(item_id).weight) {
                solution.add(item_id);
                break;
            }
        }
    }
    if (k == group_id_2 - 1) {
        for (ItemId item_id: instance.group(k).item_ids) {
            if (capacity_2_best == instance.item(item_id).weight) {
                solution.add(k);
                break;
            }
        }
    }

    if (group_id_1 != k - 1) {
        dynamic_programming_bellman_word_ram_rec_rec(
                instance,
                group_id_1,
                k,
                capacity_1_best,
                solution,
                parameters);
    }
    if (k != group_id_2 - 1) {
        dynamic_programming_bellman_word_ram_rec_rec(
                instance,
                k,
                group_id_2,
                capacity_2_best,
                solution,
                parameters);
    }
}

const Output knapsacksolver::multiple_choice_subset_sum::dynamic_programming_bellman_word_ram_rec(
        const Instance& instance,
        const Parameters& parameters)
{
    Output output(instance);
    AlgorithmFormatter algorithm_formatter(parameters, output);
    algorithm_formatter.start("Dynamic programming - Bellman - word RAM - recursve scheme");
    algorithm_formatter.print_header();

    Solution solution(instance);
    dynamic_programming_bellman_word_ram_rec_rec(
        instance,
        0,
        instance.number_of_groups(),
        instance.capacity(),
        solution,
        parameters);
    if (parameters.timer.needs_to_end()) {
        algorithm_formatter.end();
        return output;
    }

    // Update solution.
    algorithm_formatter.update_solution(solution, "algorithm end (solution)");
    // Update bound.
    algorithm_formatter.update_bound(solution.weight(), "algorithm end (bound)");

    algorithm_formatter.end();
    return output;
}

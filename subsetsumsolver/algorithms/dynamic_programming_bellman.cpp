#include "subsetsumsolver/algorithms/dynamic_programming_bellman.hpp"

using namespace subsetsumsolver;

////////////////////////////////////////////////////////////////////////////////
////////////////////// dynamic_programming_bellman_array ///////////////////////
////////////////////////////////////////////////////////////////////////////////

Output subsetsumsolver::dynamic_programming_bellman_array(
        const Instance& instance,
        Info info)
{
    init_display(instance, info);
    info.os()
            << "Algorithm" << std::endl
            << "---------" << std::endl
            << "Dynamic Programming - Bellman" << std::endl
            << std::endl
            << "Parameters" << std::endl
            << "----------" << std::endl
            << "Implementation:                  Array" << std::endl
            << std::endl;

    Output output(instance, info);
    Weight c = instance.capacity();
    std::vector<ItemId> values(c + 1, -1);
    values[0] = -2;
    Weight weight_sum = 0;
    for (ItemPos j = 0; j < instance.number_of_items(); ++j) {
        // Check time
        if (info.needs_to_end())
            return output.algorithm_end(info);

        // Update DP table
        Weight wj = instance.weight(j);
        weight_sum += wj;
        for (Weight w = std::min(c, weight_sum); w >= wj; w--)
            if (values[w] == -1 && values[w - wj] != -1)
                values[w] = j;

        //for (Weight w = 0; w < c; ++w)
        //    std::cout << (values[w] >= 0);
        //std::cout << std::endl;

        // If optimum reached, stop.
        if (values[c] != -1)
            break;
    }

    Solution solution(instance);
    Weight w_cur = 0;
    for (Weight w = c; w >= 0; w--) {
        if (values[w] != -1) {
            w_cur = w;
            break;
        }
    }
    while (w_cur > 0) {
        ItemId j = values[w_cur];
        solution.add(j);
        w_cur -= instance.weight(j);
    }
    // Update lower bound
    output.update_solution(
            solution,
            std::stringstream("tree search completed"),
            info);
    // Update upper bound
    output.update_upper_bound(
            solution.weight(),
            std::stringstream("tree search completed"),
            info);

    return output.algorithm_end(info);
}

////////////////////////////////////////////////////////////////////////////////
/////////////////////// dynamic_programming_bellman_list ///////////////////////
////////////////////////////////////////////////////////////////////////////////

Output subsetsumsolver::dynamic_programming_bellman_list(
        const Instance& instance,
        Info info)
{
    init_display(instance, info);
    info.os()
            << "Algorithm" << std::endl
            << "---------" << std::endl
            << "Dynamic Programming - Bellman" << std::endl
            << std::endl
            << "Parameters" << std::endl
            << "----------" << std::endl
            << "Implementation:                  States" << std::endl
            << "Method for retrieving solution:  No solution" << std::endl
            << std::endl;

    Output output(instance, info);

    std::vector<Weight> l0{0};
    std::vector<Weight> l;
    for (ItemId j = 0; j < instance.number_of_items(); ++j) {
        // Check time
        if (info.needs_to_end())
            return output.algorithm_end(info);

        Weight wj = instance.weight(j);
        auto it  = l0.begin();
        auto it1 = l0.begin();
        while (it != l0.end() || it1 != l0.end()) {
            if (it1 != l0.end() && (it == l0.end() || *it > *it1 + wj)) {
                Weight w = *it1 + wj;
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

        // Update lower bound.
        if (output.lower_bound < l0.back()) {
            std::stringstream ss;
            ss << "iteration " << j;
            output.update_lower_bound(l0.back(), ss, info);
        }

        if (output.optimal())
            break;
    }

    // Update upper bound.
    output.update_upper_bound(
            l0.back(),
            std::stringstream("tree search completed"),
            info);

    return output.algorithm_end(info);
}

////////////////////////////////////////////////////////////////////////////////
///////////////////// dynamic_programming_bellman_word_ram /////////////////////
////////////////////////////////////////////////////////////////////////////////

std::string int2bits(int64_t value)
{
    std::string s;
    for (int i = 0; i < 64; ++i)
        s += std::to_string((value >> i) & 1);
    return s;
}

Output subsetsumsolver::dynamic_programming_bellman_word_ram(
        const Instance& instance,
        Info info)
{
    init_display(instance, info);
    info.os()
            << "Algorithm" << std::endl
            << "---------" << std::endl
            << "Dynamic Programming - Bellman" << std::endl
            << std::endl
            << "Parameters" << std::endl
            << "----------" << std::endl
            << "Implementation:                  Word RAM" << std::endl
            << "Method for retrieving solution:  No solution" << std::endl
            << std::endl;

    Output output(instance, info);

    Weight capacity_number_of_words = (instance.capacity() >> 6);
    Weight capacity_number_of_bits_to_shift = instance.capacity() - (capacity_number_of_words << 6);
    std::vector<uint64_t> values(capacity_number_of_words + 1, 0);
    values[0] = 1;

    Weight weight_sum = 0;

    for (ItemPos j = 0; j < instance.number_of_items(); ++j) {
        // Check time
        if (info.needs_to_end())
            return output.algorithm_end(info);

        Weight wj = instance.weight(j);
        Weight number_of_words_to_shift = (wj >> 6);
        Weight number_of_bits_to_shift_left = wj - (number_of_words_to_shift << 6);
        Weight number_of_bits_to_shift_right = 64 - number_of_bits_to_shift_left;
        weight_sum += wj;
        Weight word_read = std::min(capacity_number_of_words, (weight_sum >> 6));
        Weight word_write = word_read - number_of_words_to_shift;
        if (number_of_bits_to_shift_left != 0) {
            while (word_write > 0) {
                values[word_read] |= (values[word_write] << number_of_bits_to_shift_left);
                values[word_read] |= (values[word_write - 1] >> number_of_bits_to_shift_right);
                word_read--;
                word_write--;
            }
            values[word_read] = (values[word_write] << number_of_bits_to_shift_left);
        } else {
            while (word_write >= 0) {
                values[word_read] |= values[word_write];
                word_read--;
                word_write--;
            }
        }

        //for (Weight word = 0; word < number_of_words; ++word)
        //    for (int bit = 0; bit < 64; ++bit)
        //        std::cout << ((values[word] >> bit) & 1);
        //std::cout << std::endl;

        if (((values[capacity_number_of_words - 1] >> capacity_number_of_bits_to_shift) & 1) == 1)
            break;
    }

    Weight opt = 0;
    for (Weight word = capacity_number_of_words; opt == 0; --word) {
        if (values[word] == 0)
            continue;
        for (int bit = 63; bit >= 0; --bit) {
            Weight w = 64 * word + bit;
            if (w <= instance.capacity()
                    && ((values[word] >> bit) & 1) == 1) {
                opt = w;
                break;
            }
        }
    }
    // Update lower bound.
    output.update_lower_bound(
            opt,
            std::stringstream("tree search completed"),
            info);
    // Update upper bound.
    output.update_upper_bound(
            opt,
            std::stringstream("tree search completed"),
            info);

    return output.algorithm_end(info);
}

////////////////////////////////////////////////////////////////////////////////
/////////////////// dynamic_programming_bellman_word_ram_rec ///////////////////
////////////////////////////////////////////////////////////////////////////////

std::vector<uint64_t> dynamic_programming_bellman_word_ram_rec_opts(
        const Instance& instance,
        ItemPos n1,
        ItemPos n2,
        Weight capacity,
        Info& info)
{
    Weight capacity_number_of_words = (capacity >> 6);
    Weight capacity_number_of_bits_to_shift = capacity - (capacity_number_of_words << 6);
    std::vector<uint64_t> values(capacity_number_of_words + 1, 0);
    values[0] = 1;

    Weight weight_sum = 0;

    for (ItemPos j = n1; j < n2; ++j) {
        // Check time
        if (info.needs_to_end())
            return {};

        Weight wj = instance.weight(j);
        if (wj > capacity)
            continue;
        Weight number_of_words_to_shift = (wj >> 6);
        Weight number_of_bits_to_shift_left = wj - (number_of_words_to_shift << 6);
        Weight number_of_bits_to_shift_right = 64 - number_of_bits_to_shift_left;
        weight_sum += wj;
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
            values[word_read] = (values[word_write] << number_of_bits_to_shift_left);
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
        ItemPos n1,
        ItemPos n2,
        Weight capacity,
        Solution& solution,
        Info& info)
{
    ItemPos k = (n1 + n2 - 1) / 2 + 1;

    auto values_1 = dynamic_programming_bellman_word_ram_rec_opts(
            instance, n1, k, capacity, info);
    if (info.needs_to_end())
        return;
    auto values_2 = dynamic_programming_bellman_word_ram_rec_opts(
            instance, k, n2, capacity, info);
    if (info.needs_to_end())
        return;

    Weight c1_best = 0;
    Weight c2_best = 0;
    Weight c1 = 0;
    Weight c2 = capacity + 1;
    for (;;) {
        if (c1 + c2 > capacity) {
            // Decrease c2.
            for (;;) {
                c2--;
                Weight word_2 = (c2 >> 6);
                Weight bits_2 = c2 - (word_2 << 6);
                if (c2 < 0 || ((values_2[word_2] >> bits_2) & 1) == 1)
                    break;
            }
        } else {
            // Increase c1.
            for (;;) {
                c1++;
                Weight word_1 = (c1 >> 6);
                Weight bits_1 = c1 - (word_1 << 6);
                if (c1 > capacity || ((values_1[word_1] >> bits_1) & 1) == 1)
                    break;
            }
        }
        if (c2 < 0 || c1 > capacity)
            break;
        //std::cout << "c1 " << c1 << " c2 " << c2 << std::endl;
        if (c1 + c2 <= capacity
                && c1 + c2 > c1_best + c2_best) {
            c1_best = c1;
            c2_best = c2;
        }
    }
    //std::cout
    //    << "n1 " << n1
    //    << " k " << k
    //    << " n2 " << n2
    //    << " c " << capacity
    //    << " c1_best " << c1_best
    //    << " c2_best " << c2_best
    //    << " c1_best + c2_best " << c1_best + c2_best
    //    << std::endl;

    if (n1 == k - 1)
        if (c1_best == instance.weight(n1))
            solution.add(n1);
    if (k == n2 - 1)
        if (c2_best == instance.weight(k))
            solution.add(k);

    if (n1 != k - 1) {
        dynamic_programming_bellman_word_ram_rec_rec(
                instance, n1, k, c1_best, solution, info);
    }
    if (k != n2 - 1) {
        dynamic_programming_bellman_word_ram_rec_rec(
                instance, k, n2, c2_best, solution, info);
    }
}

Output subsetsumsolver::dynamic_programming_bellman_word_ram_rec(
        const Instance& instance,
        Info info)
{
    init_display(instance, info);
    info.os()
            << "Algorithm" << std::endl
            << "---------" << std::endl
            << "Dynamic Programming - Bellman" << std::endl
            << std::endl
            << "Parameters" << std::endl
            << "----------" << std::endl
            << "Implementation:                  Word RAM" << std::endl
            << "Method for retrieving solution:  Recursive scheme" << std::endl
            << std::endl;

    Output output(instance, info);

    Solution solution(instance);
    dynamic_programming_bellman_word_ram_rec_rec(
        instance,
        0,
        instance.number_of_items(),
        instance.capacity(),
        solution,
        info);
    if (info.needs_to_end())
        return output;

    output.update_solution(
            solution,
            std::stringstream("tree search completed"),
            info);
    output.update_upper_bound(
            solution.weight(),
            std::stringstream("tree search completed"),
            info);
    return output.algorithm_end(info);
}

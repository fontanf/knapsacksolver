#include "lib/instance.hpp"

namespace knapsack
{

void generate_uncorrelated(boost::filesystem::path dir,
        const std::vector<ItemIdx>& ns, const std::vector<Profit>& rs);
void generate_weakly_correlated(boost::filesystem::path dir,
        const std::vector<ItemIdx>& ns, const std::vector<Profit>& rs);
void generate_strongly_correlated(boost::filesystem::path dir,
        const std::vector<ItemIdx>& ns, const std::vector<Profit>& rs);
void generate_inverse_strongly_correlated(boost::filesystem::path dir,
        const std::vector<ItemIdx>& ns, const std::vector<Profit>& rs);
void generate_almost_strongly_correlated(boost::filesystem::path dir,
        const std::vector<ItemIdx>& ns, const std::vector<Profit>& rs);
void generate_subset_sum(boost::filesystem::path dir,
        const std::vector<ItemIdx>& ns, const std::vector<Profit>& rs);
void generate_similar_weights(boost::filesystem::path dir,
        const std::vector<ItemIdx> ns);

void generate_all(boost::filesystem::path dir);

}


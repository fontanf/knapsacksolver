//#include "knapsack/opt_bab/bab.hpp"
#include "knapsack/opt_babstar/bab.hpp"
//#include "knapsack/opt_bellman/bellman_list.hpp"
//#include "knapsack/opt_bellman/bellman_array.hpp"
//#include "knapsack/opt_expknap/expknap.hpp"
//#include "knapsack/opt_balknap/balknap.hpp"
#include "knapsack/opt_minknap/minknap.hpp"

#include "knapsack/lib/generator.hpp"

using namespace knapsack;

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    //for (std::string t: {"u", "wc", "sc", "isc", "asc"}) {
        //for (ItemIdx i=10; i<50; i++) {
            //for (Weight r=5; r<=200; ++r) {
                //for (int h=1; h<=100; ++h) {
                    std::string t = "sc";
                    ItemIdx i = 30;
                    Weight r = 100;
                    int h = 50;
                    std::cout << t << " " << i << " " << r << " " << h << std::endl;
                    std::cout << std::endl;
                    Instance ins = generate(t, i, r, h);
                    //std::cout << ins << std::endl;

                    Instance ins1(ins);
                    Info info1(true);
                    Solution sopt1 = sopt_minknap_list_part(ins1, info1);

                    std::cout << std::endl;

                    Instance ins2(ins);
                    Info info2(true, false, false);
                    Solution sopt2 = sopt_babstar_dp(ins2, info2);
                    if (sopt1.profit() != sopt2.profit()) {
                        std::cout << info2.debug_string << std::endl;
                        return 0;
                    }

                    std::cout << std::endl;
                    std::cout << std::endl;

                //}
            //}
        //}
    //}
    return 0;
}

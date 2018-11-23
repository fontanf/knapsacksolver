#include "knapsack/opt_bab/bab.hpp"
#include "knapsack/opt_babstar/bab.hpp"
#include "knapsack/opt_bellman/bellman_list.hpp"
#include "knapsack/opt_bellman/bellman_array.hpp"
#include "knapsack/opt_expknap/expknap.hpp"
#include "knapsack/opt_balknap/balknap.hpp"
#include "knapsack/opt_minknap/minknap.hpp"

#include "knapsack/lib/generator.hpp"

using namespace knapsack;

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    /*
    {
        Instance ins = generate("sc", 1000, 1000, 50);
        Info info;
        info.set_verbose();
        //info.set_debug();
        //info.set_debuglive();
        Solution sopt2 = sopt_bellman_array_rec(ins, info);
        return 0;
    }
    */

    //for (std::string t: {"u", "wc", "sc", "isc", "asc"}) {
        //for (ItemIdx i=10; i<50; i++) {
            //for (Weight r=5; r<=200; ++r) {
                //for (int h=1; h<=100; ++h) {
                    std::string t = "asc";
                    ItemIdx i = 100;
                    Weight r = 10000;
                    int h = 58;
                    std::cout << t << " " << i << " " << r << " " << h << std::endl;
                    std::cout << std::endl;
                    Instance ins = generate(t, i, r, h);
                    //std::cout << ins << std::endl;

                    Instance ins1(ins);
                    Info info1;
                    info1.set_verbose();
                    Solution sopt1 = sopt_minknap_list_part(ins1, info1);
                    //std::cout << sopt1.print_bin() << std::endl;
                    //std::cout << sopt1.print_in() << std::endl;

                    std::cout << std::endl;

                    Instance ins2(ins);
                    Info info2;
                    info2.set_verbose();
                    //info2.set_debug();
                    //info2.set_debuglive();
                    Solution sopt2 = sopt_starknap(ins2, info2);
                    //Solution sopt2 = sopt_balknap_list_all(ins2, info2);
                    //if (sopt1.profit() != sopt2.profit()) {
                        //std::cout << info2.debug_string << std::endl;
                        //return 0;
                    //}

                    std::cout << std::endl;
                    std::cout << std::endl;

                //}
            //}
        //}
    //}
    return 0;
}

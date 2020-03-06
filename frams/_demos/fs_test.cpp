#include <iostream>
#include <assert.h>
#include "frams/genetics/fs/fS_conv.h"
#include "frams/genetics/fs/fS_general.h"

using namespace std;

int main() {
    GenoConv_fS0 converter = GenoConv_fS0();
    SString test_cases[][2] = {
            {"E", "p:sh=1\n"},
            {"P", "p:sh=2\n"},
            {"C", "p:sh=3\n"},
            {"EEE", "p:sh=1\np:sh=1\np:sh=1\nj:1, 2, sh=1\nj:0, 1, sh=1\n"},
            {"E(E,E)", "p:sh=1\np:sh=1\np:sh=1\nj:0, 1, sh=1\nj:0, 2, sh=1\n"},
            {"E(E(E,E),E,E(E,E),E)", "p:sh=1\n"
                                     "p:sh=1\n"
                                     "p:sh=1\n"
                                     "p:sh=1\n"
                                     "p:sh=1\n"
                                     "p:sh=1\n"
                                     "p:sh=1\n"
                                     "p:sh=1\n"
                                     "p:sh=1\n"
                                     "j:1, 2, sh=1\n"
                                     "j:1, 3, sh=1\n"
                                     "j:0, 1, sh=1\n"
                                     "j:0, 4, sh=1\n"
                                     "j:5, 6, sh=1\n"
                                     "j:5, 7, sh=1\n"
                                     "j:0, 5, sh=1\n"
                                     "j:0, 8, sh=1\n"},
    };
    for(int i=0; i<6; i++) {
        SString *test = test_cases[i];
        SString genotype_str = test[0];
        SString result = converter.convert(genotype_str);
        SString expected_result = test[1];

        cout << expected_result.c_str() << " " << result.c_str() <<endl;
        assert(expected_result == result);
    }
    cout << "FINISHED" << endl;
    return 0;
}

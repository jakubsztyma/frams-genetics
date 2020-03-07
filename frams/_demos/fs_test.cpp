#include <iostream>
#include <assert.h>
#include "frams/genetics/fs/fS_conv.h"
#include "frams/genetics/fs/fS_general.h"

using namespace std;

int main() {
    GenoConv_fS0 converter = GenoConv_fS0();

    SString test_cases[][2] = {
            {"E",                            "p:sh=1\n"},
            {"P",                            "p:sh=2\n"},
            {"C",                            "p:sh=3\n"},
            {"EE{ry=90}",                    "p:sh=1\n"
                                             "p:z=2.0, sh=1\n"
                                             "j:0, 1, sh=1\n"},
            {"EE{rz=90}",                    "p:sh=1\n"
                                             "p:y=2.0, sh=1\n"
                                             "j:0, 1, sh=1\n"},  // Z rotation
            {"EE{rz=90}E{rx=90}E{ry=90}",    "p:sh=1\n"
                                             "p:y=2.0, sh=1\n"
                                             "p:y=2.0, 2.0, sh=1\n"
                                             "p:-1.99, 2.0, 2.0, sh=1\n"
                                             "j:2, 3, sh=1\n"
                                             "j:1, 2, sh=1\n"
                                             "j:0, 1, sh=1\n"},  // All rotations
            {"EE{rz=45}E{rx=45}E{ry=45}",    "p:sh=1\n"
                                             "p:1.41, 1.41, sh=1\n"
                                             "p:2.83, 2.41, 1.0, sh=1\n"
                                             "p:3.12, 3.41, 2.71, sh=1\n"
                                             "j:2, 3, sh=1\n"
                                             "j:1, 2, sh=1\n"
                                             "j:0, 1, sh=1\n"},  // Acute angle rotations
            {"EE{rz=-90}E{rx=-90}E{ry=-90}", "p:sh=1\n"
                                             "p:y=-1.99, sh=1\n"
                                             "p:y=-1.99, 2.0, sh=1\n"
                                             "p:2.0, -1.99, 2.0, sh=1\n"
                                             "j:2, 3, sh=1\n"
                                             "j:1, 2, sh=1\n"
                                             "j:0, 1, sh=1\n"},   // Negative rotations
            {"EEE",                          "p:sh=1\np:2.0, sh=1\np:4.0, sh=1\nj:1, 2, sh=1\nj:0, 1, sh=1\n"},
            {"E(E,E)",                       "p:sh=1\np:2.0, sh=1\np:2.0, sh=1\nj:0, 1, sh=1\nj:0, 2, sh=1\n"},
            {"E(E(E,E),E,E(E,E),E)",         "p:sh=1\n"
                                             "p:2.0, sh=1\n"
                                             "p:4.0, sh=1\n"
                                             "p:4.0, sh=1\n"
                                             "p:2.0, sh=1\n"
                                             "p:2.0, sh=1\n"
                                             "p:4.0, sh=1\n"
                                             "p:4.0, sh=1\n"
                                             "p:2.0, sh=1\n"
                                             "j:1, 2, sh=1\n"
                                             "j:1, 3, sh=1\n"
                                             "j:0, 1, sh=1\n"
                                             "j:0, 4, sh=1\n"
                                             "j:5, 6, sh=1\n"
                                             "j:5, 7, sh=1\n"
                                             "j:0, 5, sh=1\n"
                                             "j:0, 8, sh=1\n"
            },
//            {"E{m=2.1}E{m=3.2}",             "p:sh=1, m=2\n"
//                                             "p:2.0, sh=1, m=3\n"
//                                             "j:0, 1, sh=1\n"},
            {"PbcdP", "p:sh=2\n"
                      "p:2.0, sh=2\n"
                      "j:0, 1, sh=1\n"
                      "j:0, 1, sh=1\n"
                      "j:0, 1, sh=1"},
//            {"", ""},
//            {"", ""},
//            {"", ""},
//            {"", ""},
//            {"", ""},
//            {"", ""},
//            {"", ""},
//            {"", ""},
//            {"", ""},
//            {"", ""},
//            {"", ""},
    };
    for (int i = 0; i < 12; i++) {
        SString *test = test_cases[i];
        SString genotype_str = test[0];
        SString result = converter.convert(genotype_str);
        SString expected_result = test[1];

        cout << test[0].c_str() << "\n Expected: \n" << expected_result.c_str() << "\n Result: \n" << result.c_str()
             << endl;
        assert(expected_result == result);
    }
    cout << "FINISHED" << endl;
    return 0;
}

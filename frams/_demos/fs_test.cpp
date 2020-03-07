#include <iostream>
#include <assert.h>
#include "frams/genetics/fs/fS_conv.h"
#include "frams/genetics/fs/fS_general.h"

using namespace std;

int main() {
    GenoConv_fS0 converter = GenoConv_fS0();

    SString test_cases[][2] = {
            {"SE",                            "p:sh=1\n"},
            {"SP",                            "p:sh=2\n"},
            {"SC",                            "p:sh=3\n"},
            {"SEE{ry=90}",                    "p:sh=1\n"
                                              "p:z=2.0, sh=1\n"
                                              "j:0, 1, sh=1\n"},
            {"SEE{rz=90}",                    "p:sh=1\n"
                                              "p:y=2.0, sh=1\n"
                                              "j:0, 1, sh=1\n"},  // Z rotation
            {"SEE{rz=90}E{rx=90}E{ry=90}",    "p:sh=1\n"
                                              "p:y=2.0, sh=1\n"
                                              "p:y=2.0, 2.0, sh=1\n"
                                              "p:-1.99, 2.0, 2.0, sh=1\n"
                                              "j:2, 3, sh=1\n"
                                              "j:1, 2, sh=1\n"
                                              "j:0, 1, sh=1\n"},  // All rotations
            {"SEE{rz=45}E{rx=45}E{ry=45}",    "p:sh=1\n"
                                              "p:1.41, 1.41, sh=1\n"
                                              "p:2.83, 2.41, 1.0, sh=1\n"
                                              "p:3.12, 3.41, 2.71, sh=1\n"
                                              "j:2, 3, sh=1\n"
                                              "j:1, 2, sh=1\n"
                                              "j:0, 1, sh=1\n"},  // Acute angle rotations
            {"SEE{rz=-90}E{rx=-90}E{ry=-90}", "p:sh=1\n"
                                              "p:y=-1.99, sh=1\n"
                                              "p:y=-1.99, 2.0, sh=1\n"
                                              "p:2.0, -1.99, 2.0, sh=1\n"
                                              "j:2, 3, sh=1\n"
                                              "j:1, 2, sh=1\n"
                                              "j:0, 1, sh=1\n"},   // Negative rotations
            {"SEEE",                          "p:sh=1\np:2.0, sh=1\np:4.0, sh=1\nj:1, 2, sh=1\nj:0, 1, sh=1\n"},
            {"SE(E,E)",                       "p:sh=1\np:2.0, sh=1\np:2.0, sh=1\nj:0, 1, sh=1\nj:0, 2, sh=1\n"},
            {"SE(E(E,E),E,E(E,E),E)",         "p:sh=1\n"
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
            {"SEbE",                          "p:sh=1\n"
                                              "p:2.0, sh=1\n"
                                              "j:0, 1, sh=2\n"}, // Parametrized joints
            {"SPbcdP",                        "p:sh=2\n"
                                              "p:2.0, sh=2\n"
                                              "j:0, 1, sh=2\n"
                                              "j:0, 1, sh=3\n"
                                              "j:0, 1, sh=4\n"}, // Many parametrized joints
            {"SECbCcPdCbcPbcdE",              "p:sh=1\n"
                                              "p:2.0, sh=3\n"
                                              "p:4.0, sh=3\n"
                                              "p:6.0, sh=2\n"
                                              "p:8.0, sh=3\n"
                                              "p:10.0, sh=2\n"
                                              "p:12.0, sh=1\n"
                                              "j:5, 6, sh=2\n"
                                              "j:5, 6, sh=3\n"
                                              "j:5, 6, sh=4\n"
                                              "j:4, 5, sh=2\n"
                                              "j:4, 5, sh=3\n"
                                              "j:3, 4, sh=4\n"
                                              "j:2, 3, sh=3\n"
                                              "j:1, 2, sh=2\n"
                                              "j:0, 1, sh=1\n"},
            {"SE{fr=0.3}E{fr=0.5}",           "p:sh=1, fr=0.3\n"
                                              "p:2.0, sh=1, fr=0.5\n"
                                              "j:0, 1, sh=1\n"},
// Modifier mode
            {"ME",                            "p:sh=1\n"},  // Basic modifier mode
            {"MFE",                            "p:sh=1, fr=0.44\n"},  // Friction modifier
            {"MfE",                            "p:sh=1, fr=0.36\n"},  // Friction modifier
            {"MFFFFffE",                            "p:sh=1, fr=0.48\n"},  // Friction modifier
    };
    for (int i = 0; i < 19; i++) {
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

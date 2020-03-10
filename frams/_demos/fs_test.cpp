#include <iostream>
#include <assert.h>
#include "frams/genetics/fs/fS_conv.h"
#include "frams/genetics/fs/fS_general.h"


using namespace std;

int countJoints(SString genotype){
    int result = 0;
    char tmp;
    for(int i=0; i<genotype.len(); i++){
        tmp = genotype[i];
        if(tmp == 'b' || tmp == 'c' || tmp == 'd')
            result += 1;
    }
    return result;
}

int countParams(SString genotype){
    int result = 0;
    for(int i=0; i<genotype.len(); i++){
        if(genotype[i] == '=')
            result += 1;
    }
    return result;
}
int main() {
    srand (time(NULL));

    GenoConv_fS0 converter = GenoConv_fS0();

    SString test_cases[][2] = {
            {"SE",                            "p:sh=1\n"},
            {"SP",                            "p:sh=2\n"},
            {"SC",                            "p:sh=3\n"},
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
// Modifier mode
            {"ME",                            "p:sh=1\n"},  // Basic modifier mode
            {"MFE",                           "p:sh=1, fr=0.44\n"},  // Friction modifier
            {"MfE",                           "p:sh=1, fr=0.36\n"},  // Friction modifier
            {"MFFFFffE",                      "p:sh=1, fr=0.48\n"},  // Friction modifier
            {"SE{fr=0.3}E{fr=0.5}",           "p:sh=1, fr=0.3\n"
                                              "p:2.0, sh=1, fr=0.5\n"
                                              "j:0, 1, sh=1\n"},
            {"SEE{ry=90.0}",                    "p:sh=1\n"
                                              "p:z=2.0, sh=1\n"
                                              "j:0, 1, sh=1\n"},
            {"SEE{rz=90.0}",                    "p:sh=1\n"
                                              "p:y=2.0, sh=1\n"
                                              "j:0, 1, sh=1\n"},  // Z rotation
            {"SEE{rz=90.0}E{rx=90.0}E{ry=90.0}",    "p:sh=1\n"
                                              "p:y=2.0, sh=1\n"
                                              "p:y=2.0, 2.0, sh=1\n"
                                              "p:-1.99, 2.0, 2.0, sh=1\n"
                                              "j:2, 3, sh=1\n"
                                              "j:1, 2, sh=1\n"
                                              "j:0, 1, sh=1\n"},  // All rotations
            {"SEE{rz=45.0}E{rx=45.0}E{ry=45.0}",    "p:sh=1\n"
                                              "p:1.41, 1.41, sh=1\n"
                                              "p:2.83, 2.41, 1.0, sh=1\n"
                                              "p:3.12, 3.41, 2.71, sh=1\n"
                                              "j:2, 3, sh=1\n"
                                              "j:1, 2, sh=1\n"
                                              "j:0, 1, sh=1\n"},  // Acute angle rotations
            {"SEE{rz=-90.0}E{rx=-90.0}E{ry=-90.0}", "p:sh=1\n"
                                              "p:y=-1.99, sh=1\n"
                                              "p:y=-1.99, 2.0, sh=1\n"
                                              "p:2.0, -1.99, 2.0, sh=1\n"
                                              "j:2, 3, sh=1\n"
                                              "j:1, 2, sh=1\n"
                                              "j:0, 1, sh=1\n"},   // Negative rotations
    };
    int size = 19;
    for (int i = 0; i < size; i++) {
        SString *test = test_cases[i];
        SString genotype_str = test[0];
        SString result = converter.convert(genotype_str);
        SString expected_result = test[1];

        assert(expected_result == result);
    }
    for (int i = 0; i < size; i++) {
        SString *test = test_cases[i];
        fS_Genotype geno(test[0]);
        assert(geno.getGeno() == test[0]);
    }

    int expectedCount[] = {1, 1, 1, 3, 3, 9, 2, 2, 7, 1, 1, 1, 1, 2, 2, 2, 4, 4, 4};
    for (int i = 0; i < size; i++) {
        fS_Genotype geno(test_cases[i][0]);
        assert(geno.getPartCount() == expectedCount[i]);
    }
    for(int i=0; i<size; i++){
        SString genotype_str = test_cases[i][0];
        fS_Genotype geno(genotype_str);
        bool success = geno.addJoint();
        if(success)
            assert(countJoints(genotype_str) + 1 == countJoints(geno.getGeno()));
    }
    for(int i=0; i<size; i++){
        SString genotype_str = test_cases[i][0];
        fS_Genotype geno(genotype_str);
        bool success = geno.removeJoint();
        if(success)
            assert(countJoints(genotype_str) - 1 == countJoints(geno.getGeno()));
    }
//    for(int i=0; i<size; i++){
//        SString genotype_str = test_cases[i][0];
//        fS_Genotype geno(genotype_str);
//        bool success = geno.removeParam();
//        cout<<countParams(genotype_str) <<" "<<geno.getGeno().c_str() <<endl;
//        if(success)
//            assert(countParams(genotype_str) - 1 == countParams(geno.getGeno()));
//    }
    for(int i=0; i<size; i++){
        SString genotype_str = test_cases[i][0];
        fS_Genotype geno(genotype_str);
        bool success = geno.addParam();
        if(success)
            assert(countParams(genotype_str) + 1 == countParams(geno.getGeno()));
    }
    for(int i=0; i<size; i++){
        SString genotype_str = test_cases[i][0];
        fS_Genotype geno(genotype_str);
        int initialCount = geno.getPartCount();
        geno.addPart();
        assert(initialCount + 1 == geno.getPartCount());
    }
    for(int i=0; i<size; i++){
        SString genotype_str = test_cases[i][0];
        fS_Genotype geno(genotype_str);
        int initialCount = geno.getPartCount();
        bool success = geno.removePart();
        cout<<success<<" "<< initialCount << " "<<geno.getPartCount()<<endl;
        if(success)
            assert(initialCount == 1 + geno.getPartCount());
    }
    cout << "FINISHED" << endl;
    return 0;
}

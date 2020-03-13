#include <iostream>
#include <assert.h>
#include <chrono>
#include "frams/genetics/fs/fS_conv.h"
#include "frams/genetics/fs/fS_general.h"


using namespace std;


int countSigns(SString genotype, char *chars, int count) {
    int result = 0;
    for (int i = 0; i < genotype.len(); i++) {
        for (int j = 0; j < count; j++) {
            if (genotype[i] == chars[j])
                result += 1;
        }
    }
    return result;
}

int countJoints(SString genotype) {
    char signs[3] = {'b', 'c', 'd'};
    return countSigns(genotype, signs, 3);
}

int countParams(SString genotype) {
    char signs[1] = {'='};
    return countSigns(genotype, signs, 1);
}

int countModifiers(SString genotype) {
    char signs[2] = {'F', 'f'};
    return countSigns(genotype, signs, 2);
}

int main() {
    srand(time(NULL));

    GenoConv_fS0 converter = GenoConv_fS0();

    SString test_cases[][2] = {
            {"SE",                                  "p:sh=1\n"},
            {"SP",                                  "p:sh=2\n"},
            {"SC",                                  "p:sh=3\n"},
            {"SEEE",                                "p:sh=1\np:2.0, sh=1\np:4.0, sh=1\nj:1, 2, sh=1\nj:0, 1, sh=1\n"},
            {"SE(E,E)",                             "p:sh=1\np:2.0, sh=1\np:2.0, sh=1\nj:0, 1, sh=1\nj:0, 2, sh=1\n"},
            {"SE(E(E,E),E,E(E,E),E)",               "p:sh=1\n"
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
            {"SEbE",                                "p:sh=1\n"
                                                    "p:2.0, sh=1\n"
                                                    "j:0, 1, sh=2\n"}, // Parametrized joints
            {"SPbcdP",                              "p:sh=2\n"
                                                    "p:2.0, sh=2\n"
                                                    "j:0, 1, sh=2\n"
                                                    "j:0, 1, sh=3\n"
                                                    "j:0, 1, sh=4\n"}, // Many parametrized joints
            {"SECbCcPdCbcPbcdE",                    "p:sh=1\n"
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
            {"ME",                                  "p:sh=1\n"},  // Basic modifier mode
            {"MFE",                                 "p:sh=1, fr=0.44\n"},  // Friction modifier
            {"MfE",                                 "p:sh=1, fr=0.36\n"},  // Friction modifier
            {"MFFFFffE",                            "p:sh=1, fr=0.48\n"},  // Friction modifier
            {"SE{fr=0.3}E{fr=0.5}",                 "p:sh=1, fr=0.3\n"
                                                    "p:2.0, sh=1, fr=0.5\n"
                                                    "j:0, 1, sh=1\n"},
            {"SEE{ry=90.0}",                        "p:sh=1\n"
                                                    "p:z=2.0, sh=1\n"
                                                    "j:0, 1, sh=1\n"},
            {"SEE{rz=90.0}",                        "p:sh=1\n"
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
    bool success = false;
    int tmp = -1;
    const int size = 19;
    int expectedPartCount[] = {1, 1, 1, 3, 3, 9, 2, 2, 7, 1, 1, 1, 1, 2, 2, 2, 4, 4, 4};
    auto start = chrono::steady_clock::now();
    for (int i = 0; i < size; i++) {
        // Test translate
        SString *test = test_cases[i];
        SString genotype_str = test[0];
        SString result = converter.convert(genotype_str);
        SString expected_result = test[1];
        if (true)
        {
            assert(expected_result == result);

            // Test get geno
            fS_Genotype geno1(test[0]);
            assert(geno1.getGeno() == test[0]);

            ////Test operations
            // Test part count
            fS_Genotype geno2(test_cases[i][0]);
            assert(geno2.getPartCount() == expectedPartCount[i]);

            // Test add joint
            fS_Genotype geno3(genotype_str);
            success = geno3.addJoint();
            if (success)
                assert(countJoints(genotype_str) + 1 == countJoints(geno3.getGeno()));

            // Test remove joint
            fS_Genotype geno4(genotype_str);
            success = geno4.removeJoint();
            if (success)
                assert(countJoints(genotype_str) - 1 == countJoints(geno4.getGeno()));

            // Test add param
            fS_Genotype geno5(genotype_str);
            success = geno5.addParam();
            if (success)
                assert(countParams(genotype_str) + 1 == countParams(geno5.getGeno()));

            // Test add part
            fS_Genotype geno6(genotype_str);
            tmp = geno6.getPartCount();
            geno6.addPart();
            assert(tmp + 1 == geno6.getPartCount());

            // Test remove part
            fS_Genotype geno7(genotype_str);
            tmp = geno7.getPartCount();
            success = geno7.removePart();
            if (success)
                assert(tmp == 1 + geno7.getPartCount());

            // Test change param
            fS_Genotype geno8(genotype_str);
            success = geno8.changeParam();
            if (success)
                assert(countParams(genotype_str) == countParams(geno8.getGeno()));

            // Test remove param
            fS_Genotype geno9(genotype_str);
            success = geno9.removeParam();
            if (success)
                assert(countParams(genotype_str) == 1 + countParams(geno9.getGeno()));

            // Test change part
            fS_Genotype geno11(genotype_str);
            tmp = geno11.getPartCount();
            success = geno11.changePartType();
            if (success)
                assert(tmp == geno11.getPartCount());

            // Test remove modifier
            fS_Genotype geno12(genotype_str);
            success = geno12.removeModifier();
            if (success)
                assert(countModifiers(genotype_str) == 1 + countModifiers(geno12.getGeno()));

            // Test add modifier
            fS_Genotype geno13(genotype_str);
            success = geno13.addModifier();
            if (success)
                assert(countModifiers(genotype_str) + 1 == countModifiers(geno13.getGeno()));
        }

        for(int i=0; i<10000; i++) {
            fS_Genotype geno10(genotype_str);
            geno10.mutate();
            assert(genotype_str != geno10.getGeno());
            geno10.buildModel(new Model());
        }
    }
    auto end = chrono::steady_clock::now();
    cout<<chrono::duration_cast<chrono::milliseconds>(end - start).count()<<" ms"<<endl;

    cout << "FINISHED" << endl;
    return 0;
}

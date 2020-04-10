#include <iostream>
#include <stdio.h>
#include <assert.h>
#include <chrono>
#include "frams/genetics/fs/fS_conv.h"
#include "frams/genetics/fs/fS_oper.h"

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
    int count = 10;
    char signs[count] = {'I', 'i', 'F', 'f', 'X', 'x', 'Y', 'y', 'Z', 'z'};
    return countSigns(genotype, signs, count);
}

int main() {
    srand(time(NULL));

    GenoConv_fS0 converter = GenoConv_fS0();

    SString test_cases[][2] = {
            {"S:E",                                          "p:sh=1\n"},
            {"S:P",                                          "p:sh=2\n"},
            {"S:C",                                          "p:sh=3\n"},
            {"S:EEE",                                        "p:sh=1\np:2.0, sh=1\np:4.0, sh=1\nj:1, 2, sh=1\nj:0, 1, sh=1\n"},
            {"S:E(E,E)",                                     "p:sh=1\np:2.0, sh=1\np:2.0, sh=1\nj:0, 1, sh=1\nj:0, 2, sh=1\n"},
            {"S:E(E(E,E),E,E(E,E),E)",                       "p:sh=1\n"
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
            {"S:EbE",                                        "p:sh=1\n"
                                                             "p:2.0, sh=1\n"
                                                             "j:0, 1, sh=2\n"}, // Parametrized joints
            {"S:PbcdP",                                      "p:sh=2\n"
                                                             "p:2.0, sh=2\n"
                                                             "j:0, 1, sh=2\n"
                                                             "j:0, 1, sh=3\n"
                                                             "j:0, 1, sh=4\n"}, // Many parametrized joints
            {"S:ECbCcPdCbcPbcdE",                            "p:sh=1\n"
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
            {"M:E",                                          "p:sh=1\n"},  // Basic modifier mode
            {"M:FE",                                         "p:sh=1, fr=0.44\n"},  // Friction modifier
            {"M:fE",                                         "p:sh=1, fr=0.36\n"},  // Friction modifier
            {"M:FFFFffE",                                    "p:sh=1, fr=0.48\n"},  // Friction modifier
            {"S:E{f=0.3}E{f=0.5}",                           "p:sh=1, fr=0.3\n"
                                                             "p:2.0, sh=1, fr=0.5\n"
                                                             "j:0, 1, sh=1\n"},
            {"S:EE{ty=90.0}",                                "p:sh=1\n"
                                                             "p:z=2.0, sh=1\n"
                                                             "j:0, 1, sh=1\n"},
            {"S:EE{tz=90.0}",                                "p:sh=1\n"
                                                             "p:y=2.0, sh=1\n"
                                                             "j:0, 1, sh=1\n"},  // Z rotation
            {"S:EE{tz=90.0}E{tx=90.0}E{ty=90.0}",            "p:sh=1\n"
                                                             "p:y=2.0, sh=1\n"
                                                             "p:y=2.0, 2.0, sh=1\n"
                                                             "p:-1.99, 2.0, 2.0, sh=1\n"
                                                             "j:2, 3, sh=1\n"
                                                             "j:1, 2, sh=1\n"
                                                             "j:0, 1, sh=1\n"},  // All rotations
            {"S:EE{tz=45.0}E{tx=45.0}E{ty=45.0}",            "p:sh=1\n"
                                                             "p:1.41, 1.41, sh=1\n"
                                                             "p:2.83, 2.41, 1.0, sh=1\n"
                                                             "p:3.12, 3.41, 2.71, sh=1\n"
                                                             "j:2, 3, sh=1\n"
                                                             "j:1, 2, sh=1\n"
                                                             "j:0, 1, sh=1\n"},  // Acute angle rotations
            {"S:EE{tz=-90.0}E{tx=-90.0}E{ty=-90.0}",         "p:sh=1\n"
                                                             "p:y=-1.99, sh=1\n"
                                                             "p:y=-1.99, 2.0, sh=1\n"
                                                             "p:2.0, -1.99, 2.0, sh=1\n"
                                                             "j:2, 3, sh=1\n"
                                                             "j:1, 2, sh=1\n"
                                                             "j:0, 1, sh=1\n"},   // Negative rotations
            {"S:E{j=4.1}EE",                                 "p:sh=1\n"
                                                             "p:2.0, sh=1\n"
                                                             "p:4.0, sh=1\n"
                                                             "j:1, 2, sh=1\n"
                                                             "j:0, 1, sh=1\n"
                                                             "j:0, 2, sh=1\n"},
            {"S:E{j=3.9}EE",                                 "p:sh=1\n"
                                                             "p:2.0, sh=1\n"
                                                             "p:4.0, sh=1\n"
                                                             "j:1, 2, sh=1\n"
                                                             "j:0, 1, sh=1\n"},
            {"S:E{j=4.1}EEE",                                "p:sh=1\n"
                                                             "p:2.0, sh=1\n"
                                                             "p:4.0, sh=1\n"
                                                             "p:6.0, sh=1\n"
                                                             "j:2, 3, sh=1\n"
                                                             "j:1, 2, sh=1\n"
                                                             "j:0, 1, sh=1\n"
                                                             "j:0, 2, sh=1\n"},
            {"S:EE{x=3.0}",                                  "p:sh=1\n"
                                                             "p:4.0, sh=1, sx=3.0\n"
                                                             "j:0, 1, sh=1\n"},
            {"S:EE{x=3.0;y=3.0;z=3.0}",                      "p:sh=1\n"
                                                             "p:4.0, sh=1, sx=3.0, sy=3.0, sz=3.0\n"
                                                             "j:0, 1, sh=1\n"},
            {"M:XXE",                                        "p:sh=1, sx=1.21\n"},  // sx modifier
            {"M:xxE",                                        "p:sh=1, sx=0.83\n"},  // sx modifier
            {"M:XYYZZZE",                                    "p:sh=1, sx=1.1, sy=1.21, sz=1.33\n"},  // size modifiers
            {"M:EXYYZZZE",                                   "p:sh=1\n"
                                                             "p:2.08, sh=1, sx=1.1, sy=1.21, sz=1.33\n"
                                                             "j:0, 1, sh=1\n"},  // size modifiers
            {"M:XYYZZZEE",                                   "p:sh=1, sx=1.1, sy=1.21, sz=1.33\n"
                                                             "p:2.2, sh=1, sx=1.1, sy=1.21, sz=1.33\n"
                                                             "j:0, 1, sh=1\n"},  // size modifiers
            {"M:IE",                                         "p:sh=1, ing=0.28\n"},  // Ingestion modifier
            {"M:iE",                                         "p:sh=1, ing=0.23\n"},  // Ingestion modifier
            {"M:IIIIiiE",                                    "p:sh=1, ing=0.3\n"},  // Ingestion modifier
            {"S:E{i=0.3}E{i=0.5}",                           "p:sh=1, ing=0.3\n"     // Ingestion param
                                                             "p:2.0, sh=1, ing=0.5\n"
                                                             "j:0, 1, sh=1\n"},
            {"MS:IIIIiiE{i=0.5}",                            "p:sh=1, ing=0.61\n"},  // Ingestion modifier and param
            // Test collisions
            {"S:EE{ty=180.0;x=3.0}",                         "p:sh=1\n"
                                                             "p:-3.99, sh=1, sx=3.0\n"
                                                             "j:0, 1, sh=1\n"},
            {"S:EE{ty=90.0;z=5.0}",                          "p:sh=1\n"
                                                             "p:z=6.0, sh=1, sz=5.0\n"
                                                             "j:0, 1, sh=1\n"},
            {"S:E{x=1.5;z=2.0}E{tz=90.0;x=2.0;y=5.0;z=3.0}", "p:sh=1, sx=1.5, sz=2.0\n"
                                                             "p:y=5.96, sh=1, sx=2.0, sy=5.0, sz=3.0\n"
                                                             "j:0, 1, sh=1\n"},
            {"S:E{y=3.0;z=4.0}E{y=4.0;z=5.0}",               "p:sh=1, sy=3.0, sz=4.0\n"
                                                             "p:2.0, sh=1, sy=4.0, sz=5.0\n"
                                                             "j:0, 1, sh=1\n"},
            {"S:E{y=399.0;z=599.0}E{y=799.0;z=999.0}",       "p:sh=1, sy=399.0, sz=599.0\n"
                                                             "p:53.25, sh=1, sy=799.0, sz=999.0\n"
                                                             "j:0, 1, sh=1\n"},  // Test a lot of spheres
            {"S:EE{ty=45.0}",                                "p:sh=1\n"
                                                             "p:1.41, z=1.41, sh=1\n"
                                                             "j:0, 1, sh=1\n"},
            {"S:EE{ty=120.0}",                               "p:sh=1\n"
                                                             "p:-0.99, z=1.73, sh=1\n"
                                                             "j:0, 1, sh=1\n"},
            {"S:EE{ry=90.0;z=2.0}",                          "p:sh=1\n"
                                                             "p:3.0, sh=1, sz=2.0, ry=90.0\n"
                                                             "j:0, 1, sh=1\n"},
            {"S:E{ry=90.0;z=2.0}E{ry=90.0;z=2.0}",           "p:sh=1, sz=2.0, ry=90.0\n"
                                                             "p:4.0, sh=1, sz=2.0, ry=90.0\n"
                                                             "j:0, 1, sh=1\n"},
            {"S:EE{ry=45.0;z=2.0}",                          "p:sh=1\n"
                                                             "p:2.58, sh=1, sz=2.0, ry=45.0\n"
                                                             "j:0, 1, sh=1\n"},
            {"S:EE{ry=30.0;z=2.0}",                          "p:sh=1\n"
                                                             "p:2.3, sh=1, sz=2.0, ry=30.0\n"
                                                             "j:0, 1, sh=1\n"},
    };
    bool success = false;
    int tmp = -1;
    const int size = 45;
    int expectedPartCount[] = {1, 1, 1, 3, 3, 9, 2, 2, 7, 1, 1, 1, 1, 2, 2, 2, 4, 4, 4, 3, 3, 4, 2, 2, 1, 1, 1, 2,
                               2, 1, 1, 1, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
    auto start = chrono::steady_clock::now();
    MultiMap map;
    for (int i = 0; i < size; i++) {
        // Test translate
        SString *test = test_cases[i];
        SString genotype_str = test[0];
        cout << test[0].c_str() << endl;
        if (true) {
            cout << converter.convert(genotype_str, &map, false).c_str() << endl;
            cout << test[1].c_str() << endl;
            assert(test[1] == converter.convert(genotype_str, &map, false).c_str());

            // Test get geno
            fS_Genotype geno1(test[0]);
            cout << geno1.getGeno().c_str() << endl;
            assert(geno1.getGeno() == test[0]);

            ////Test operations
            // Test part count
            fS_Genotype geno2(test_cases[i][0]);
            assert(geno2.getNodeCount() == expectedPartCount[i]);

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
            tmp = geno6.getNodeCount();
            geno6.addPart();
            assert(tmp + 1 == geno6.getNodeCount());

            // Test remove part
            fS_Genotype geno7(genotype_str);
            tmp = geno7.getNodeCount();
            success = geno7.removePart();
            if (success)
                assert(tmp == 1 + geno7.getNodeCount());

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
            tmp = geno11.getNodeCount();
            success = geno11.changePartType();
            if (success)
                assert(tmp == geno11.getNodeCount());

            // Test remove modifier
            fS_Genotype geno12(genotype_str);
            success = geno12.removeModifier();
            cout << geno12.getGeno().c_str() << endl;
            if (success)
                assert(countModifiers(genotype_str) == 1 + countModifiers(geno12.getGeno()));

            // Test add modifier
            fS_Genotype geno13(genotype_str);
            success = geno13.addModifier();
            if (success)
                assert(countModifiers(genotype_str) + 1 == countModifiers(geno13.getGeno()));
        }
    }

    if (true) {
        fS_Operators operators;
        SString invalidGenotypes[] = {
                "EEE",      // No mode specifier
                "S:FFF",    // No part type
                "S:FFF{x=5.0}",    // No part type
                "M:qqE",    // Invalid modifier
                "S:E{f}",    // No equal sign
//            "S:E{qw=1.0}",    // Wrong param key
//            "S:E{f=}",    // Wrong param value
//            "S:E{f=fr}",    // Wrong param value
        };
        const int invalidCount = 5;
        for (int i = 0; i < invalidCount; i++) {
            MultiMap map;
            cout << invalidGenotypes[i].c_str() << endl;
            assert(1 == operators.checkValidity(invalidGenotypes[i].c_str(), ""));
            SString genes = converter.convert(invalidGenotypes[i], &map, false);
            assert(genes == "");
        }

        SString *g1 = new SString("SMJ:EE{x=3.0;y=3.0;z=3.0}");
        SString *g2 = new SString("SMJ:C{j=3.9}CC");
        FILE *pFile = fopen("output.txt", "w");
        int operationCount = 100;
        for (int i = 0; i < operationCount; i++) {
            cout << i << " out of " << operationCount << " Length: " << g1->len() + g2->len() << endl;
//            cout << g1->c_str() << endl;
//            cout << g2->c_str() << endl;
            int method;
            float f1, f2, gp;

            char *arr1 = strdup(g1->c_str());
            char *arr2 = strdup(g2->c_str());

            operators.mutate(arr1, gp, method);
            operators.mutate(arr2, gp, method);

            int crossOverResult = operators.crossOver(arr1, arr2, f1, f2);

            if (crossOverResult == GENOPER_OK) {
                assert(0 == operators.checkValidity(arr1, ""));
                assert(0 == operators.checkValidity(arr2, ""));
//            assert(SString(arr1) != *g1);
//            assert(SString(arr2) != *g2);
            }

            delete g1;
            delete g2;
            g1 = new SString(arr1);
            g2 = new SString(arr2);
            fprintf(pFile, g1->c_str());
            fprintf(pFile, "\n");
            fprintf(pFile, converter.convert(*g1, &map, false).c_str());
            fprintf(pFile, "\n");
            free(arr1);
            free(arr2);
        }
        delete g1;
        delete g2;
        fclose(pFile);
    }

    auto end = chrono::steady_clock::now();
    cout << chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << endl;

    cout << "FINISHED" << endl;
    return 0;
}

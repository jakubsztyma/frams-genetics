//
// Created by jakub on 21.02.2020.
//

#ifndef CPP_FS_OPER_H
#define CPP_FS_OPER_H

class fS_Operators{
public:
    int mutate(char *&geno, float& chg, int &method){
        fS_Genotype genotype(geno);
        genotype.mutate();

        free(geno);
        geno = strdup(genotype.getGeno().c_str());
        return GENOPER_OK;
    }

    int crossOver(char *&g1, char *&g2, float& chg1, float& chg2){int parentCount = 2;
        fS_Genotype *parents[parentCount] = {new fS_Genotype(g1), new fS_Genotype(g2)};

        if(parents[0]->start_node->childSize == 0 || parents[1]->start_node->childSize == 0) {
            delete parents[0];
            delete parents[1];
            return GENOPER_OPFAIL;
        }

        Node *chosen[parentCount];
        int indexes[2];
        for(int i=0; i<parentCount; i++) {
            vector < Node * > allNodes = parents[i]->getTree();
            do {
                chosen[i] = allNodes[parents[i]->randomFromRange(allNodes.size(), 0)];
            } while (chosen[i]->childSize == 0);
            indexes[i] = parents[i]->randomFromRange(chosen[i]->childSize, 0);
        }
        swap(chosen[0]->children[indexes[0]], chosen[1]->children[indexes[1]]);

        free(g1);
        free(g2);
        g1 = strdup(parents[0]->getGeno().c_str());
        g2 = strdup(parents[1]->getGeno().c_str());

        delete parents[0];
        delete parents[1];
        return GENOPER_OK;
    }
};

#endif //CPP_FS_OPER_H

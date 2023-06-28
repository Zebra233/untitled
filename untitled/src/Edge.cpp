#include "../include/Edge.h"

using namespace std;

Edge::Edge(int id, double p, double EstTaskVolume, double aa, double bb) : eId(id), P_e(p), EstTaskVolume(EstTaskVolume), aa(aa), bb(bb) {

}


int Edge::getFirst() {
    if (preferForCloudVec.empty()) {
        return -1;
    }
    return preferForCloudVec[0].first;
}
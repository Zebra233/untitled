#include "../include/Cloud.h"
#include "../include/Edge.h"
#include <algorithm>

using namespace std;

Cloud::Cloud(int id, double p, double f, double e, double q) : cId(id), P_Cloud(p), F(f), E(e), Q(q) {

}


// @brief: 加入一个 edge 到 edgesOnMatching 中，并保证从大到小到顺序
// @param: Edge id
// @ret: void
void Cloud::addEdgesOnMatching(int eId) {
    if (this->edgesOnMatching.size() == 0)
        this->edgesOnMatching.push_back(eId);
    else {
        vector<int>::iterator it;
        for (it = this->edgesOnMatching.begin(); it != this->edgesOnMatching.end(); it++) {
//            cout << it - this->edgesOnMatching.begin() << endl;
//            cout << this->preferForEdge[eId] << " " << this->preferForEdge[it - this->edgesOnMatching.begin()] << endl;
            if (this->preferForEdge[eId] >= this->preferForEdge[it - this->edgesOnMatching.begin()]) {
                // cout << "cloud id " << this->cId << " insert " << eId << " before " << *it << endl;
                break;
            }
        }
        this->edgesOnMatching.emplace(it, eId);
    }
}


int Cloud::getLast() {
    return this->edgesOnMatching.back();
}

// @brief: 接收到一个 Edge 请求后，是否接受。如果接受修改并排序 edgesOnMatching 列表
// @param: Edge id
// @ret: 如果接受返回被删除的 eId(容量未满返回 -1)，不接受返回 -2
/*
1. 通过 getLast() 获得排最后的那个 edgeLast
2. 通过 preferForEdge 获取 edgeLast 和 edge偏好值
3. 比较偏好值
4. 拒绝或者接受，返回相应的返回值。接受修改并排序 edgesOnMatching 列表。
5. 拒绝的话要修改 edge 的匹配上的云为空
*/
int Cloud::recvEdgeQuest(int eId) {
    // cloud 服务容量未满
    if (this->edgesOnMatching.size() < this->Q) {
         //cout << "cloud 服务容量未满" << endl;
        this->addEdgesOnMatching(eId);
        return -1;
    }

    // cloud 服务容量已满
     //cout << "cloud 服务容量已满" << endl;
    int edgeLast = this->getLast();
    double preferLast = this->preferForEdge[edgeLast];
    double preferNew = this->preferForEdge[eId];
    // 如果新的偏好值大于最后一个偏好值，那么替换最后一个
    if (preferNew > preferLast) {
        // 先删除最后一个
         //cout << "删除最后一个 " << edgeLast << endl;
        //v1
//        int removedeId = this->edgesOnMatching.at(edgesOnMatching.size() - 1);
//        this->edgesOnMatching.erase(this->edgesOnMatching.end() - 1);

        // v2
        auto removedPos = find(this->edgesOnMatching.begin(), this->edgesOnMatching.end(), edgeLast);
        this->edgesOnMatching.erase(removedPos);

        // 再加入新的
         //cout << "加入新的 " << eId << endl;
        this->addEdgesOnMatching(eId);
        return edgeLast;
    }
    return -2;
}
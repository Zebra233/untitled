#include "include/Edge.h"
#include "include/Cloud.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <unistd.h>

using namespace std;

// init
map<int, Cloud> clouds;
map<int, Edge> edges;

// 是否存在迭代过程
bool flag = false;

// @brief: 计算得到云对于 Edge id 的偏好
// @param: Cloud Edge
// @ret: Cloud 对于传入 Edge 对象的偏好值
double getPreferCtoE(const Cloud &cloud, const Edge &edge) {
    return (cc * cloud.P_Cloud - dd * cloud.E) * edge.EstTaskVolume;
};

// @brief: 计算得到 Edge 对于 Cloud 的初始偏好，即价格
// @param: Edge Cloud
// @ret: Edge 对于传入 Cloud 对象的初始偏好值
double getPreferEtoCInit(const Edge &edge, const Cloud &cloud) {
    return -cloud.P_Cloud;
};

// @brief: 计算得到 Edge 对于 Cloud 的偏好
// @param: Edge Cloud
// @ret: Edge 对于传入 Cloud 对象的偏好值
double getPreferEtoC(const Edge &edge, const Cloud &cloud) {
    return edge.aa * (cloud.F / edge.EstTaskVolume) + edge.bb * ( 1 / cloud.P_Cloud)  - 0.0005 * cloud.edgesOnMatching.size() / cloud.Q;
};

// @brief: 更新云对于所有 Edge id 的偏好值 preferForEdge
// @param: int [] edge id 列表
// @ret: void
/*
1. 通过 preferForEdge(Edge edge) 获得每一个云对于每一个 edge 的偏好值
2. 插入 map 中
*/
void updatePreForEdge(Cloud &cloud) {
    for (auto e : edges) {
        int id = e.first;
        double value = getPreferCtoE(cloud, e.second);
        //cout << "updatePreForEdge cId: " << cloud.cId << " eId: " << id << " value: " << value << endl;
        //cloud.preferForEdge.insert(make_pair(id, value));
        cloud.preferForEdge[id] = value;

        cloud.preferForEdgeVec.push_back(make_pair(id, value));
    }
    sort(cloud.preferForEdgeVec.begin(), cloud.preferForEdgeVec.end(), Cloud::cmp_value);
}

// @brief: edge 对于所有 cloud id 的偏好值 preferForCloud
// @param: int [] cloud id 列表
// @ret: void
void updatePreForCloudInit(Edge &edge) {
    for (auto c : clouds) {
        int id = c.first;
        double value = getPreferEtoCInit(edge, c.second);
        //cout << "updatePreForCloudInit cId: " << c.second.cId << " eId: " << id << " value: " << value << endl;
        //cloud.preferForEdge.insert(make_pair(id, value));
        edge.preferForCloud[id] = value;

        edge.preferForCloudVec.push_back(make_pair(id, value));
    }
    sort(edge.preferForCloudVec.begin(), edge.preferForCloudVec.end(), Edge::cmp_value);
}

// @brief: 更新 edge 对于所有 cloud id 的偏好值 preferForCloud
// @param: int [] cloud id 列表
// @ret: void
void updatePreForCloud(Edge &edge) {
    edge.preferForCloud.clear();
    edge.preferForCloudVec.clear();
    for (auto c : clouds) {
        int id = c.first;
        double value = getPreferEtoC(edge, c.second);
        //cout << "cId: " << cloud.cId << " eId: " << id << " value: " << value << endl;
        //cloud.preferForEdge.insert(make_pair(id, value));
        edge.preferForCloud[id] = value;
        //cout << &cloud << endl;
        edge.preferForCloudVec.push_back(make_pair(id, value));
    }
    sort(edge.preferForCloudVec.begin(), edge.preferForCloudVec.end(), Edge::cmp_value);
}

// 是否所有 edge 都匹配上了或者偏好列表为空
bool isAllEdgeMatchedOrNull() {
    for (auto e : edges) {
        // 没匹配上并且偏好列表非空
        if (e.second.cloudOnMatching == -1 && !e.second.preferForCloud.empty()) {
            // cout << "edge " << e.first << " 未匹配，且偏好列表中有 ";
//            for (auto c : e.second.preferForCloud) {
//                cout <<  "cloud " << c.first << " ";
//            }
//            cout << endl;
            return false;
        }
    }
    return true;
}

// 是否 edge 更偏好于 cloud， cloud 也更偏好于 edge
bool isExistEdgeCloudBlock(Edge &edge, Cloud &cloud) {
    // 如果 edge 偏好列表中没有 cloud 那么肯定不是破坏对了
    if (edge.preferForCloud.find(cloud.cId) == edge.preferForCloud.end()) {
        return false;
    }

    // 分情况
    /*
     * 1. cloud 没匹配上任何 edge 那么默认 cloud 会更偏好于 edge
     * 2. cloud 存在匹配上的 edge 那么 edge 和 cloud 匹配队列中最后一个比较
     * 3. edge 没匹配上
     */
    if (cloud.edgesOnMatching.empty()) {
        if (edge.cloudOnMatching != -1) {
            return edge.preferForCloud[cloud.cId] > edge.preferForCloud[edge.cloudOnMatching];
        } else {
            return true;
        }
    } else {
        if (edge.cloudOnMatching != -1) {
            return edge.preferForCloud[cloud.cId] > edge.preferForCloud[edge.cloudOnMatching]
                   && cloud.preferForEdge[edge.eId] > cloud.preferForEdge[cloud.getLast()];
        } else {
            return cloud.preferForEdge[edge.eId] > cloud.preferForEdge[cloud.getLast()];
        }
    }
    return false;
}


// @brief: 是否存在 edge i 更偏好于 cloud j， cloud j 也更偏好于 edge i
// @param: void
// @ret: map<int, int> 云对于边的匹配
map<int, int> getBlockPair() {
    map<int, int> blockPair;
    for (auto e : edges) {
        for (auto c : clouds) {
            // 如果 edge 已经匹配上了 cloud 那么不可能是破坏对
            if (e.second.cloudOnMatching == c.second.cId) {
                continue;
            }
            // 如果 edge 没有匹配上 cloud 并且 cloud 匹配队列未满，那么一定是破坏对
            if (e.second.cloudOnMatching == -1 && (double)c.second.edgesOnMatching.size() < c.second.Q) {
                blockPair.insert(make_pair(e.first, c.first));
                continue;
            }
            if (isExistEdgeCloudBlock(e.second, c.second)) {
                blockPair.insert(make_pair(e.first, c.first));
            }
        }
    }
    return blockPair;
}


// 输出所有 Cloud 对所有 Edge 的偏好
void printPreferForEdge() {
    cout << "---------- 所有 Cloud 对所有 Edge 的偏好 ----------" << endl;
    for (map<int, Cloud>::iterator it = clouds.begin(); it != clouds.end(); it++) {
        cout << "cloud id: " << it->first << ":" << endl;
        for (auto it2 = it->second.preferForEdge.begin();
             it2 != it->second.preferForEdge.end(); it2++) {
            cout << "edge id: " << it2->first << " value: " << it2->second << endl;
        }
        cout << endl;
    }
}

// 输出有序的 cloud 对所有 edge 的偏好
void printPreferForEdgeVec() {
    cout << "---------- 有序的所有 Cloud 对所有 Edge 的偏好 ----------" << endl;
    for (map<int, Cloud>::iterator it = clouds.begin(); it != clouds.end(); it++) {
        cout << "cloud id: " << it->first << ":" << endl;
        for (auto it2 = it->second.preferForEdgeVec.begin();
             it2 != it->second.preferForEdgeVec.end(); it2++) {
            cout << "edge id: " << it2->first << " value: " << it2->second << endl;
        }
        cout << endl;
    }
}

// 输出所有 Edge 对所有 Cloud 的偏好
void printPreferForCloud() {
    cout << "----------- 输出所有 Edge 对所有 Cloud 的偏好 ----------" << endl;
    for (map<int, Edge>::iterator it = edges.begin(); it != edges.end(); it++) {
        cout << "edge id: " << it->first << ":" << endl;
        for (auto it2 = it->second.preferForCloud.begin();
             it2 != it->second.preferForCloud.end(); it2++) {
            cout << "cloud id: " << it2->first << " value: " << it2->second << endl;
        }
        cout << endl;
    }
}


// 输出有序的 Edge 对所有 Cloud 的偏好
void printPreferForCloudVec() {
    cout << "---------- 有序的所有 Edge 对所有 Cloud 的偏好 ----------" << endl;
    for (map<int, Edge>::iterator it = edges.begin(); it != edges.end(); it++) {
        cout << "edge id: " << it->first << ":" << endl;
        for (auto it2 = it->second.preferForCloudVec.begin();
             it2 != it->second.preferForCloudVec.end(); it2++) {
            cout << "cloud id: " << it2->first << " value: " << it2->second << endl;
        }
        cout << endl;
    }
}

// 输出有序的单个 edge 对于所有 cloud 的偏好
void printPreferForCloudVecJusteId(int eId) {
    cout << "---------- edge " << eId << " 对所有 cloud 的偏好 ----------" << endl;
    for (auto it2 = edges.find(eId)->second.preferForCloudVec.begin();
         it2 != edges.find(eId)->second.preferForCloudVec.end(); it2++) {
        cout << "cloud " << it2->first << " value: " << it2->second << endl;
    }
    cout << endl;
}

// 输出所有 edge 的匹配
void printEdgeOnMatching() {
    cout << "---------- 输出所有 edge 的匹配 ----------" << endl;
    for (map<int, Edge>::iterator it = edges.begin(); it != edges.end(); it++) {
        cout << "edge id: " << it->first << " cloud id: " << it->second.cloudOnMatching << endl;
    }
    cout << endl;
}

// 输出所有 cloud 的匹配
void printCloudOnMatching() {
    cout << "---------- 输出所有 cloud 的匹配 ----------" << endl;
    for (auto &cloud : clouds) {
        for (auto it2 = cloud.second.edgesOnMatching.begin(); it2 != cloud.second.edgesOnMatching.end(); it2++) {
            cout << "cloud id: " << cloud.first << " edge id: " << *it2 << endl;
        }
        cout << endl;
    }
}

// 加入新的 Edge 到 Cloud 的匹配列表 EdgesOnMatching
void addNewEdgeToMatch() {
    for (map<int, Cloud>::iterator it = clouds.begin(); it != clouds.end(); it++) {
        it->second.addEdgesOnMatching(0);
        it->second.addEdgesOnMatching(1);
    }

    for (auto i : clouds.begin()->second.edgesOnMatching) {
        cout << i << " ";
    }
}

// 测试云是否接受 Edge 的请求
void testRecvEdgeQuest() {
    if (clouds.begin()->second.recvEdgeQuest(2)) {
        cout << "recv success" << endl;
    }
    for (auto i : clouds.begin()->second.edgesOnMatching) {
        cout << i << " ";
    }
}


// @brief: 在 edge 发送请求，cloud 处理后，更新 edge 的匹配对象和偏好列表
// @param: recvEdgeQuest 的 返回值 re (表示是否接受，cloud 剔除了哪个 edge); 接受请求的 cloud id; 发送请求的 edge id
// @ret: map<int, int> 云对于边的匹配
void updatePreAftRecv(int re, int cId, int eId) {
    auto it = edges.find(eId);
    if (re >= -1) {
        // 接受请求

        // 还需要从原来的匹配的 cloud 的匹配列表中剔除 eId
        int OriginalCloud = it->second.cloudOnMatching;
        // 存在原来的匹配的 cloud
        if (OriginalCloud > -1) {
            vector<int>::iterator OriginalPos = find(clouds.find(OriginalCloud)->second.edgesOnMatching.begin(),
                                                     clouds.find(OriginalCloud)->second.edgesOnMatching.end(), eId);
            clouds.find(OriginalCloud)->second.edgesOnMatching.erase(OriginalPos);
        }

        it->second.cloudOnMatching = cId;
        //cout << "cloud " << cId << " 接受了 edge " << it->first << endl;

        // re > -1 说明 cloud 拒绝了另一个 edge
        if (re > -1) {
            Edge &removedEdge = edges.find(re)->second;
            removedEdge.cloudOnMatching = -1;
            pair<int, double> removePair = make_pair(cId, removedEdge.preferForCloud[cId]);
            vector<pair<int, double>>::iterator pos = find(removedEdge.preferForCloudVec.begin(),
                                                           removedEdge.preferForCloudVec.end(), removePair);
            removedEdge.preferForCloud.erase(cId);
            removedEdge.preferForCloudVec.erase(pos);
            //cout << "cloud " << cId << " 拒绝了 edge " << re << endl;
            //cout << "edge " << re << "更新后的偏好列表: "  << endl;
            //printPreferForCloudVecJusteId(re);
        }
    } else if (re == -2) {
        // 拒绝请求
        // it->second.cloudOnMatching = -1; // 没有被当前对象拒绝，不需要修改当前匹配对象
        pair<int, double> removePair = make_pair(cId, it->second.preferForCloud[cId]);
        vector<pair<int, double>>::iterator pos = find(it->second.preferForCloudVec.begin(),
                                                       it->second.preferForCloudVec.end(), removePair);
        it->second.preferForCloud.erase(cId);
        it->second.preferForCloudVec.erase(pos);
        //cout << "cloud " << cId << " 拒绝了 edge " << it->first << endl;
        //cout << "edge " << it->first << "更新后的偏好列表: "  << endl;
        //printPreferForCloudVecJusteId(it->first);
    }
}

// RIMA 初始化
void RIMAInit() {

    // 得到所有 cloud 对于所有 edge 的偏好值 preferForEdge
    for (map<int, Cloud>::iterator it = clouds.begin(); it != clouds.end(); it++) {
        updatePreForEdge(it->second);
    }

    // 得到所有 edge 对于所有 cloud 的初始偏好值 (price) preferForCloud
    for (map<int, Edge>::iterator it = edges.begin(); it != edges.end(); it++) {
        updatePreForCloudInit(it->second);
    }

    cout << "---------- 初始偏好 ----------" << endl;
    printPreferForEdgeVec();
    printPreferForCloudVec();

    // 比较有序的和无序的差别
    // 结论：没有差别
//    printPreferForCloud();
//    printPreferForCloudVec();
//    printPreferForEdge();
//    printPreferForEdgeVec();



    // 退出条件：所有 edge 都匹配上了
    while (!isAllEdgeMatchedOrNull()) {
        for (map<int, Edge>::iterator it = edges.begin(); it != edges.end(); it++) {
            //cout << "---------------edge " << it->first << "---------------" << endl;
            if (it->second.cloudOnMatching != -1 || it->second.preferForCloud.empty()) {
                //cout << "edge " << it->first << " 已经有匹配上的 cloud 了或者其偏好列表为空" << endl;
                continue;
            }

            //cout << "edge " << it->first << " 没有匹配上的 cloud 并且其偏好列表非空" << endl;

            // 得到 edge 排第一的 cloud
            int cId = it->second.getFirst();
            //cout << "edge " << it->first << " 最偏好的 cloud " << cId << endl;

            // 发送请求
            int re = clouds.find(cId)->second.recvEdgeQuest(it->first);
            //cout << "edge " << it->first << " 向 cloud " << cId << "发送服务请求"<< endl;
            updatePreAftRecv(re, cId, it->first);
        }
    }

}


// v1 edge 有更偏好的 cloud 就发送请求，等待 cloud 接受或者拒绝
//void RIMA_v1() {
//
//    RIMAInit();
//    cout << "----------- RIMA init 结束 -----------" << endl;
//    printEdgeOnMatching();
//    printCloudOnMatching();
//
//    for (map<int, Edge>::iterator it = edges.begin(); it != edges.end(); it++) {
//        // 先清除原来的偏好值
//        it->second.preferForCloud.clear();
//        it->second.preferForCloudVec.clear();
//        updatePreForCloud(it->second);
//    }
//
//    map<int, int> blockPair = getBlockPair();
//
//    /* 迭代主体
//        1. 循环
//        2. 更新 edge 偏好 排序
//        3. 看有没有更偏好的 cloud
//            发送请求
//            cloud 是否接受
//    */
//    // 迭代次数
//    int times = 0;
//    cout << "********** RIMA 迭代开始 **********" << endl;
//    do {
//        cout << "********** 第 " << ++times << " 次 RIMA 迭代开始" << endl;
//        printPreferForCloudVec();
//        cout << "-------- 第 " << times << " 次迭代中存在的 blockPair ----------" << endl;
//        for (auto i : blockPair) {
//            cout << "破坏对中 edge " << i.first << " cloud " << i.second << endl;
//        }
//
//        for (map<int, Edge>::iterator it = edges.begin(); it != edges.end(); it++) {
//            // 当前 edge 匹配上的 cloud
//            int curC = it->second.cloudOnMatching;
//
//            // 当前没有匹配上 cloud 直接按偏好发送请求
//            if (curC == -1) {
//                // cout << "edge id: " << it->first << " has matched or preferForCloud is empty" << endl;
//                // 得到 edge 排第一的 cloud
//                if (!it->second.preferForCloud.empty()) {
//                    int cId = it->second.getFirst();
//                    // cout << "edge id: " << it->first << " first cloud id: " << cId << endl;
//                    int re = clouds.find(cId)->second.recvEdgeQuest(it->first);
//                    updatePreAftRecv(re, cId, it->first);
//                }
//            } else {
//                // 存在匹配上的 cloud
//                // 看有没有更偏好的 cloud
//                for (vector<pair<int, double>>::iterator it2 = it->second.preferForCloudVec.begin();
//                     it2 != it->second.preferForCloudVec.end(); it2++) {
//
//                    // 不是当前匹配的 cloud 且 edge 对于这个 cloud 的偏好值更高
//                    if (it2->first != curC && it2->second > it->second.preferForCloud[curC]) {
//                        // cout << "edge id: " << it->first << " has not matched and preferForCloud is not empty" << endl;
//
//                        // 得到 edge 更偏好的 cloud
//                        int cId = it2->first;
//                        //cout << "edge " << it->first << " 更加偏好于 cloud " << cId << endl;
//
//                        // 发送请求
//                        int re = clouds.find(cId)->second.recvEdgeQuest(it->first);
//                        updatePreAftRecv(re, cId, it->first);
//
//                        // 如果被接受了，就不需要继续遍历了
//                        if (re >= -1)
//                            break;
//                    }
//                }
//            }
//        }
//
//        // 输出所有 cloud 的匹配情况
//        printEdgeOnMatching();
//
//        // 更新所有 edge 对于所有 cloud 的偏好值 preferForCloud
//        for (map<int, Edge>::iterator it = edges.begin(); it != edges.end(); it++) {
//            // 先清除原来的偏好值
//            it->second.preferForCloud.clear();
//            it->second.preferForCloudVec.clear();
//            updatePreForCloud(it->second);
//        }
//
//        // 更新破坏对
//        blockPair = getBlockPair();
//        if (blockPair.empty())
//            cout << "第 " << times << " 次迭代中没有产生破坏对" << endl;
////        cout << "blockPiar 数量 " << blockPair.size() << endl;
////        cout << "-------- 第 " << times << " 次迭代中产生的 blockPair----------" << endl;
////        for (auto i : blockPair) {
////            cout << "破坏对中 edge " << i.first << " cloud " << i.second << endl;
////        }
//
//
//    } while (!blockPair.empty() && times <= 10); // 破坏对为空则结束
//
//    cout << "-------- RIMA 结束 --------" << endl;
//}

// v2 只考虑破坏对中的 edge，破坏对中的 edge 发送请求给破坏对中的 cloud
// 返回迭代次数
int RIMA_v2() {

    RIMAInit();
    cout << "----------- RIMA init 结束 -----------" << endl;
    printEdgeOnMatching();
    printCloudOnMatching();



    for (map<int, Edge>::iterator it = edges.begin(); it != edges.end(); it++) {
        // 先清除原来的偏好值
        it->second.preferForCloud.clear();
        it->second.preferForCloudVec.clear();
        updatePreForCloud(it->second);
    }

    map<int, int> blockPair = getBlockPair();

    /* 迭代主体
        1. 循环
        2. 更新 edge 偏好 排序
        3. 看有没有更偏好的 cloud
            发送请求
            cloud 是否接受
    */
    // 迭代次数
    int times = 0;

    cout << "********** RIMA 迭代开始 **********" << endl;

    do {
        if (times > 100)
            return times;

        cout << "********** 第 " << ++times << " 次 RIMA 迭代开始" << endl;
        //printPreferForCloudVec();

        cout << "-------- 第 " << times << " 次迭代中存在的 blockPair --------" << endl;
        for (auto i : blockPair) {
            cout << "破坏对中 edge " << i.first << " cloud " << i.second << endl;
        }


        for (map<int, int>::iterator it = blockPair.begin(); it != blockPair.end(); it++) {
            // 破坏对中的 edge
            int eId = it->first;
            // 破坏对中的 cloud
            int cId = it->second;

            // 破坏对中的 edge 给破坏对中的 cloud 发送请求
            // cout << "edge " << eId << " 向 cloud " << cId << " 发送请求" << endl;
            int re = clouds.find(cId)->second.recvEdgeQuest(eId);
            updatePreAftRecv(re, cId, eId);
        }

        // 输出所有 cloud 的匹配情况
        //printEdgeOnMatching();

        // 更新所有 edge 对于所有 cloud 的偏好值
        for (map<int, Edge>::iterator it = edges.begin(); it != edges.end(); it++) {
            // 先清除原来的偏好值
            it->second.preferForCloud.clear();
            it->second.preferForCloudVec.clear();
            updatePreForCloud(it->second);
        }

        printPreferForCloud();

        // 更新破坏对
        blockPair = getBlockPair();
        if (blockPair.empty())
            cout << "第 " << times << " 次迭代中没有产生破坏对" << endl;

    } while (!blockPair.empty()); // 破坏对为空则结束

    cout << "-------- RIMA 结束 --------" << endl;
    cout << "共迭代 " << times << " 次" << endl;

    return times;
}



int main() {
    int flag = true;
        int numberOfIteration = 0;
        int runTimes = 1;
        while(runTimes <= 1000){
            srand((unsigned) time(NULL));

            // 初始化
            clouds.clear();
            edges.clear();

            // 生成前一半价格低 速度慢的
            int i = 0;
            for (i = 0; i < NumOfCloud / 2; i++) {
                // Cloud c(i, rand() % 9 + 1, rand() % 9 + 1, rand() % 9 + 1, NumOfEdge / NumOfCloud);
                double p = double(rand() % 10 / 10) + 2.5;
                Cloud c(i, p, double(rand() % 100) + 550, p * 0.8, 2);
                clouds.insert(make_pair(c.cId, c));
            }
            // 后一半价格高 速度快的
            for (; i < NumOfCloud; i++) {
                // Cloud c(i, rand() % 9 + 1, rand() % 9 + 1, rand() % 9 + 1, NumOfEdge / NumOfCloud);
                double p = double(rand() % 10 / 10) + 4.5;
                Cloud c(i, p, double(rand() % 100) / 100 + 1150, p * 0.8, 2);
                clouds.insert(make_pair(c.cId, c));
            }

            // 一半偏好低延迟的
            for (i = 0; i < NumOfEdge / 2; i++) {
                Edge e(i, rand() % 10, 1200, double(rand() % 10 / 10) + 9.5, double(rand() % 10 / 10) + 19.5);
                edges.insert(make_pair(e.eId, e));
            }
            // 一半偏好低价格的
            for (; i < NumOfEdge; i++) {
                Edge e(i, rand() % 10, 1200, double(rand() % 10 / 10) + 4.5, double(rand() % 10 / 10) + 29.5);
                edges.insert(make_pair(e.eId, e));
            }

            // 测试 RIMA
            int times1 = RIMA_v2();
            if (times1 > 100)
                continue;

            runTimes++;
            numberOfIteration += times1;

            printEdgeOnMatching();
            printCloudOnMatching();
//        if (flag) {
////            // 输出 cloud 属性值
////            cout << "********** cloud 属性值 **********" << endl;
////            for (map<int, Cloud>::iterator it = clouds.begin(); it != clouds.end(); it++) {
////                cout << "cloud " << it->second.cId << " p " << it->second.P_Cloud << " f " << it->second.F << " e " << it->second.E << endl;
////            }
////            // 输出 edge 属性值
////            cout << "********** edge 属性值 **********" << endl;
////            for (map<int, Edge>::iterator it = edges.begin(); it != edges.end(); it++) {
////                cout << "edge " << it->second.eId << " p " << it->second.P_e << " EstTaskVolume " << it->second.EstTaskVolume << endl;
////            }
//            break;
//        }


//    }
        }
        cout << "AVG Number of iteration " << numberOfIteration / runTimes << endl;

    return 0;
}
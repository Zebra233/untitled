#include "include/Edge.h"
#include "include/Cloud.h"


using namespace std;


//void updatePre(map<int, Cloud> clouds, map<int, Edge> edges) {
//    // 更新所有 Cloud 对于 Edge 的偏好
//    for (map<int, Cloud>::iterator it = clouds.begin(); it != clouds.end(); it++) {
//        updatePreForEdge(it->second);
//    }
//
//    for (map<int, Edge>::iterator it = edges.begin(); it != edges.end(); it++) {
//        updatePreForCloud(it->second);
//    }
//
//    // 输出所有 Cloud 对所有 Edge 的偏好
//    printPreferForEdge(clouds, edges);
//
//    // 输出所有 Edge 对所有 Cloud 的偏好
//    printPreferForCloud(clouds, edges);
//}

// 输出有序的 Cloud 对所有 Edge 的偏好
void printPreferForEdge(map<int, Cloud> clouds) {
    cout << "preferForEdgeVec" << endl;
    for (map<int, Cloud>::iterator it = clouds.begin(); it != clouds.end(); it++) {
        cout << "cloud id: " << it->first << ":" << endl;
        for (auto it2 = it->second.preferForEdge.begin();
             it2 != it->second.preferForEdge.end(); it2++) {
            cout << "edge id: " << it2->first << " value: " << it2->second << endl;
        }
        cout << "----------------------------------" << endl;
    }
}

// 输出有序的 Edge 对所有 Cloud 的偏好
void printPreferForCloud(map<int, Edge> edges) {
    cout << "preferForCloudVec" << endl;
    for (map<int, Edge>::iterator it = edges.begin(); it != edges.end(); it++) {
        cout << "edge id: " << it->first << ":" << endl;
        for (auto it2 = it->second.preferForCloud.begin();
             it2 != it->second.preferForCloud.end(); it2++) {
            cout << "cloud id: " << it2->first << " value: " << it2->second << endl;
        }
        cout << "----------------------------------" << endl;
    }
}

// 加入新的 Edge 到 Cloud 的匹配列表 EdgesOnMatching
void addNewEdgeToMatch(map<int, Cloud> clouds) {
    for (map<int, Cloud>::iterator it = clouds.begin(); it != clouds.end(); it++) {
        it->second.addEdgesOnMatching(0);
        it->second.addEdgesOnMatching(1);
    }

    for (auto i : clouds.begin()->second.edgesOnMatching) {
        cout << i << " ";
    }
}

// 测试云是否接受 Edge 的请求
void testRecvEdgeQuest(map<int, Cloud> clouds) {
    if (clouds.begin()->second.recvEdgeQuest(2)) {
        cout << "recv success" << endl;
    }
    for (auto i : clouds.begin()->second.edgesOnMatching) {
        cout << i << " ";
    }
}



//    Cloud c0(0, 6, 1, 1, 1);
//    Cloud c1(1, 8, 2, 2, 2);
//
//    clouds.insert(make_pair(0, c0));
//    clouds.insert(make_pair(1, c1));
//
//    Edge e0(0, 1, 1);
//    Edge e1(1, 2, 2);
//    Edge e2(2, 3, 3);
//    Edge e3(3, 4, 4);
//
//    edges.insert(make_pair(0, e0));
//    edges.insert(make_pair(1, e1));
//    edges.insert(make_pair(2, e2));
//    edges.insert(make_pair(3, e3));

//    for (map<int, Edge>::iterator it = edges.begin(); it != edges.end(); it++) {
//        updatePreForCloudInit(it->second);
//    }
//
//    for (map<int, Cloud>::iterator it = clouds.begin(); it != clouds.end(); it++) {
//        updatePreForEdge(it->second);
//    }


//    printPreferForEdge();
//    printCloudOnMatching();

//    int re = clouds.find(0)->second.recvEdgeQuest(edges.find(1)->second.eId);
//    updatePreAftRecv(re, 0, 1);
//
//    re = clouds.find(0)->second.recvEdgeQuest(edges.find(2)->second.eId);
//    updatePreAftRecv(re, 0, 2);
//
//
//    re = clouds.find(1)->second.recvEdgeQuest(edges.find(1)->second.eId);
//    updatePreAftRecv(re, 1, 1);
//
//    re = clouds.find(1)->second.recvEdgeQuest(edges.find(3)->second.eId);
//    updatePreAftRecv(re, 1, 3);
//
//    re = clouds.find(1)->second.recvEdgeQuest(edges.find(0)->second.eId);
//    cout << "re " << re << endl;
//    updatePreAftRecv(re, 1, 0);

//    printPreferForCloud();
//
//    printCloudOnMatching();
//




//    printEdgeOnMatching();
//    cout << "-------- 初始化中产生的 blockPair ----------" << endl;
//    for (auto i : blockPair) {
//        cout << "破坏对中 edge " << i.first << " cloud " << i.second << endl;
//        int EcurC = edges.find(i.first)->second.cloudOnMatching; // cId
//        int CcurE = clouds.find(i.second)->second.getLast(); // eId
//        cout << "edge 当前匹配的 cloud " << EcurC << " cloud 当前匹配的 edge " << CcurE << endl;
//        cout << "EtoC preference value : " << edges.find(i.first)->second.preferForCloud[i.second] << endl;
//        cout << "EtoCurC preference value : " << edges.find(i.first)->second.preferForCloud[EcurC] << endl;
//        cout << "CtoE preference value : " << clouds.find(i.second)->second.preferForEdge[i.first] << endl;
//        cout << "CtoCurE preference value : " << clouds.find(i.second)->second.preferForEdge[CcurE] << endl;
//    }


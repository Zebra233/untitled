//
// @brief: Cloud类设计
// @birth: created by zebra on 2023-05-26
// @version: V1
//

#pragma once
#include <iostream>
#include <map>
#include <vector>
#include "Edge.h"
#include "setting.h"

using namespace std;



// 云类
class Cloud
{
public:
    Cloud(int id, double p, double f, double e, double q);
    int cId;        // 云的 id
    double P_Cloud; // 云的价格
    double F;       // 云的计算速度
    double E;       // 云的能耗
    double Q;       // 云的容量限制

    // 有序的 map 来保存对于 Edge (int 为 Edge id) 的偏好
    map<int, double> preferForEdge;
    // 排序用
    vector<pair<int, double>> preferForEdgeVec;

    // todo
    // 输出两者进行比较

    //定义比较的函数
    static bool cmp_value(const pair<int, double> left, const pair<int,double> right) {
        return left.second > right.second;
    }

    // 云已经匹配上的 Edges id
    vector<int> edgesOnMatching;


    // @brief: 加入一个 edge 到 edgesOnMatching 中，并保证从大到小到顺序
    // @param: Edge id
    // @ret: void
    void addEdgesOnMatching(int eId);



    // @brief: 得到匹配列表中的排最后的 Edge id
    // @param: void
    // @ret: 非空返回 Edge id，空返回 -1
    int getLast();

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
    int recvEdgeQuest(int eId);
};
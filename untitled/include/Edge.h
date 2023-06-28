//
// @brief: Edge 类设计
// @birth: created by zebra on 2023-05-26
// @version: V1
// @revision: last revised by xxx on xxx
//
#pragma once
#include <iostream>
#include <map>
#include <vector>
#include "Cloud.h"
#include "setting.h"
#include "Contract.h"

using namespace std;

// 边缘类
class Edge
{
public:
    Edge(int id, double p, double EstTaskVolume, double aa, double bb);
    int eId;                  // 边缘 id
    Contract OptimalContract; // 设计得到的最优契约
    double P_e;               // 边缘价格
    double EstTaskVolume;     // 估计的需要卸载到云的任务量
    double aa;
    double bb;

    // 有序的 map 来保存对于 Cloud(int 为 Cloud id) 的偏好
    map<int, double> preferForCloud;
    // 排序用
    vector<pair<int, double>> preferForCloudVec;

    //定义比较的函数
    static bool cmp_value(const pair<int, double> left, const pair<int,double> right) {
        return left.second > right.second;
    }

    // edge 已经匹配上的云id
    int cloudOnMatching = -1;

    // // @brief: 计算得到 edge 对于云的偏好
    // // @param: Cloud id
    // // @ret: 对于传入 Cloud 对象的偏好值
    // // @birth: created by zebra on 20230526
    // double getPreferForCloud(int cId);

    // @brief: 得到偏好列表中排第一的 Cloud
    // @param: void
    // @ret: 非空返回 Cloud id，空返回 -1
    // @birth: created by zebra on 20230526
    int getFirst();
};
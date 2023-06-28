//
// @brief: User 类设计
// @birth: created by zebra on 2023-05-26
// @version: V1
// @revision: last revised by xxx on xxx
//
#pragma once
#include <iostream>
#include <map>
#include <vector>
#include "setting.h"

using namespace std;

// 用户类
class User
{
    User();
    ~User();
    double offloadAmount;    // t_k
    double delaySensitivity; // \beta_s
    double priceSensitivity; // \gamma_m
    double x;                // 选择的契约中的卸载到云的任务量
    double r;                // 选择的契约中的补贴
    double P_e;              // 边缘价格
};
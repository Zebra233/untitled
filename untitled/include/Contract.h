//
// @brief: 匹配契约角色类设计
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

// 契约类
class Contract
{
    double offloadAmount;    // t_k
    double delaySensitivity; // \beta_s
    double priceSensitivity; // \gamma_m
    double x;                // 卸载到云的任务量
    double r;                // 补贴
};





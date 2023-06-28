//
// @brief: 参数设置
// @birth: created by zebra on 2023-05-26
// @version: V1
// @revision: last revised by xxx on xxx
//
#pragma once

// 云 边 用户数量
#define NumOfCloud 16  // 云的数量
#define NumOfEdge 24 // 边缘数量
#define NumOfUser 100 // 用户数量可以指定，但是具体的用户类型信息应该按照分布随机生成

// 定义用户私有信息类型的数量
#define K 3 // 需要卸载的总的任务量
#define S 3 // 延迟敏感度类型数量
#define M 3 // 价格敏感度类型数量

// 匹配中的系数设置
//#define aa 1
//#define bb 5.5
#define cc 1
#define dd 1


#pragma once

namespace game::data {

/**
 * @brief 场景内的（非跨关卡）游戏资源及统计数据
 * 
 * 包含可用 cost、cost 生成速率、基地血量、敌人数量、敌人到达数量、敌人击杀数量等。
 */
struct GameStats
{
    float m_costAvailable{ 10.0F };   ///< @brief 可用 cost
    float m_costGenPerSecond{ 1.0F }; ///< @brief cost 生成速率
    int m_baseHp{ 5 };                ///< @brief 基地血量
    int m_totalEnemyCount{ 0 };       ///< @brief 敌人(总)数量
    int m_arrivedEnemyCount{ 0 };     ///< @brief 敌人到达数量
    int m_killedEnemyCount{ 0 };      ///< @brief 敌人击杀数量
};

} // namespace game::data

#pragma once

namespace game::component {

/**
 * @brief 属性组件
 * 用于存储角色的属性，包括生命值、攻击力、防御力、
 * 攻击范围、攻击间隔、攻击计时器、等级和稀有度。
 */
struct StatsComponent
{
    float m_hp{};
    float m_maxHp{};
    float m_atk{};
    float m_def{};
    float m_range{};       // 攻击范围（射程）
    float m_atkInterval{}; // 攻击间隔（决定攻速）
    float m_atkTimer{};    // 攻击计时器
    int m_level{ 1 };
    int m_rarity{ 1 }; // 稀有度，从1开始（例如1:普通，2:稀有，3:史诗，4:传说，5:神话...）
};

} // namespace game::component

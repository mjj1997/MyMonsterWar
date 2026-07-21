#pragma once

/* 蓝图结构体，为实体工厂提供数据 */
namespace game::data {

/// @brief 属性蓝图, 用于创建属性组件
struct StatsBlueprint
{
    float m_hp{ 0.0F };
    float m_atk{ 0.0F };
    float m_def{ 0.0F };
    float m_range{ 0.0F };
    float m_atkInterval{ 0.0F };
};

} // namespace game::data

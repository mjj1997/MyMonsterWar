#pragma once

#include <entt/entity/entity.hpp>

#include <string>

namespace game::component {

/**
 * @brief 职业名称组件。
 * 用于存储玩家职业（例如战士、法师、弓箭手） 或 
 * 敌人类型（例如史莱姆、狼、哥布林）的 ID 和名称
 */
struct ClassNameComponent
{
    entt::id_type m_classId{ entt::null };
    std::string m_className; // 可以是中文，主要用于显示
};

} // namespace game::component

#pragma once

#include "../defs/constants.h"

#include "../../engine/utils/math.h"

#include <entt/entity/entity.hpp>
#include <glm/vec2.hpp>

#include <string>
#include <unordered_map>
#include <vector>

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

/// @brief 精灵蓝图, 用于创建精灵组件
struct SpriteBlueprint
{
    entt::id_type m_textureId{ entt::null };
    std::string m_texturePath;
    engine::utils::Rect m_sourceRect{};
    glm::vec2 m_size{ 0.0F };
    glm::vec2 m_offset{ 0.0F };
    bool m_isFacedRight{ true }; ///< @brief 角色图片默认朝右，如果朝左就设置为false
};

/// @brief 单一动画的蓝图，多个蓝图构成的关联容器即可用于创建动画组件
struct AnimationBlueprint
{
    float m_durationPerFrame{ 0.0F };
    int m_row{ 0 };
    std::vector<int> m_frameIndices; ///< @brief 动画帧索引数组
    ///< @brief 动画关键帧事件，键为关键帧索引，值为事件 ID
    std::unordered_map<int, entt::id_type> m_events;
};

/// @brief 声音蓝图, 用于创建音频组件
struct SoundsBlueprint
{
    /// @brief 键为音效名称 id，值为音效路径 id
    std::unordered_map<entt::id_type, entt::id_type> m_sounds;
};

/// @brief 敌人蓝图, 用于创建敌人组件
struct EnemyBlueprint
{
    bool m_isRanged{ false };
    float m_speed{ 0.0F };
};

/// @brief 玩家蓝图, 用于创建玩家组件
struct PlayerBlueprint
{
    game::defs::PlayerType m_type{ game::defs::PlayerType::Unknown };
    entt::id_type m_skillId{ entt::null };
    bool m_isHealer{ false };
    int m_block{ 0 };
    int m_cost{ 0 };
};

/// @brief 显示信息蓝图, 可用于查找对应职业的名称和描述
struct DisplayInfoBlueprint
{
    std::string m_name;
    std::string m_description;
};

/// @brief 敌人类型蓝图, 包含所有必要的子蓝图，用于创建敌人实体中的所有组件
struct EnemyClassBlueprint
{
    entt::id_type m_classId{ entt::null };
    entt::id_type m_projectileId{ entt::null };
    std::string m_className;
    StatsBlueprint m_stats{};
    EnemyBlueprint m_enemy{};
    SoundsBlueprint m_sounds{};
    SpriteBlueprint m_sprite{};
    DisplayInfoBlueprint m_displayInfo{};
    std::unordered_map<entt::id_type, AnimationBlueprint> m_animations;
};

/// @brief 玩家类型蓝图, 包含所有必要的子蓝图，用于创建玩家实体中的所有组件
struct PlayerClassBlueprint
{
    entt::id_type m_classId{ entt::null };
    entt::id_type m_projectileId{ entt::null };
    std::string m_className;
    StatsBlueprint m_stats{};
    PlayerBlueprint m_player{};
    SoundsBlueprint m_sounds{};
    SpriteBlueprint m_sprite{};
    DisplayInfoBlueprint m_displayInfo{};
    std::unordered_map<entt::id_type, AnimationBlueprint> m_animations;
};

/// @brief 投射物蓝图, 用于创建投射物组件
struct ProjectileBlueprint
{
    entt::id_type m_id{ entt::null };
    std::string m_name;
    float m_arcHeight{};
    float m_totalFlightTime{};
    SoundsBlueprint m_sounds{};
    SpriteBlueprint m_sprite{};
};

} // namespace game::data

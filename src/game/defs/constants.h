#pragma once

namespace game::defs {

constexpr float BLOCK_RADIUS{ 40.0F }; // 阻挡判断半径
constexpr float UNIT_RADIUS{ 20.0F };  // 角色自身半径（相当于碰撞盒，用于计算攻击范围）

/// @brief 玩家类型枚举
enum class PlayerType {
    Unknown,
    Melee,  // 近战型，只能放在近战区域
    Ranged, // 远程型，只能放在远程区域
    Mixed   // 混合型，可以在放在任意区域（暂不实现，未来可拓展）
};

} // namespace game::defs

#pragma once

namespace game::defs {

/// @brief 玩家类型枚举
enum class PlayerType {
    Unknown,
    Melee,  // 近战型，只能放在近战区域
    Ranged, // 远程型，只能放在远程区域
    Mixed   // 混合型，可以在放在任意区域（暂不实现，未来可拓展）
};

} // namespace game::defs

#pragma once

#include <entt/entity/fwd.hpp>

#include <unordered_map>

namespace engine::component {

/**
 * @brief 音频组件，包含音效集合。
 */
struct AudioComponent
{
    ///< @brief 音效集合，键为音效名称 id，值为音效路径 id
    std::unordered_map<entt::id_type, entt::id_type> m_sounds;
};

} // namespace engine::component

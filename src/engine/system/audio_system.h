#pragma once

#include <entt/entity/fwd.hpp>

namespace engine::core {
class Context;
}

namespace engine::utils {
struct PlaySoundEvent;
}

namespace engine::system {

/**
 * @brief 音频系统，负责处理播放音频事件
 * 
 */
class AudioSystem
{
public:
    AudioSystem(entt::registry& registry, engine::core::Context& context);
    ~AudioSystem();

private:
    /* --- 回调函数 --- */
    /// @brief 播放音效事件的处理函数
    void playSound(const engine::utils::PlaySoundEvent& event);

    entt::registry& m_registry;
    engine::core::Context& m_context;
};

} // namespace engine::system

#pragma once

#include <entt/entity/fwd.hpp>

namespace engine::core {
class Context;
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
    entt::registry& m_registry;
    engine::core::Context& m_context;
};

} // namespace engine::system

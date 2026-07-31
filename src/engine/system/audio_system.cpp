#include "audio_system.h"

namespace engine::system {

AudioSystem::AudioSystem(entt::registry& registry, engine::core::Context& context)
    : m_registry{ registry }
    , m_context{ context }
{}

AudioSystem::~AudioSystem() {}

} // namespace engine::system

#include "projectile_system.h"
#include "../defs/events.h"

#include <spdlog/spdlog.h>

namespace game::system {

ProjectileSystem::ProjectileSystem(entt::registry& registry, entt::dispatcher& dispatcher)
    : m_registry{ registry }
    , m_dispatcher{ dispatcher }
{}

ProjectileSystem::~ProjectileSystem() {}

void ProjectileSystem::handleEmitProjectileEvent(const game::defs::EmitProjectileEvent& event)
{
    spdlog::info("发射投射物: {}", event.m_id);

    // TODO: 创建投射物实体
}

} // namespace game::system

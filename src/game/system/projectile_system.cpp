#include "projectile_system.h"
#include "../defs/events.h"
#include "../factory/entity_factory.h"

#include <spdlog/spdlog.h>

namespace game::system {

ProjectileSystem::ProjectileSystem(entt::registry& registry,
                                   entt::dispatcher& dispatcher,
                                   game::factory::EntityFactory& entityFactory)
    : m_registry{ registry }
    , m_dispatcher{ dispatcher }
    , m_entityFactory{ entityFactory }
{}

ProjectileSystem::~ProjectileSystem() {}

void ProjectileSystem::handleEmitProjectileEvent(const game::defs::EmitProjectileEvent& event)
{
    spdlog::info("发射投射物: {}", event.m_id);

    // 创建投射物实体
    m_entityFactory.createProjectile(event.m_id,
                                     event.m_startPosition,
                                     event.m_targetPosition,
                                     event.m_target,
                                     event.m_damage);
}

} // namespace game::system

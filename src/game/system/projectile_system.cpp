#include "projectile_system.h"
#include "../component/projectile_component.h"
#include "../defs/events.h"
#include "../defs/tags.h"
#include "../factory/entity_factory.h"

#include "../../engine/component/transform_component.h"
#include "../../engine/utils/events.h"

#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <glm/common.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <spdlog/spdlog.h>

using namespace entt::literals;

namespace game::system {

ProjectileSystem::ProjectileSystem(entt::registry& registry,
                                   entt::dispatcher& dispatcher,
                                   game::factory::EntityFactory& entityFactory)
    : m_registry{ registry }
    , m_dispatcher{ dispatcher }
    , m_entityFactory{ entityFactory }
{
    m_dispatcher.sink<game::defs::EmitProjectileEvent>()
        .connect<&ProjectileSystem::handleEmitProjectileEvent>(this);
}

ProjectileSystem::~ProjectileSystem()
{
    m_dispatcher.disconnect(this);
}

void ProjectileSystem::update(float deltaTime)
{
    // 获取所有投射物
    auto view = m_registry.view<game::component::ProjectileComponent,
                                engine::component::TransformComponent>();
    for (auto entity : view) {
        // 更新飞行时间
        auto& projectile = m_registry.get<game::component::ProjectileComponent>(entity);
        projectile.m_currentFlightTime += deltaTime;

        // 如果飞行时间超过总飞行时间，则命中目标（发送攻击事件并播放音效），然后销毁
        if (projectile.m_currentFlightTime >= projectile.m_totalFlightTime) {
            m_dispatcher.enqueue(
                game::defs::AttackEvent{ entity, projectile.m_target, projectile.m_damage });
            m_dispatcher.enqueue(engine::utils::PlaySoundEvent{ entity, "hit"_hs });
            m_registry.emplace<game::defs::DeadTag>(entity);
            continue;
        }

        // 计算飞行进度（time 从 0 到 1）
        float time{ projectile.m_currentFlightTime / projectile.m_totalFlightTime };
        time = glm::clamp(time, 0.0F, 1.0F); // 确保 time 在 [0, 1] 之间

        // 1. 计算水平位置（线性插值）
        glm::vec2 horizontalPosition{
            glm::mix(projectile.m_startPosition, projectile.m_targetPosition, time)
        };

        // 2. 计算垂直方向的弧线偏移
        float arcOffset{ glm::sin(time * glm::pi<float>()) * projectile.m_arcArcHeight };

        // 3. 合成最终位置
        auto& transform = m_registry.get<engine::component::TransformComponent>(entity);
        transform.m_position = horizontalPosition;
        transform.m_position.y -= arcOffset; // Y 轴向下为正方向，因此减去偏移使其向上拱起

        // 4. 根据上一帧的位置计算朝向，并更新变换组件的旋转参数
        auto direction = transform.m_position - projectile.m_previousPosition;
        transform.m_rotation = glm::atan(direction.y, direction.x) * 180.0F / glm::pi<float>();

        // 5. 更新上一帧的位置
        projectile.m_previousPosition = transform.m_position;
    }
}

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

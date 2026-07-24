#include "entity_factory.h"
#include "blueprint_manager.h"

#include <entt/entity/registry.hpp>

namespace game::factory {

EntityFactory::EntityFactory(entt::registry& registry, BlueprintManager& blueprintManager)
    : m_registry{ registry }
    , m_blueprintManager{ blueprintManager }
{}

entt::entity EntityFactory::createEnemyUnit(entt::id_type classId)
{
    auto entity = m_registry.create();
    const auto& blueprint = m_blueprintManager.getEnemyClassBlueprint(classId);

    /* --- 添加组件 --- */
    // TODO: 添加变换组件
    // TODO: 添加精灵组件
    // TODO: 添加动画组件
    // TODO: 添加音频组件
    // TODO: 添加属性组件
    // TODO: 添加敌人组件

    // TODO: 未来可添加其它组件

    return entity;
}

} // namespace game::factory

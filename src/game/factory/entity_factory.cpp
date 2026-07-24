#include "entity_factory.h"
#include "../data/entity_blueprint.h"
#include "blueprint_manager.h"

#include "../../engine/component/sprite_component.h"
#include "../../engine/component/transform_component.h"

#include <entt/entity/registry.hpp>

namespace game::factory {

EntityFactory::EntityFactory(entt::registry& registry, BlueprintManager& blueprintManager)
    : m_registry{ registry }
    , m_blueprintManager{ blueprintManager }
{}

entt::entity EntityFactory::createEnemyUnit(entt::id_type classId, glm::vec2 position)
{
    auto entity = m_registry.create();
    const auto& blueprint = m_blueprintManager.getEnemyClassBlueprint(classId);

    /* --- 添加组件 --- */
    // 添加变换组件
    addTransformComponent(entity, position);
    // 添加精灵组件
    addSpriteComponent(entity, blueprint.m_sprite);
    // TODO: 添加动画组件
    // TODO: 添加音频组件
    // TODO: 添加属性组件
    // TODO: 添加敌人组件

    // TODO: 未来可添加其它组件

    return entity;
}

void EntityFactory::addTransformComponent(entt::entity entity,
                                          glm::vec2 position,
                                          glm::vec2 scale,
                                          float rotation)
{
    m_registry.emplace<engine::component::TransformComponent>(entity, position, scale, rotation);
}

void EntityFactory::addSpriteComponent(entt::entity entity,
                                       const data::SpriteBlueprint& sprite,
                                       bool isFlipped)
{
    m_registry.emplace<engine::component::SpriteComponent>(entity,
                                                           engine::component::Sprite{
                                                               sprite.m_texturePath,
                                                               sprite.m_sourceRect,
                                                               isFlipped },
                                                           sprite.m_size,
                                                           sprite.m_offset);
    // TODO: 如果图片不是面向右侧, 添加 FaceLeftTag
}

} // namespace game::factory

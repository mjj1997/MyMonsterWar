#pragma once

#include <entt/entity/fwd.hpp>

namespace game::factory {

class BlueprintManager;

/**
 * @brief 实体工厂，用于创建不同类型的实体
 * 
 * 实体工厂通过蓝图管理器获取蓝图数据，并创建不同类型的实体。
 */
class EntityFactory
{
public:
    /// @brief 实体工厂构造函数, 需要传入注册表和蓝图管理器。通过蓝图数据创建不同实体
    EntityFactory(entt::registry& registry, BlueprintManager& blueprintManager);

private:
    entt::registry& m_registry;
    BlueprintManager& m_blueprintManager;
};

} // namespace game::factory

#pragma once

#include <entt/entity/entity.hpp>

#include <string>

namespace engine::component {

/**
 * @brief 名称组件，可用于标记实体名称。
 */
struct NameComponent
{
    entt::id_type m_nameId{ entt::null }; ///< @brief 名称ID
    std::string m_name;                   ///< @brief 名称
};

} // namespace engine::component

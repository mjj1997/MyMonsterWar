#pragma once

#include <entt/entity/fwd.hpp>

namespace engine::system {

/**
 * @brief y-sort 排序系统
 * 
 * 如果一个实体会移动，每帧根据变换组件的 y 坐标更新渲染组件的深度
 */
class YSortSystem
{
public:
    void update(entt::registry& registry);
};

} // namespace engine::system

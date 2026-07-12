#pragma once

namespace engine::component {

/**
 * @brief 渲染组件, 包含图层序号和实体深度。
 */
struct RenderComponent
{
    RenderComponent(int layerIndex = 0, float depth = 0.0F)
        : m_layerIndex{ layerIndex }
        , m_depth{ depth }
    {}

    // 重载比较运算符，用于排序
    bool operator<(const RenderComponent& other) const
    {
        if (m_layerIndex == other.m_layerIndex) { // 如果图层相同，则比较实体深度
            return m_depth < other.m_depth;
        }
        return m_layerIndex < other.m_layerIndex; // 如果图层不同，则比较图层序号
    }

    ///< @brief 图层序号，数字越小越先绘制
    int m_layerIndex{};
    ///< @brief 实体在同一图层内的深度，数字越小越先绘制
    float m_depth{}; /*  (可用于实现 y-sort 排序，也可设定其它渲染顺序逻辑) */

    // TODO: 未来可添加其他信息，比如透明度等 ...
};

} // namespace engine::component

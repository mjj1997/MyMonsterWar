#pragma once

namespace engine::component {

/**
 * @brief 渲染组件, 包含图层序号。
 */
struct RenderComponent
{
    RenderComponent(int layerIndex = 0)
        : m_layerIndex{ layerIndex }
    {}

    // 重载比较运算符，用于排序
    bool operator<(const RenderComponent& other) const
    {
        return m_layerIndex < other.m_layerIndex; // 如果图层不同，则比较图层序号
    }

    ///< @brief 图层序号，数字越小越先绘制
    int m_layerIndex{};

    // TODO: 未来可添加其他信息，比如透明度等 ...
};

} // namespace engine::component

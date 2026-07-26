#pragma once

namespace game::component {

/// @brief 阻挡者组件，存储阻挡者的最大阻挡数量和当前阻挡数量
struct BlockerComponent
{
    int m_maxCount{};
    int m_currentCount{};
};

} // namespace game::component

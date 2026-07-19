#include "entity_builder_mw.h"
#include "../data/path_node.h"

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

namespace game::loader {

EntityBuilderMW::EntityBuilderMW(engine::loader::LevelLoader& levelLoader,
                                 engine::core::Context& context,
                                 entt::registry& registry,
                                 std::unordered_map<int, game::data::PathNode>& pathNodes,
                                 std::vector<int>& startpointIds)
    : engine::loader::BasicEntityBuilder{ levelLoader, context, registry }
    , m_pathNodes{ pathNodes }
    , m_startpointIds{ startpointIds }
{}

EntityBuilderMW* EntityBuilderMW::build()
{
    if (m_objectJson != nullptr && m_tileInfo == nullptr) {
        // 代表自己绘制的形状,当前游戏只用到了路径节点
        buildPath();
    } else {
        BasicEntityBuilder::build();
    }

    return this;
}

void EntityBuilderMW::buildPath()
{
    // 检查数据有效性
    if (m_objectJson->value("point", false) != true) {
        return;
    }
    if (!m_objectJson->contains("properties") || !m_objectJson->at("properties").is_array()) {
        return;
    }
    auto id = m_objectJson->value("id", 0);
    if (id == 0) {
        return;
    }

    // 解析数据并添加到容器
    glm::vec2 position{ m_objectJson->value("x", 0.0F), m_objectJson->value("y", 0.0F) };
    std::vector<int> nextNodeIds;
    for (const auto& property : m_objectJson->at("properties")) {
        // 如果是对象类型，且名称以 next 开头，则添加到 nextNodeIds
        if (property.value("type", "") == "object"
            && property.value("name", "").starts_with("next")) {
            auto nextNodeId = property.value("value", 0);
            if (nextNodeId != 0) {
                nextNodeIds.push_back(nextNodeId);
            }
        }
        // 如果名称是 start，且值为真，则将自身id添加到 m_startpointIds 中
        if (property.value("name", "") == "start" && property.value("value", false) == true) {
            m_startpointIds.push_back(id);
        }
    }

    // 添加到节点容器中
    m_pathNodes.emplace(id, game::data::PathNode{ id, position, std::move(nextNodeIds) });

    spdlog::trace("pathNodes size: {}", m_pathNodes.size());
}

} // namespace game::loader

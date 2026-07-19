#pragma once

#include "../../engine/loader/basic_entity_builder.h"

#include <unordered_map>
#include <vector>

namespace game::data {
struct PathNode;
}

namespace game::loader {

/**
 * @brief 拓展的关卡载入实体生成器
 * 功能包括：
 * 1. 生成路径节点和起点。
 * 2. ...
 */
class EntityBuilderMW final : public engine::loader::BasicEntityBuilder
{
public:
    /**
     * @brief 构造函数
     * @param levelLoader 关卡载入器
     * @param context 上下文
     * @param registry 实体注册表
     * @param pathNodes 路径节点
     * @param startpointIds 起点ID列表
     */
    EntityBuilderMW(engine::loader::LevelLoader& levelLoader,
                    engine::core::Context& context,
                    entt::registry& registry,
                    std::unordered_map<int, game::data::PathNode>& pathNodes,
                    std::vector<int>& startpointIds);
    ~EntityBuilderMW() = default;

private:
    void buildPath(); ///< @brief 生成路径节点

    // 保存路径节点和起点数据（非拥有）
    std::unordered_map<int, game::data::PathNode>& m_pathNodes;
    std::vector<int>& m_startpointIds;
};

} // namespace game::loader

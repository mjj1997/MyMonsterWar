#include "entity_builder_mw.h"
#include "../data/path_node.h"

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

} // namespace game::loader

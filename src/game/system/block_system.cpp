#include "block_system.h"

#include <entt/entity/registry.hpp>
#include <spdlog/spdlog.h>

namespace game::system {

void BlockSystem::update(entt::registry& registry)
{
    spdlog::trace("BlockSystem::update");
}

} // namespace game::system

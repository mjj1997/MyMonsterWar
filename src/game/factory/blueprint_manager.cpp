#include "blueprint_manager.h"

namespace game::factory {

BlueprintManager::BlueprintManager(engine::resource::ResourceManager& resourceManager)
    : m_resourceManager{ resourceManager }
{}

} // namespace game::factory

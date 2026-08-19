#include "player_unit_portrait_ui.h"
#include "../data/entity_blueprint.h"
#include "../data/game_stats.h"
#include "../data/session_data.h"
#include "../data/ui_config.h"
#include "../factory/blueprint_manager.h"

#include "../../engine/core/context.h"
#include "../../engine/core/game_state.h"
#include "../../engine/ui/ui_button.h"
#include "../../engine/ui/ui_image.h"
#include "../../engine/ui/ui_label.h"
#include "../../engine/ui/ui_manager.h"
#include "../../engine/ui/ui_panel.h"

#include <entt/core/hashed_string.hpp>
#include <entt/entity/registry.hpp>
#include <spdlog/spdlog.h>

using namespace entt::literals;

namespace game::ui {

PlayerUnitPortraitUi::PlayerUnitPortraitUi(entt::registry& registry,
                                           engine::ui::UiManager& uiManager,
                                           engine::core::Context& context)
    : m_registry{ registry }
    , m_uiManager{ uiManager }
    , m_context{ context }
{
    spdlog::trace("PlayerUnitPortraitUi 构造完成");
}

PlayerUnitPortraitUi::~PlayerUnitPortraitUi() = default;

} // namespace game::ui

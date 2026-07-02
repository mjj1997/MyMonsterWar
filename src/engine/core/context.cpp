#include "context.h"

#include <spdlog/spdlog.h>

namespace engine::core {

Context::Context(engine::input::InputManager& inputManager,
                 engine::render::Renderer& renderer,
                 engine::render::Camera& camera,
                 engine::resource::ResourceManager& resourceManager,
                 engine::audio::AudioPlayer& audioPlayer,
                 engine::render::TextRenderer& textRenderer,
                 GameState& gameState,
                 entt::dispatcher& dispatcher)
    : m_inputManager{ inputManager }
    , m_renderer{ renderer }
    , m_camera{ camera }
    , m_resourceManager{ resourceManager }
    , m_audioPlayer{ audioPlayer }
    , m_textRenderer{ textRenderer }
    , m_gameState{ gameState }
    , m_dispatcher{ dispatcher }
{
    spdlog::trace("上下文已创建并初始化，包含输入管理器、渲染器、相机、资源引擎、音频播放器、文字渲"
                  "染引擎、游戏状态和事件分发器。");
}

} // namespace engine::core

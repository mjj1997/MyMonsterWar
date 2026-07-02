#include "engine/core/context.h"
#include "engine/core/game_app.h"
#include "engine/utils/events.h"
#include "game/scene/game_scene.h"

#include <entt/signal/dispatcher.hpp>
#include <spdlog/spdlog.h>

void setupInitialScene(engine::core::Context& context)
{
    // GameApp在调用run方法之前，先创建并设置初始场景
    auto initialScene = std::make_unique<game::scene::GameScene>(context);
    // 发送压入场景事件信号，将初始场景压入栈顶
    context.dispatcher().trigger<engine::utils::PushSceneEvent>(
        engine::utils::PushSceneEvent{ std::move(initialScene) });
}

int main(int /*argc*/, char* /*argv*/[])
{
    spdlog::set_level(spdlog::level::debug);

    engine::core::GameApp app;
    app.registerSceneSetupFunc(setupInitialScene);
    app.run();

    return 0;
}

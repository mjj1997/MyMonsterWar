#include "engine/core/context.h"
#include "engine/core/game_app.h"
#include "game/scene/game_scene.h"

#include <spdlog/spdlog.h>

void setupInitialScene(engine::core::Context& context)
{
    // GameApp在调用run方法之前，先创建并设置初始场景
    auto initialScene = std::make_unique<game::scene::GameScene>(context);
}

int main(int /*argc*/, char* /*argv*/[])
{
    spdlog::set_level(spdlog::level::debug);

    engine::core::GameApp app;
    app.registerSceneSetupFunc(setupInitialScene);
    app.run();

    return 0;
}

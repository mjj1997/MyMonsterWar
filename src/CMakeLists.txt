add_executable(MonsterWar)

target_sources(
  MonsterWar
  PRIVATE main.cpp
          engine/core/game_app.cpp
          engine/core/frame_time_controller.cpp
          engine/resource/texture_manager.cpp
          engine/resource/audio_manager.cpp
          engine/resource/font_manager.cpp
          engine/resource/resource_manager.cpp
          engine/render/camera.cpp
          engine/render/renderer.cpp
          engine/core/configurator.cpp
          engine/input/input_manager.cpp
          engine/core/context.cpp
          engine/scene/scene_base.cpp
          engine/scene/scene_manager.cpp
          engine/audio/audio_player.cpp
          engine/render/text_renderer.cpp
          engine/ui/ui_element_base.cpp
          engine/ui/ui_panel.cpp
          engine/ui/ui_manager.cpp
          engine/ui/ui_image.cpp
          engine/ui/ui_label.cpp
          engine/ui/ui_interactive_element_base.cpp
          engine/ui/state/ui_normal_state.cpp
          engine/ui/state/ui_hover_state.cpp
          engine/ui/state/ui_pressed_state.cpp
          engine/ui/ui_button.cpp
          engine/core/game_state.cpp
          game/scene/game_scene.cpp
          engine/system/movement_system.cpp
          engine/system/render_system.cpp
          engine/system/animation_system.cpp
          engine/loader/level_loader.cpp
          engine/loader/basic_entity_builder.cpp
          engine/system/y_sort_system.cpp
          game/loader/entity_builder_mw.cpp
          game/system/follow_path_system.cpp
          game/system/remove_dead_system.cpp
          game/factory/blueprint_manager.cpp
          game/factory/entity_factory.cpp
          game/system/block_system.cpp
          game/system/set_target_system.cpp
          game/system/timer_system.cpp
          game/system/attack_starter_system.cpp
          game/system/animation_state_system.cpp
          game/system/orientation_system.cpp
          game/system/combat_resolve_system.cpp)

target_link_libraries(
  MonsterWar
  PRIVATE SDL3::SDL3
          SDL3_image::SDL3_image
          SDL3_mixer::SDL3_mixer
          SDL3_ttf::SDL3_ttf
          glm::glm
          nlohmann_json::nlohmann_json
          spdlog::spdlog
          EnTT::EnTT
          imgui::imgui)

# 1. 定义资源目录
set(RESOURCE_SRC_DIR "${PROJECT_SOURCE_DIR}/assets")
set(RESOURCE_DST_DIR "$<TARGET_FILE_DIR:MonsterWar>/assets")

# 2. 创建自定义目标用于增量复制资源
add_custom_target(
  CopyAssets ALL
  COMMENT "Copying only changed asset files..."
  VERBATIM)

# 3. 创建自定义命令复制整个资源目录到输出目录
add_custom_command(
  TARGET CopyAssets
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different "${RESOURCE_SRC_DIR}" "${RESOURCE_DST_DIR}"
  COMMENT "Copying assets to output directory...")

# 4. 为可执行目标添加依赖
add_dependencies(MonsterWar CopyAssets)

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
          engine/object/game_object.cpp
          engine/core/context.cpp
          # engine/component/transform_component.cpp
          # engine/component/sprite_component.cpp
          engine/scene/scene_base.cpp
          engine/scene/scene_manager.cpp
          # engine/component/parallax_component.cpp
          # engine/scene/level_loader.cpp
          # engine/component/tilelayer_component.cpp
          engine/render/animation.cpp
          # engine/component/animation_component.cpp
          # engine/component/health_component.cpp
          engine/audio/audio_player.cpp
          # engine/component/audio_component.cpp
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
          game/scene/game_scene.cpp)

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

# 2. 使用 GLOB 递归获取源资源文件列表（推荐使用 CONFIGURE_DEPENDS）
file(
  GLOB_RECURSE RESOURCE_FILES CONFIGURE_DEPENDS
  RELATIVE "${RESOURCE_SRC_DIR}"
  "${RESOURCE_SRC_DIR}/*")

# 3. 创建自定义目标用于增量复制资源
add_custom_target(
  CopyAssets ALL
  COMMENT "Copying only changed asset files..."
  VERBATIM)

# 4. 为每个资源文件添加复制命令
foreach(RSRC_FILE IN LISTS RESOURCE_FILES)
  get_filename_component(RSRC_DIR "${RSRC_FILE}" DIRECTORY)

  # 为每个文件定义复制命令
  add_custom_command(
    TARGET CopyAssets
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${RESOURCE_DST_DIR}/${RSRC_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${RESOURCE_SRC_DIR}/${RSRC_FILE}" "${RESOURCE_DST_DIR}/${RSRC_FILE}"
    COMMENT "Checking: ${RSRC_FILE}"
    WORKING_DIRECTORY "${RESOURCE_SRC_DIR}" # 确保相对路径正确
  )
endforeach()

# 5. 为可执行目标添加依赖
add_dependencies(MonsterWar CopyAssets)

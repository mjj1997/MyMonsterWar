#pragma once

#include <entt/entity/entity.hpp>

#include <memory>

namespace engine::scene {

class SceneBase;

} // namespace engine::scene

namespace engine::utils {

struct QuitEvent // 退出事件
{};

struct PopSceneEvent // 弹出场景事件
{};

struct PushSceneEvent // 压入场景事件
{
    std::unique_ptr<engine::scene::SceneBase> scene;
};

struct ReplaceSceneEvent // 替换场景事件
{
    std::unique_ptr<engine::scene::SceneBase> scene;
};

struct PlayAnimationEvent // 播放动画事件
{
    entt::entity m_entity{ entt::null };       // 目标实体
    entt::id_type m_animationId{ entt::null }; // 动画 ID
    bool m_loop{ true };                       // 是否循环播放
};

} // namespace engine::utils

#include "animation_system.h"
#include "../component/animation_component.h"
#include "../component/sprite_component.h"
#include "../utils/events.h"

#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <spdlog/spdlog.h>

namespace engine::system {

AnimationSystem::AnimationSystem(entt::registry& registry, entt::dispatcher& dispatcher)
    : m_registry{ registry }
    , m_dispatcher{ dispatcher }
{
    m_dispatcher.sink<engine::utils::PlayAnimationEvent>().connect<&AnimationSystem::playAnimation>(
        this);
}

AnimationSystem::~AnimationSystem()
{
    m_dispatcher.disconnect(this);
}

void AnimationSystem::update(float deltaTime)
{
    spdlog::trace("AnimationSystem::update");

    auto view = m_registry.view<component::AnimationComponent, component::SpriteComponent>();
    for (auto entity : view) {
        auto& animationComponent = view.get<component::AnimationComponent>(entity);
        auto& spriteComponent = view.get<component::SpriteComponent>(entity);

        // 如果动画不存在，则跳过
        auto it = animationComponent.m_animations.find(animationComponent.m_currentAnimationId);
        if (it == animationComponent.m_animations.end()) {
            continue;
        }

        // 获取当前动画
        auto& currentAnimation = it->second;
        // 如果没有帧，则跳过
        if (currentAnimation.m_frames.empty()) {
            continue;
        }

        // 更新当前播放时间 (推进计时器)
        animationComponent.m_currentTime += deltaTime * 1000.0F * animationComponent.m_speed;

        // 获取当前帧
        const auto& currentFrame = currentAnimation.m_frames.at(
            animationComponent.m_currentFrameIndex);

        // 检查是否需要切换到下一帧
        if (animationComponent.m_currentTime >= currentFrame.m_duration) {
            animationComponent.m_currentTime -= currentFrame.m_duration;
            ++animationComponent.m_currentFrameIndex;

            // 处理动画播放完成
            if (animationComponent.m_currentFrameIndex >= currentAnimation.m_frames.size()) {
                if (currentAnimation.m_isLoop) {
                    animationComponent.m_currentFrameIndex = 0;
                } else {
                    // 动画播放完毕且不循环，停在最后一帧
                    animationComponent.m_currentFrameIndex = currentAnimation.m_frames.size() - 1;
                }
            }
        }

        // 更新 SpriteComponent 的源矩形 （根据当前动画帧的源矩形信息）
        const auto& nextFrame = currentAnimation.m_frames.at(animationComponent.m_currentFrameIndex);
        spriteComponent.m_sprite.m_sourceRect = nextFrame.m_sourceRect;
    }
}

void AnimationSystem::playAnimation(const engine::utils::PlayAnimationEvent& event)
{
    // 使用 try_get 安全获取可能存在的组件，如果不存在则返回 nullptr
    if (auto animationComponent = m_registry.try_get<engine::component::AnimationComponent>(
            event.m_entity);
        animationComponent != nullptr) {
        // 替换为事件中的动画 ID
        animationComponent->m_currentAnimationId = event.m_animationId;
        // 从头播放动画
        animationComponent->m_currentFrameIndex = 0;
        animationComponent->m_currentTime = 0.0F;
        // 设置是否循环播放
        animationComponent->m_animations.at(event.m_animationId).m_isLoop = event.m_loop;
    }
}

} // namespace engine::system

#include "animation_system.h"
#include "../component/animation_component.h"
#include "../component/sprite_component.h"

#include <entt/entity/registry.hpp>

namespace engine::system {

void AnimationSystem::update(entt::registry& registry, float deltaTime)
{
    auto view
        = registry.view<engine::component::AnimationComponent, engine::component::SpriteComponent>();
    for (auto entity : view) {
        auto& animationComponent = view.get<engine::component::AnimationComponent>(entity);
        auto& spriteComponent = view.get<engine::component::SpriteComponent>(entity);

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
        animationComponent.m_currentTime += deltaTime * 1000.0f * animationComponent.m_speed;

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

} // namespace engine::system

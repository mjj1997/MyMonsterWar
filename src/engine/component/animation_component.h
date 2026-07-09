#pragma once

#include "../utils/math.h"

#include <entt/entity/entity.hpp>

#include <unordered_map>
#include <vector>

namespace engine::component {

/**
 * @brief 动画帧数据结构，代表动画中的单个帧。
 *
 * 包含该帧在纹理图集上的源矩形，和帧持续时间（毫秒）。
 */
struct AnimationFrame
{
    AnimationFrame(engine::utils::Rect sourceRect, float duration = 100.0F)
        : m_sourceRect{ sourceRect }
        , m_duration{ duration }
    {}

    engine::utils::Rect m_sourceRect; ///< @brief 该帧在纹理图集上的源矩形
    float m_duration;                 ///< @brief 帧持续时间（毫秒）
};

/**
 * @brief 动画数据结构，代表整个动画。
 * 
 * 包含帧列表、总时长和是否循环等属性。
 */
struct Animation
{
public:
    /**
     * @brief 构造函数
     * @param frames 动画帧列表。
     * @param isLoop 动画是否应该循环播放。
     */
    explicit Animation(std::vector<AnimationFrame> frames, bool isLoop = true)
        : m_frames{ std::move(frames) }
        , m_isLoop{ isLoop }
    {
        // 计算总时长
        for (const auto& frame : m_frames) {
            m_totalDuration += frame.m_duration;
        }
    }

    std::vector<AnimationFrame> m_frames; ///< @brief 动画帧列表
    float m_totalDuration{ 0.0F };        ///< @brief 动画的总持续时间（毫秒）
    bool m_isLoop{ true };                ///< @brief 动画是否循环播放，默认值为 true
};

/**
 * @brief 动画组件。
 *
 * 包含动画集合、当前播放的动画、当前播放的帧索引、当前播放时间（毫秒）和播放速度。
 */
struct AnimationComponent
{
    /**
     * @brief 构造函数
     * @param animations 动画集合
     * @param currentAnimationId 当前播放的动画名称 id
     * @param currentFrameIndex 当前播放的帧索引
     * @param currentTime 当前播放时间（毫秒）
     * @param speed 播放速度
     */
    AnimationComponent(std::unordered_map<entt::id_type, Animation> animations,
                       entt::id_type currentAnimationId,
                       size_t currentFrameIndex = 0,
                       float currentTime = 0.0F,
                       float speed = 1.0F)
        : m_animations{ std::move(animations) }
        , m_currentAnimationId{ currentAnimationId }
        , m_currentFrameIndex{ currentFrameIndex }
        , m_currentTime{ currentTime }
        , m_speed{ speed }
    {}

    /// @brief 动画集合，键为动画名称 id，值为动画数据
    std::unordered_map<entt::id_type, Animation> m_animations;
    entt::id_type m_currentAnimationId{ entt::null }; ///< @brief 当前播放的动画名称 id
    size_t m_currentFrameIndex{ 0 };                  ///< @brief 当前播放的帧索引
    float m_currentTime{ 0.0F };                      ///< @brief 当前播放时间（毫秒）
    float m_speed{ 1.0F };                            ///< @brief 播放速度
};

} // namespace engine::component

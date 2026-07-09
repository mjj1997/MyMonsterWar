#pragma once

#include "../utils/math.h"

#include <entt/core/hashed_string.hpp>
#include <entt/entity/entity.hpp>
#include <glm/common.hpp>
#include <glm/vec2.hpp>

#include <string>

namespace engine::component {

/**
 * @brief 精灵数据结构
 * 
 * 包含纹理名称、源矩形和是否翻转。
 */
struct Sprite
{
    Sprite() = default; ///< @brief 空的构造函数

    /**
     * @brief 构造函数 (通过纹理路径构造)
     * @param texturePath 纹理路径
     * @param sourceRect 源矩形
     * @param isFlipped 是否翻转，默认 false
     */
    Sprite(std::string_view texturePath, engine::utils::Rect sourceRect, bool isFlipped = false)
        : m_texturePath{ texturePath }
        , m_sourceRect{ sourceRect }
        , m_isFlipped{ isFlipped }
    {
        m_textureId = entt::hashed_string(m_texturePath.c_str());
    }

    /**
     * @brief 构造函数 (通过纹理 ID 构造)
     * @param textureId 纹理 ID
     * @param sourceRect 源矩形
     * @param isFlipped 是否翻转，默认 false
     * @note 用此方法，需确保对应 ID 的纹理已经加载到 ResourceManager 中，因此不需要再提供纹理路径。
     */
    Sprite(entt::id_type textureId, engine::utils::Rect sourceRect, bool isFlipped = false)
        : m_textureId{ textureId }
        , m_sourceRect{ sourceRect }
        , m_isFlipped{ isFlipped }
    {}

    entt::id_type m_textureId{ entt::null }; ///< @brief 纹理ID
    std::string m_texturePath;               ///< @brief 纹理路径
    ///< @brief 源矩形(为了保证效率，不再使用std::optional，构造时必须提供)
    engine::utils::Rect m_sourceRect;
    bool m_isFlipped{ false }; ///< @brief 是否翻转
};

/**
 * @brief 精灵组件
 * 
 * 包含精灵、尺寸、偏移和是否可见
 */
struct SpriteComponent
{
public:
    /**
     * @brief 构造函数
     * @param sprite 精灵。
     * @param size 尺寸。
     * @param offset 偏移。
     * @param isHidden 是否隐藏。
     */
    SpriteComponent(Sprite sprite,
                    glm::vec2 size = glm::vec2(0.0F),
                    glm::vec2 offset = glm::vec2(0.0F),
                    bool isHidden = false)
        : m_sprite{ std::move(sprite) }
        , m_size{ size }
        , m_offset{ offset }
        , m_isHidden{ isHidden }
    {
        // 如果 size 为 0（未提供），则使用精灵的源矩形尺寸
        if (glm::all(glm::equal(size, glm::vec2(0.0F)))) {
            m_size = glm::vec2{ m_sprite.m_sourceRect.size.x, m_sprite.m_sourceRect.size.y };
        }
    }

    Sprite m_sprite;                  ///< @brief 精灵
    glm::vec2 m_size{ 0.0F, 0.0F };   ///< @brief 尺寸
    glm::vec2 m_offset{ 0.0F, 0.0F }; ///< @brief 偏移量
    bool m_isHidden{ false };         ///< @brief 是否隐藏（不渲染）
};

} // namespace engine::component

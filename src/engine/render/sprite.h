// sprite.h
#pragma once

#include "../utils/math.h"

#include <SDL3/SDL_rect.h> // 用于 SDL_FRect
#include <entt/core/hashed_string.hpp>
#include <entt/entity/entity.hpp>

#include <optional> // 用于 std::optional 表示可选的源矩形
#include <string>
#include <string_view>

namespace engine::render {

/**
 * @brief 表示要绘制的视觉精灵的数据。
 *
 * @note 包含纹理标识符、要绘制的纹理部分（源矩形）以及翻转状态。
 * @note 位置、缩放和旋转由外部（例如 SpriteComponent）标识。
 * @note 渲染工作由 Renderer 类完成。（传入Sprite作为参数）
 */
class Sprite final
{
public:
    /**
     * @brief 默认构造函数(创建一个空的精灵，不包含任何纹理或源矩形)
     */
    Sprite() = default;

    /**
     * @brief 构造一个精灵（通过纹理路径构造）
     *
     * @param texturePath 纹理资源的文件路径。不应为空。
     * @param sourceRect 可选的源矩形，定义要使用的纹理部分。如果为 std::nullopt，则使用整个纹理。
     * @param isFlipped 是否水平翻转
     */
    explicit Sprite(std::string_view texturePath,
                    std::optional<engine::utils::Rect> sourceRect = std::nullopt,
                    bool isFlipped = false)
        : m_texturePath{ texturePath.data() }
        , m_textureId{ entt::hashed_string(texturePath.data()).value() }
        , m_sourceRect{ std::move(sourceRect) }
        , m_isFlipped{ isFlipped }
    {}

    /**
     * @brief 构造一个精灵（通过纹理ID构造）
     *
     * @param textureId 纹理资源的标识符。不应为空。
     * @param sourceRect 可选的源矩形，定义要使用的纹理部分。如果为 std::nullopt，则使用整个纹理。
     * @param isFlipped 是否水平翻转
     * @note 用此方法，需确保对应ID的纹理已经加载到 ResourceManager 中，因此不需要再提供纹理路径。
     */
    explicit Sprite(entt::id_type textureId,
                    std::optional<engine::utils::Rect> sourceRect = std::nullopt,
                    bool isFlipped = false)
        : m_textureId{ textureId }
        , m_sourceRect{ std::move(sourceRect) }
        , m_isFlipped{ isFlipped }
    {}

    // --- getters and setters ---
    std::string_view texturePath() const { return m_texturePath; } ///< @brief 获取纹理路径
    entt::id_type textureId() const { return m_textureId; }        ///< @brief 获取纹理 ID
    ///< @brief 获取源矩形 (如果使用整个纹理则为 std::nullopt)
    const std::optional<engine::utils::Rect>& sourceRect() const { return m_sourceRect; }
    bool isFlipped() const { return m_isFlipped; } ///< @brief 获取是否水平翻转

    /**
     * @brief 设置纹理路径同时更新纹理ID
     * @param texturePath 纹理资源的文件路径。不应为空。
     */
    void setTexture(std::string_view texturePath)
    {
        m_texturePath = texturePath.data();
        m_textureId = entt::hashed_string(texturePath.data()).value();
    }

    ///< @brief 设置纹理ID (需确保已载入)
    void setTextureId(entt::id_type textureId) { m_textureId = textureId; }

    /**
     * @brief 设置源矩形 (如果使用整个纹理则为 std::nullopt)
     * @param sourceRect 源矩形。如果使用整个纹理则为 std::nullopt
     */
    void setSourceRect(std::optional<engine::utils::Rect> sourceRect)
    {
        m_sourceRect = std::move(sourceRect);
    }

    /**
     * @brief 设置是否水平翻转
     * @param isFlipped 是否水平翻转
     */
    void setFlipped(bool isFlipped) { m_isFlipped = isFlipped; }

private:
    ///< @brief 纹理资源的文件路径
    std::string m_texturePath;
    ///< @brief 纹理资源的标识符 (entt::null是推荐的初始化方式，表示无效的ID)
    entt::id_type m_textureId{ entt::null };
    ///< @brief 可选：要绘制的纹理部分
    std::optional<engine::utils::Rect> m_sourceRect{ std::nullopt };
    bool m_isFlipped{ false }; ///< @brief 是否水平翻转
};

} // namespace engine::render

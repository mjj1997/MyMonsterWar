#pragma once

#include "animation_component.h"
#include "sprite_component.h"

#include <glm/vec2.hpp>
#include <nlohmann/json.hpp>

#include <optional>
#include <vector>

namespace engine::component {

/**
 * @brief 定义瓦片的类型，用于游戏逻辑（例如碰撞）。
 * @note 当前项目并未用到此信息
 */
enum class TileType {
    Empty,     ///< @brief 空白瓦片
    Normal,    ///< @brief 普通瓦片
    Solid,     ///< @brief 静止可碰撞瓦片
    Unisolid,  ///< @brief 单向静止可碰撞瓦片
    Slope_0_1, ///< @brief 斜坡瓦片，高度:左0   右1
    Slope_1_0, ///< @brief 斜坡瓦片，高度:左1   右0
    Slope_0_2, ///< @brief 斜坡瓦片，高度:左0   右1/2
    Slope_2_1, ///< @brief 斜坡瓦片，高度:左1/2 右1
    Slope_1_2, ///< @brief 斜坡瓦片，高度:左1   右1/2
    Slope_2_0, ///< @brief 斜坡瓦片，高度:左1/2 右0
    Hazard,    ///< @brief 危险瓦片（例如火焰、尖刺等）
    Ladder,    ///< @brief 梯子瓦片
    // 未来补充其它类型
};

/**
 * @brief 瓦片信息
 * 
 * 包含精灵、类型、动画和属性。
 * @note 它只是辅助 LevelLoader 解析的临时数据，不会保存在游戏中。
 */
struct TileInfo
{
    TileInfo() = default;

    TileInfo(engine::component::Sprite sprite,
             engine::component::TileType type,
             std::optional<engine::component::Animation> animation = std::nullopt,
             std::optional<nlohmann::json> properties = std::nullopt)
        : m_sprite{ std::move(sprite) }
        , m_type{ type }
        , m_animation{ std::move(animation) }
        , m_properties{ std::move(properties) }
    {}

    engine::component::Sprite m_sprite;                      ///< @brief 精灵
    engine::component::TileType m_type;                      ///< @brief 类型
    std::optional<engine::component::Animation> m_animation; ///< @brief 动画（支持 Tiled 动画图块）
    std::optional<nlohmann::json>
        m_properties; ///< @brief 属性（存放 Tiled 自定义属性，方便 LevelLoader 解析）
};

/**
 * @brief 瓦片层组件。
 *
 * 包含瓦片大小、地图大小和瓦片实体列表。
 * @note 现在瓦片层更像一个容器，只是存储所有的“瓦片”，而每个瓦片就是一个实体。
 */
struct TileLayerComponent
{
    /**
     * @brief 构造函数
     * @param tileSize 单个瓦片尺寸（像素）
     * @param mapSize 地图尺寸（瓦片数）
     * @param tiles 瓦片实体列表
     */
    TileLayerComponent(glm::ivec2 tileSize, glm::ivec2 mapSize, std::vector<entt::entity> tiles)
        : m_tileSize{ tileSize }
        , m_mapSize{ mapSize }
        , m_tiles{ std::move(tiles) }
    {}

    glm::ivec2 m_tileSize; ///< @brief 单个瓦片尺寸（像素）
    glm::ivec2 m_mapSize;  ///< @brief 地图尺寸（瓦片数）
    ///< @brief 瓦片实体列表，每个瓦片对应一个实体，按顺序排列
    std::vector<entt::entity> m_tiles;
};

} // namespace engine::component

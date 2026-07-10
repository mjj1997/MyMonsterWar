#pragma once

#include "../utils/math.h"

#include <glm/vec2.hpp>
#include <nlohmann/json.hpp>

#include <map>
#include <optional>
#include <string>
#include <string_view>

namespace engine::component {
struct TileInfo;
enum class TileType;
} // namespace engine::component

namespace engine::scene {
class SceneBase;

/**
 * 关卡加载器，负责加载关卡数据，并生成游戏实体
 */
class LevelLoader final
{
public:
    LevelLoader() = default;

    /**
     * @brief 加载关卡数据到指定的 Scene 对象中，并根据关卡数据生成游戏实体。
     * @param mapPath Tiled JSON 地图文件的路径。
     * @param scene 要加载数据的目标 Scene 对象。
     * @return bool 是否加载成功。
     */
    [[nodiscard]] bool loadLevel(std::string_view mapPath, SceneBase& scene);

private:
    ///< @brief 加载图片图层
    void loadImageLayer(const nlohmann::json& layerJson, SceneBase& scene);
    ///< @brief 加载瓦片图层
    void loadTileLayer(const nlohmann::json& layerJson, SceneBase& scene);
    ///< @brief 加载对象图层
    void loadObjectLayer(const nlohmann::json& layerJson, SceneBase& scene);


    /**
     * @brief 获取瓦片属性
     * @tparam T 属性类型
     * @param tileJson 瓦片 JSON 数据
     * @param propertyName 属性名称
     * @return 属性值，如果属性不存在则返回 std::nullopt
     */
    template<typename T>
    std::optional<T> getTileProperty(const nlohmann::json& tileJson, std::string_view propertyName)
    {
        if (!tileJson.contains("properties")) {
            return std::nullopt;
        }

        for (const auto& property : tileJson["properties"]) {
            if (property.contains("name") && property["name"].get<std::string>() == propertyName) {
                if (property.contains("value")) {
                    return property["value"].get<T>();
                }
            }
        }

        return std::nullopt;
    }

    /**
     * @brief 根据瓦片 JSON 数据对象获取瓦片类型（当前项目中，TileType 无任何作用）
     * @param tileJson 瓦片 JSON 数据
     * @return 瓦片类型
     */
    static engine::component::TileType getTileType(const nlohmann::json& tileJson);

    /**
     * @brief 根据（单一图片）图块集中的 ID 获取瓦片类型（当前项目中，TileType 无任何作用）
     * @param tilesetJson 图块集 JSON 数据
     * @param localId 图块集中的 ID
     * @return 瓦片类型
     */
    static engine::component::TileType getTileTypeById(const nlohmann::json& tilesetJson,
                                                       int localId);

    /**
     * @brief 根据全局 ID 获取瓦片信息。
     * @param gid 全局 ID。
     * @return engine::component::TileInfo 瓦片信息。
     */
    engine::component::TileInfo getTileInfoByGid(int gid);

    /**
     * @brief 加载 Tiled tileset 文件 (.tsj)，数据保存到 m_tilesets 中。
     * @param tilesetPath Tileset 文件路径。
     * @param firstGid 此 tileset 的第一个全局 ID。
     */
    void loadTileset(std::string_view tilesetPath, int firstGid);

    /**
     * @brief 解析图片路径，合并文件路径和相对路径。例如：
     * 1. 文件路径："assets/maps/level1.tmj"
     * 2. 相对路径："../textures/Layers/back.png"
     * 3. 最终路径："assets/textures/Layers/back.png"
     * @param relativePath 相对路径（相对于文件）
     * @param filePath 文件路径
     * @return std::string 解析后的完整路径。
     */
    static std::string resolvePath(std::string_view relativePath, std::string_view filePath);

    std::string m_mapPath; ///< @brief 地图路径（拼接路径时需要）
    glm::ivec2 m_mapSize;  ///< @brief 地图尺寸（瓦片数量）
    glm::ivec2 m_tileSize; ///< @brief 瓦片尺寸（像素）

    std::map<int, nlohmann::json> m_tilesets; ///< @brief firstgid -> 瓦片集 json 数据
};

} // namespace engine::scene

#include "level_loader.h"
#include "../component/parallax_component.h"
#include "../component/sprite_component.h"
#include "../component/tilelayer_component.h"
#include "../component/transform_component.h"
#include "../core/context.h"
#include "../scene/scene_base.h"

#include <glm/vec2.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <filesystem>
#include <fstream>

namespace engine::scene {

void LevelLoader::setEntityBuilder(std::unique_ptr<BasicEntityBuilder> builder)
{
    m_entityBuilder = std::move(builder);
}

bool LevelLoader::loadLevel(std::string_view mapPath, SceneBase& scene)
{
    // 1. 加载关卡地图 JSON 文件
    auto path = std::filesystem::path{ mapPath };
    std::ifstream file{ path };
    if (!file.is_open()) {
        spdlog::error("无法打开关卡地图文件: {}", mapPath);
        return false;
    }

    // 2. 解析关卡地图 JSON 数据
    nlohmann::json mapJson;
    try {
        file >> mapJson;
    } catch (const nlohmann::json::parse_error& e) {
        spdlog::error("解析关卡地图 JSON 数据失败: {}", e.what());
        return false;
    }

    // 3. 获取基本地图信息（地图路径、地图尺寸、瓦片尺寸）
    m_mapPath = mapPath;
    m_mapSize = glm::ivec2{ mapJson.value("width", 0), mapJson.value("height", 0) };
    m_tileSize = glm::ivec2{ mapJson.value("tilewidth", 0), mapJson.value("tileheight", 0) };

    // 4. 加载瓦片集数据
    if (mapJson.contains("tilesets") && mapJson.at("tilesets").is_array()) {
        for (const auto& tileset : mapJson.at("tilesets")) {
            auto tilesetPath = LevelLoader::resolvePath(tileset.at("source").get<std::string>(),
                                                        m_mapPath);
            auto firstGid = tileset.at("firstgid").get<int>();
            loadTileset(tilesetPath, firstGid);
        }
    }

    // 5. 加载图层数据
    if (!mapJson.contains("layers") || !mapJson.at("layers").is_array()) {
        // 地图文件中必须有 layers 数组
        spdlog::error("地图文件 '{}' 中缺少或无效的 'layers' 数组。", mapPath);
        return false;
    }

    for (const auto& layer : mapJson.at("layers")) {
        // 获取各图层对象中的类型（type）字段
        std::string type{ layer.value("type", "none") };
        if (!layer.value("visible", true)) {
            spdlog::info("图层 '{}' 不可见，跳过加载。", layer.value("name", "Unnamed"));
            continue;
        }

        // 根据图层类型决定加载方法
        if (type == "imagelayer") {
            loadImageLayer(layer, scene);
        } else if (type == "tilelayer") {
            loadTileLayer(layer, scene);
        } else if (type == "objectgroup") {
            loadObjectLayer(layer, scene);
        } else {
            spdlog::warn("不支持的图层类型: {}", type);
        }
    }

    spdlog::info("关卡加载完成: {}", mapPath);
    return true;
}

void LevelLoader::loadImageLayer(const nlohmann::json& layerJson, SceneBase& scene)
{
    // 获取纹理相对路径 （会自动处理'\/'符号）
    const std::string imagePath{ layerJson.value("image", "") };
    if (imagePath.empty()) {
        spdlog::error("图层 '{}' 缺少 'image' 属性。", layerJson.value("name", "Unnamed"));
        return;
    }
    auto textureId = LevelLoader::resolvePath(imagePath, m_mapPath);

    // 获取图层偏移量（json中没有则代表未设置，给默认值即可）
    const glm::vec2 offset{ glm::vec2{ layerJson.value("offsetx", 0.0F),
                                       layerJson.value("offsety", 0.0F) } };

    // 获取视差因子及重复标志
    const glm::vec2 scrollFactor{ glm::vec2{ layerJson.value("parallaxx", 1.0F),
                                             layerJson.value("parallaxy", 1.0F) } };
    const glm::bvec2 repeat{ glm::bvec2{ layerJson.value("repeatx", false),
                                         layerJson.value("repeaty", false) } };

    // 获取图层名称
    std::string layerName{ layerJson.value("name", "Unnamed") };

    /*  可用类似方法获取其它各种属性，这里我们暂时用不上 */

    spdlog::info("加载图层: '{}' 完成", layerName);
}

void LevelLoader::loadTileLayer(const nlohmann::json& layerJson, SceneBase& scene)
{
    if (!layerJson.contains("data") || !layerJson.at("data").is_array()) {
        spdlog::error("图层 '{}' 缺少 'data' 属性。", layerJson.value("name", "Unnamed"));
        return;
    }

    // 准备 TileInfo Vector (瓦片数量 = 地图宽度 * 地图高度)
    std::vector<engine::component::TileInfo> tiles;
    tiles.reserve(static_cast<std::size_t>(m_mapSize.x * m_mapSize.y));

    // 根据gid获取必要信息，并依次填充 TileInfo Vector
    std::transform(layerJson.at("data").begin(),
                   layerJson.at("data").end(),
                   std::back_inserter(tiles),
                   [this](const auto& gid) { return getTileInfoByGid(gid); });

    // 获取图层名称
    std::string layerName{ layerJson.value("name", "Unnamed") };
    spdlog::info("加载图层: '{}' 完成", layerName);
}

void LevelLoader::loadObjectLayer(const nlohmann::json& layerJson, SceneBase& scene)
{
    if (!layerJson.contains("objects") || !layerJson.at("objects").is_array()) {
        spdlog::error("对象图层 '{}' 缺少 'objects' 属性。", layerJson.value("name", "Unnamed"));
        return;
    }

    for (const auto& object : layerJson.at("objects")) {
        auto gid = object.value("gid", 0);

        if (gid == 0) { // 如果 gid 为 0，代表是自定义形状，如碰撞盒，我们以后再处理
        } else { // 如果 gid 存在，则代表这是一个带图像的对象
        }
    }
}


engine::component::TileType LevelLoader::getTileType(const nlohmann::json& tileJson)
{
    if (tileJson.contains("properties")) {
        for (const auto& property : tileJson.at("properties")) {
            if (property.value("name", "") == "solid") {
                return property.value("value", false) ? engine::component::TileType::Solid
                                                      : engine::component::TileType::Normal;
            } else if (property.value("name", "") == "hazard") {
                return property.value("value", false) ? engine::component::TileType::Hazard
                                                      : engine::component::TileType::Normal;
            }
            // TODO: 可以在这里添加更多的自定义属性处理逻辑
        }
    }

    return engine::component::TileType::Normal;
}

engine::component::TileType LevelLoader::getTileTypeById(const nlohmann::json& tilesetJson,
                                                         int localId)
{
    if (tilesetJson.contains("tiles")) {
        const auto& tiles = tilesetJson.at("tiles");
        auto iter = std::find_if(tiles.begin(), tiles.end(), [localId](const auto& tile) {
            return tile.value("id", -1) == localId;
        });

        if (iter != tiles.end()) {
            return LevelLoader::getTileType(*iter);
        }
    }

    return engine::component::TileType::Normal;
}

engine::component::TileInfo LevelLoader::getTileInfoByGid(int gid)
{
    if (gid == 0) {
        return engine::component::TileInfo{};
    }

    // upper_bound：查找 tilesets 中键大于 gid 的第一个元素，返回迭代器
    auto iter = m_tilesets.upper_bound(gid);
    if (iter == m_tilesets.begin()) {
        spdlog::error("gid为 {} 的瓦片未找到图块集。", gid);
        return engine::component::TileInfo{};
    }
    --iter; // 前移一个位置，这样就得到不大于gid的最近一个元素（我们需要的）

    const auto& tilesetJson = iter->second;
    const auto& tilesetFirstGid = iter->first;

    const std::string filePath{ tilesetJson.value("filePath", "") }; // 获取图块集文件路径
    if (filePath.empty()) {
        spdlog::error("Tileset 文件 '{}' 缺少 'filePath' 属性。", tilesetFirstGid);
        return engine::component::TileInfo{};
    }

    auto localId = gid - tilesetFirstGid; // 计算瓦片在图块集中的局部 ID

    // 图块集分为两种情况，需要分别考虑
    if (tilesetJson.contains("image")) { // 这是单一图片的情况
        // 获取图片路径
        auto textureId = LevelLoader::resolvePath(tilesetJson.at("image").get<std::string>(),
                                                  filePath);
        // 计算在图片网格中的瓦片坐标
        auto coordinateX = localId % tilesetJson.at("columns").get<int>();
        auto coordinateY = localId / tilesetJson.at("columns").get<int>();
        // 根据瓦片坐标确定源矩形
        SDL_FRect srcRect{ .x = static_cast<float>(coordinateX * m_tileSize.x),
                           .y = static_cast<float>(coordinateY * m_tileSize.y),
                           .w = static_cast<float>(m_tileSize.x),
                           .h = static_cast<float>(m_tileSize.y) };
        // 创建瓦片精灵
        const engine::render::Sprite sprite{ textureId, srcRect };
        // 获取瓦片类型
        auto tileType = LevelLoader::getTileTypeById(tilesetJson, localId);
        // 返回瓦片信息
        return engine::component::TileInfo{ sprite, tileType };
    } else { // 这是多图片的情况
        // 没有 tiles 字段的话不符合数据格式要求，直接返回空的瓦片信息
        if (!tilesetJson.contains("tiles")) {
            spdlog::error("Tileset 文件 '{}' 缺少 'tiles' 属性。", tilesetFirstGid);
            return engine::component::TileInfo{};
        }

        // 遍历 tileset JSON 中的 tiles 数组，根据局部 ID 查找对应的瓦片
        for (const auto& tile : tilesetJson.at("tiles")) {
            auto tileId = tile.value("id", 0);
            if (tileId == localId) { // 找到对应的瓦片，进行后续操作
                // 没有 image 字段的话不符合数据格式要求，直接返回空的瓦片信息
                if (!tile.contains("image")) {
                    spdlog::error("Tileset 文件 '{}' 中瓦片 {} 缺少 'image' 属性。",
                                  tilesetFirstGid,
                                  tileId);
                    return engine::component::TileInfo{};
                }

                // --- 接下来根据必要信息创建并返回 TileInfo ---
                // 获取图片路径
                auto textureId = LevelLoader::resolvePath(tile.at("image").get<std::string>(),
                                                          filePath);
                // 先确认图片尺寸
                auto imageWidth = tile.value("imagewidth", 0);
                auto imageHeight = tile.value("imageheight", 0);
                /** 从 tile JSON 中获取源矩形信息
                 * Tiled 中源矩形信息只有设置了才会有值，没有就是默认值
                 * 如果未设置，则使用图片尺寸
                 */
                SDL_FRect srcRect{ .x = static_cast<float>(tile.value("x", 0)),
                                   .y = static_cast<float>(tile.value("y", 0)),
                                   .w = static_cast<float>(tile.value("width", imageWidth)),
                                   .h = static_cast<float>(tile.value("height", imageHeight)) };
                // 创建瓦片精灵
                const engine::render::Sprite sprite{ textureId, srcRect };
                // 获取瓦片类型
                auto tileType = LevelLoader::getTileType(tile);
                // 返回瓦片信息
                return engine::component::TileInfo{ sprite, tileType };
            }
        }
    }

    // 如果能走到这里，说明查找失败，返回空的瓦片信息
    spdlog::error("Tileset 文件 '{}' 中未找到gid为 {} 的瓦片。", tilesetFirstGid, gid);
    return engine::component::TileInfo{};
}

void LevelLoader::loadTileset(std::string_view tilesetPath, int firstGid)
{
    auto path = std::filesystem::path{ tilesetPath };
    std::ifstream file{ path };
    if (!file.is_open()) {
        spdlog::error("无法打开瓦片集文件: {}", tilesetPath);
        return;
    }

    nlohmann::json tilesetJson;
    file >> tilesetJson;

    // 注入瓦片集文件路径，方便后续加载瓦片时解析相对路径
    tilesetJson.emplace("filePath", tilesetPath);

    m_tilesets[firstGid] = std::move(tilesetJson);
}

std::string LevelLoader::resolvePath(std::string_view relativePath, std::string_view filePath)
{
    try {
        // 获取地图/瓦片集文件的父目录（相对于可执行文件） "assets/maps/level1.tmj" -> "assets/maps"
        auto fileDir = std::filesystem::path(filePath).parent_path();
        // 合并路径（相对于可执行文件）并返回。
        /* std::filesystem::canonical：解析路径中的当前目录（.）和上级目录（..）导航符，得到一个干净的路径 */
        auto finalPath = std::filesystem::canonical(fileDir / relativePath);
        return finalPath.string();
    } catch (const std::exception& e) {
        spdlog::error("解析路径失败: {}", e.what());
        return std::string(relativePath);
    }
}

} // namespace engine::scene

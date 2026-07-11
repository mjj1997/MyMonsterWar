#include "level_loader.h"
#include "../component/name_component.h"
#include "../component/parallax_component.h"
#include "../component/sprite_component.h"
#include "../component/tilelayer_component.h"
#include "../component/transform_component.h"
#include "../core/context.h"
#include "../resource/resource_manager.h"
#include "../scene/scene_base.h"

#include <entt/core/hashed_string.hpp>
#include <entt/entity/registry.hpp>
#include <glm/vec2.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <filesystem>
#include <fstream>

namespace engine::scene {

LevelLoader::~LevelLoader() = default;

void LevelLoader::setEntityBuilder(std::unique_ptr<BasicEntityBuilder> builder)
{
    m_entityBuilder = std::move(builder);
}

bool LevelLoader::loadLevel(std::string_view mapPath, engine::scene::SceneBase* scene)
{
    if (scene == nullptr) {
        spdlog::error("场景指针为空");
        return false;
    }
    m_scene = scene;

    if (m_entityBuilder == nullptr) {
        spdlog::info("设置默认的实体生成器");
        m_entityBuilder = std::make_unique<BasicEntityBuilder>(*this,
                                                               scene->context(),
                                                               scene->registry());
    }

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

    // 4. 加载瓦片集 JSON 数据
    if (mapJson.contains("tilesets") && mapJson.at("tilesets").is_array()) {
        for (const auto& tileset : mapJson.at("tilesets")) {
            if (!tileset.contains("source") || !tileset.at("source").is_string()
                || !tileset.contains("firstgid") || !tileset.at("firstgid").is_number_integer()) {
                spdlog::error("瓦片集 JSON 数据对象中缺少有效的 'source' 或 'firstgid' 字段。");
                continue;
            }

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
            loadImageLayer(layer);
        } else if (type == "tilelayer") {
            loadTileLayer(layer);
        } else if (type == "objectgroup") {
            loadObjectLayer(layer);
        } else {
            spdlog::warn("不支持的图层类型: {}", type);
        }
    }

    spdlog::info("关卡加载完成: {}", mapPath);
    return true;
}

void LevelLoader::loadImageLayer(const nlohmann::json& layerJson)
{
    // 获取纹理相对路径 （会自动处理'\/'符号）
    const std::string imagePath{ layerJson.value("image", "") };
    if (imagePath.empty()) {
        spdlog::error("图层 '{}' 缺少 'image' 属性。", layerJson.value("name", "Unnamed"));
        return;
    }

    // 创建精灵（在获取纹理大小时会确保纹理加载）
    auto texturePath = LevelLoader::resolvePath(imagePath, m_mapPath);
    auto& resourceManager = m_scene->context().resourceManager();
    auto textureSize = resourceManager.getTextureSize(entt::hashed_string(texturePath.c_str()),
                                                      texturePath);
    auto sprite = engine::component::Sprite{ texturePath,
                                             engine::utils::Rect{ glm::vec2(0.0F), textureSize } };

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
    entt::id_type layerNameId{ entt::hashed_string(layerName.c_str()) };

    /*  可用类似方法获取其它各种属性，这里我们暂时用不上 */

    // 创建图层实体
    auto& registry = m_scene->registry();
    auto layerEntity = registry.create();

    // 添加组件
    registry.emplace<engine::component::NameComponent>(layerEntity, layerNameId, layerName);
    registry.emplace<engine::component::TransformComponent>(layerEntity, offset);
    registry.emplace<engine::component::ParallaxComponent>(layerEntity, scrollFactor, repeat);
    registry.emplace<engine::component::SpriteComponent>(layerEntity, sprite);

    /* 实体与组件创建完毕后，由 registry 自动管理，不需要“添加到场景”的步骤 */

    spdlog::info("加载图层: '{}' 完成", layerName);
}

void LevelLoader::loadTileLayer(const nlohmann::json& layerJson)
{
    if (!layerJson.contains("data") || !layerJson.at("data").is_array()) {
        spdlog::error("图层 '{}' 缺少 'data' 属性。", layerJson.value("name", "Unnamed"));
        return;
    }

    // 获取图层名称
    std::string layerName{ layerJson.value("name", "Unnamed") };
    entt::id_type layerNameId{ entt::hashed_string(layerName.c_str()) };

    // 准备瓦片实体 Vector (瓦片数量 = 地图宽度 * 地图高度)
    std::vector<entt::entity> tiles;
    tiles.reserve(static_cast<std::size_t>(m_mapSize.x * m_mapSize.y));

    // 获取图层数据 (瓦片 ID 列表)
    const auto& data = layerJson.at("data");
    size_t index{ 0 }; // data数据的索引，它决定图块在地图中的位置
    // --- 每一个瓦片都是一个独立的entity ---
    for (const int gid : data) {
        if (gid == 0) {
            ++index;
            continue;
        }

        auto tileInfo = getTileInfoByGid(gid);
        if (!tileInfo) {
            spdlog::error("瓦片 ID 为 {} 的瓦片未找到图块集。", gid);
            ++index;
            continue;
        }

        // 使用生成器创建瓦片实体
        auto tileEntity = m_entityBuilder->configure(index, &tileInfo.value())->build()->entityId();
        // 添加到vector中
        tiles.push_back(tileEntity);
        ++index;
    }

    // 创建图层实体
    auto& registry = m_scene->registry();
    auto layerEntity = registry.create();

    // 添加组件
    registry.emplace<engine::component::NameComponent>(layerEntity, layerNameId, layerName);
    registry.emplace<engine::component::TileLayerComponent>(layerEntity,
                                                            m_tileSize,
                                                            m_mapSize,
                                                            tiles);

    spdlog::info("加载图层: '{}' 完成", layerName);
}

void LevelLoader::loadObjectLayer(const nlohmann::json& layerJson)
{
    if (!layerJson.contains("objects") || !layerJson.at("objects").is_array()) {
        spdlog::error("对象图层 '{}' 缺少 'objects' 属性。", layerJson.value("name", "Unnamed"));
        return;
    }

    for (const auto& object : layerJson.at("objects")) {
        // 获取对象 gid
        auto gid = object.value("gid", 0);
        // 如果 gid 为 0（即不存在），则代表是自定义形状
        if (gid == 0) {
            // 配置自定义形状生成器
            m_entityBuilder->configure(&object)->build();
        } else { // 如果 gid 存在，则代表这是一个带图像的对象
            auto tileInfo = getTileInfoByGid(gid);
            if (!tileInfo) {
                spdlog::warn("对象图层 '{}' 中的对象缺少有效的 'gid' 或瓦片信息。",
                             layerJson.value("name", "Unnamed"));
                continue;
            }
            // 配置图片对象生成器
            m_entityBuilder->configure(&object, &tileInfo.value())->build();
        }
    }
}

engine::utils::Rect LevelLoader::getTextureRect(const nlohmann::json& tilesetJson, int localId)
{
    auto columns = tilesetJson.value("columns", 1);
    auto tileWidth = tilesetJson.value("tilewidth", 0);
    auto tileHeight = tilesetJson.value("tileheight", 0);
    auto tileX = localId % columns;
    auto tileY = localId / columns;
    return engine::utils::Rect{ glm::vec2{ tileX * tileWidth, tileY * tileHeight },
                                glm::vec2{ tileWidth, tileHeight } };
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

std::optional<engine::component::TileInfo> LevelLoader::getTileInfoByGid(int gid)
{
    if (gid == 0) {
        return std::nullopt;
    }

    // upper_bound：查找 tilesets 中键大于 gid 的第一个元素，返回迭代器
    auto iter = m_tilesets.upper_bound(gid);
    if (iter == m_tilesets.begin()) {
        spdlog::error("gid为 {} 的瓦片未找到图块集。", gid);
        return std::nullopt;
    }
    --iter; // 前移一个位置，这样就得到不大于gid的最近一个元素（我们需要的）

    const auto& tilesetJson = iter->second;
    const auto& tilesetFirstGid = iter->first;

    const std::string filePath{ tilesetJson.value("filepath", "") }; // 获取图块集文件路径
    if (filePath.empty()) {
        spdlog::error("Tileset 文件 '{}' 缺少 'filepath' 属性。", tilesetFirstGid);
        return std::nullopt;
    }

    auto localId = gid - tilesetFirstGid; // 计算瓦片在图块集中的局部 ID

    engine::component::TileInfo tileInfo; // 初始化瓦片信息
    // 图块集分为两种情况，使用一个标志进行记录区分
    bool isSingleImage{ false };
    // --- 单一图片的情况 ---
    if (tilesetJson.contains("image")) {
        // 获取图片路径
        auto texturePath = LevelLoader::resolvePath(tilesetJson.at("image").get<std::string>(),
                                                    filePath);
        // 获取纹理源矩形
        auto textureRect = LevelLoader::getTextureRect(tilesetJson, localId);
        // 填充瓦片信息中的精灵和类型
        tileInfo.m_sprite = engine::component::Sprite{ texturePath, textureRect };
        tileInfo.m_type = LevelLoader::getTileTypeById(tilesetJson,
                                                       localId); // 只有瓦片 id，还没有瓦片 json 数据
        // 标记为单一图片
        isSingleImage = true;
    }

    // --- 多图片的情况 ---
    if (!isSingleImage && !tilesetJson.contains("tiles")) {
        // 没有 tiles 字段的话不符合数据格式要求，直接返回空的瓦片信息
        spdlog::error("Tileset 文件 '{}' 缺少 'tiles' 属性。", tilesetFirstGid);
        return std::nullopt;
    }

    // 遍历 tileset JSON 中的 tiles 数组，根据局部 ID 查找对应的瓦片
    for (const auto& tile : tilesetJson.at("tiles")) {
        auto tileId = tile.value("id", 0);
        // 找到对应的瓦片，进行后续操作
        if (tileId == localId) {
            // 如果是多图片，需要先补充精灵和类型信息
            if (!isSingleImage) {
                // 没有 image 字段的话不符合数据格式要求，直接返回空的瓦片信息
                if (!tile.contains("image")) {
                    spdlog::error("Tileset 文件 '{}' 中瓦片 {} 缺少 'image' 属性。",
                                  tilesetFirstGid,
                                  tileId);
                    return std::nullopt;
                }

                // --- 接下来根据必要信息创建并返回 TileInfo ---
                // 获取图片路径
                auto texturePath = LevelLoader::resolvePath(tile.at("image").get<std::string>(),
                                                            filePath);
                // 先确认图片尺寸
                auto imageWidth = tile.value("imagewidth", 0);
                auto imageHeight = tile.value("imageheight", 0);
                /** 从 tile JSON 中获取源矩形信息
                 * Tiled 中源矩形信息只有设置了才会有值，没有就是默认值
                 * 如果未设置，则使用图片尺寸
                 */
                engine::utils::Rect srcRect{ static_cast<float>(tile.value("x", 0)),
                                             static_cast<float>(tile.value("y", 0)),
                                             static_cast<float>(tile.value("width", imageWidth)),
                                             static_cast<float>(tile.value("height", imageHeight)) };
                // 填充瓦片信息中的精灵和类型
                tileInfo.m_sprite = engine::component::Sprite{ texturePath, srcRect };
                tileInfo.m_type = LevelLoader::getTileType(tile); // 已经有具体的瓦片 JSON 数据
            }

            // 补充动画信息（瓦片动画为 animation 字段，且必须为数组，目前只考虑单一图片的情况）
            if (tile.contains("animation") && isSingleImage && tile.at("animation").is_array()) {
                std::vector<engine::component::AnimationFrame> animationFrames;
                for (const auto& frame : tile.at("animation")) {
                    // 每个瓦片动画帧 JSON 包含两个字段：tileid 和 duration
                    float duration{ frame.value("duration", 100.0F) }; // 毫秒
                    int tileId{ frame.value("tileid", 0) };
                    auto frameRect = LevelLoader::getTextureRect(tilesetJson, tileId);
                    // 帧源矩形 + 帧时长 = 动画帧
                    animationFrames.emplace_back(frameRect, duration);
                }
                // 填充瓦片信息中的动画
                tileInfo.m_animation = engine::component::Animation{ std::move(animationFrames) };
            }

            // 补充属性信息
            if (tile.contains("properties")) {
                tileInfo.m_properties = tile.at("properties");
            }
        }
    }

    return tileInfo;
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
    tilesetJson.emplace("filepath", tilesetPath);

    m_tilesets.try_emplace(firstGid, std::move(tilesetJson));
    spdlog::info("瓦片集文件 '{}' 加载完成，firstgid: {}", tilesetPath, firstGid);
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

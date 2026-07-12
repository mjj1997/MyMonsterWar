#pragma once

#include <entt/entity/registry.hpp>
#include <glm/vec2.hpp>
#include <nlohmann/json_fwd.hpp>

#include <optional>

namespace engine::core {
class Context;
}

namespace engine::component {
struct TileInfo;
}

namespace engine::loader {
class LevelLoader;

/**
 * 基础游戏实体生成器，由 LevelLoader 使用，通过建造者模式生成各种游戏实体
 * 子类可以继承并添加新的 buildXXX 方法，以实现更多功能
 */
class BasicEntityBuilder
{
public:
    BasicEntityBuilder(engine::loader::LevelLoader& levelLoader,
                       engine::core::Context& context,
                       entt::registry& registry);
    virtual ~BasicEntityBuilder();

    // --- 三个关键方法：配置、构建、返回 ---
    // --- 配置 ---
    /// @brief 针对自定义形状（对象层）
    BasicEntityBuilder* configure(const nlohmann::json* objectJson);

    /// @brief 针对图片对象 (对象层)
    BasicEntityBuilder* configure(const nlohmann::json* objectJson,
                                  const engine::component::TileInfo* tileInfo);

    /// @brief 针对瓦片 (瓦片层，相比“阳光岛”新增的功能)
    BasicEntityBuilder* configure(int index, const engine::component::TileInfo* tileInfo);

    // --- 构建 ---
    virtual BasicEntityBuilder* build(); ///< @brief 构建实体

    // --- 返回 ---
    entt::entity entityId(); ///< @brief 获取实体 ID

protected:
    void reset(); ///< @brief 重置生成器状态，每次 configure 的时候先调用

    void buildBase();
    void buildSprite();
    void buildTransform();
    void buildAnimation();
    void buildAudio();

    // --- 代理函数，让子类能获取到 LevelLoader 的私有方法 ---
    template<typename T>
    std::optional<T> getTileProperty(const nlohmann::json& tileJson, std::string_view propertyName);

    engine::loader::LevelLoader& m_levelLoader;
    engine::core::Context& m_context;
    entt::registry& m_registry;

    // 解析游戏对象所需要的关键信息
    ///< @brief 来自 .tmj 地图文件的对象数据
    const nlohmann::json* m_objectJson{ nullptr };
    ///< @brief 来自 .tsj 瓦片集文件的瓦片数据
    const engine::component::TileInfo* m_tileInfo{ nullptr };
    int m_index{ -1 }; ///< @brief 瓦片索引，用于计算位置（瓦片层）

    // --- 保存会多次用到的变量，避免重复解析 ---
    entt::entity m_entityId;
    glm::vec2 m_position;
    glm::vec2 m_dstSize;
    glm::vec2 m_srcSize;
};

} // namespace engine::loader

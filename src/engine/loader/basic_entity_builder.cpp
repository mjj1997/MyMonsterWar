#include "basic_entity_builder.h"
#include "../component/name_component.h"
#include "../component/render_component.h"
#include "../component/sprite_component.h"
#include "../component/tilelayer_component.h"
#include "../component/transform_component.h"
#include "../core/context.h"
#include "../resource/resource_manager.h"
#include "level_loader.h"

#include <entt/entt.hpp>
#include <spdlog/spdlog.h>

namespace engine::loader {

BasicEntityBuilder::BasicEntityBuilder(engine::loader::LevelLoader& levelLoader,
                                       engine::core::Context& context,
                                       entt::registry& registry)
    : m_levelLoader{ levelLoader }
    , m_context{ context }
    , m_registry{ registry }
{}

BasicEntityBuilder::~BasicEntityBuilder() = default;

void BasicEntityBuilder::reset()
{
    m_objectJson = nullptr;
    m_tileInfo = nullptr;
    m_index = -1;
    m_entityId = entt::null;
    m_position = glm::vec2(0.0F);
    m_dstSize = glm::vec2(0.0F);
    m_srcSize = glm::vec2(0.0F);
}

BasicEntityBuilder* BasicEntityBuilder::configure(const nlohmann::json* objectJson)
{
    // 重置生成器状态
    reset();

    if (objectJson == nullptr) {
        spdlog::error("配置生成器时，objectJson 不能为空");
        return nullptr;
    }
    m_objectJson = objectJson;

    spdlog::trace("针对自定义形状配置生成器完成");
    return this;
}

BasicEntityBuilder* BasicEntityBuilder::configure(const nlohmann::json* objectJson,
                                                  const engine::component::TileInfo* tileInfo)
{
    // 重置生成器状态
    reset();

    if (objectJson == nullptr || tileInfo == nullptr) {
        spdlog::error("配置生成器时，objectJson 和 tileInfo 不能为空");
        return nullptr;
    }
    m_objectJson = objectJson;
    m_tileInfo = tileInfo;

    spdlog::trace("针对图片对象配置生成器完成");
    return this;
}

BasicEntityBuilder* BasicEntityBuilder::configure(int index,
                                                  const engine::component::TileInfo* tileInfo)
{
    // 重置生成器状态
    reset();

    if (tileInfo == nullptr) {
        spdlog::error("配置生成器时，tileInfo 不能为空");
        return nullptr;
    }
    m_index = index;
    m_tileInfo = tileInfo;

    spdlog::trace("针对瓦片配置生成器完成");
    return this;
}

BasicEntityBuilder* BasicEntityBuilder::build()
{
    if (m_objectJson == nullptr && m_tileInfo == nullptr) {
        spdlog::error("objectJson 和 tileInfo 都为空，无法进行构建");
        return this;
    }

    // 按顺序构建各个组件
    buildBase();
    buildSprite();
    buildTransform();
    buildAnimation();
    buildAudio();
    buildRender();

    return this;
}

entt::entity BasicEntityBuilder::entityId()
{
    return m_entityId;
}

// --- 构建组件 ---

void BasicEntityBuilder::buildBase()
{
    spdlog::trace("构建基础组件");

    // 创建一个实体并添加 NameComponent 组件
    m_entityId = m_registry.create();
    if (m_objectJson != nullptr && m_objectJson->contains("name")) {
        std::string name{ m_objectJson->value("name", "") };
        entt::id_type nameId{ entt::hashed_string(name.c_str()) };
        m_registry.emplace<engine::component::NameComponent>(m_entityId, nameId, name);

        spdlog::trace("添加 NameComponent 组件，name: {}", name);
    }
}

void BasicEntityBuilder::buildSprite()
{
    spdlog::trace("构建精灵组件");

    // 如果是自定义形状对象，则不需要精灵组件
    if (m_tileInfo == nullptr) {
        return;
    }

    // 添加精灵组件时，确保纹理加载
    auto& resourceManager = m_context.resourceManager();
    resourceManager.loadTexture(m_tileInfo->m_sprite.m_textureId,
                                m_tileInfo->m_sprite.m_texturePath);
    m_registry.emplace<engine::component::SpriteComponent>(m_entityId, m_tileInfo->m_sprite);
}

void BasicEntityBuilder::buildTransform()
{
    spdlog::trace("构建变换组件");

    glm::vec2 scale{ 1.0F, 1.0F };
    float rotation{ 0.0F };

    // 对象层实体，位置、尺寸和旋转信息从 m_objectJson 中获取
    if (m_objectJson != nullptr) {
        m_position = glm::vec2(m_objectJson->value("x", 0.0F), m_objectJson->value("y", 0.0F));
        m_dstSize = glm::vec2(m_objectJson->value("width", 0.0F),
                              m_objectJson->value("height", 0.0F));
        // 图片对象的位置需要进行调整(左下角到左上角)
        m_position = glm::vec2(m_position.x, m_position.y - m_dstSize.y);
        rotation = m_objectJson->value("rotation", 0.0F);
        // 如果是图片对象，需要调整缩放
        if (m_tileInfo != nullptr) {
            m_srcSize = glm::vec2(m_tileInfo->m_sprite.m_sourceRect.size.x,
                                  m_tileInfo->m_sprite.m_sourceRect.size.y);
            scale = m_dstSize / m_srcSize;
        }
    }

    // 瓦片层实体，通过index (Tiled瓦片层data数据的索引) 计算位置
    if (m_index >= 0) {
        const auto mapSize{ m_levelLoader.mapSize() };
        const auto tileSize{ m_levelLoader.tileSize() };
        m_position = glm::vec2((m_index % mapSize.x) * tileSize.x,
                               (m_index / mapSize.x) * tileSize.y);
    }

    // 添加变换组件
    m_registry.emplace<engine::component::TransformComponent>(m_entityId,
                                                              m_position,
                                                              scale,
                                                              rotation);
}

void BasicEntityBuilder::buildAnimation()
{
    spdlog::trace("构建动画组件");

    // 如果存在动画，其信息已经解析并保存在m_tileInfo中
    if (m_tileInfo != nullptr && m_tileInfo->m_animation.has_value()) {
        // 创建动画map
        std::unordered_map<entt::id_type, engine::component::Animation> animations;
        auto animationId = entt::hashed_string("tile"); // 图块动画名称默认为"tile"
        animations.emplace(animationId, std::move(m_tileInfo->m_animation.value()));
        // 通过动画map创建动画组件，并添加到实体
        m_registry.emplace<engine::component::AnimationComponent>(m_entityId,
                                                                  std::move(animations),
                                                                  animationId);
    }
}

void BasicEntityBuilder::buildAudio()
{
    spdlog::trace("构建音频组件");
    // 当前项目并未使用，未来可约定自定义属性并解析
}

void BasicEntityBuilder::buildRender()
{
    spdlog::trace("构建渲染组件");

    int currentLayerIndex{ m_levelLoader.currentLayerIndex() };
    float depth{ m_position.y };
    // 添加渲染组件，图层序号越小越先绘制，深度越小越先绘制
    m_registry.emplace<engine::component::RenderComponent>(m_entityId, currentLayerIndex, depth);
}

// --- 代理函数，让子类能获取到 LevelLoader 的私有方法 ---
template<typename T>
std::optional<T> BasicEntityBuilder::getTileProperty(const nlohmann::json& tileJson,
                                                     std::string_view propertyName)
{
    return m_levelLoader.getTileProperty<T>(tileJson, propertyName);
}

} // namespace engine::loader

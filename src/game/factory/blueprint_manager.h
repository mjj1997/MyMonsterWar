#pragma once

namespace engine::resource {
class ResourceManager;
}

namespace game::factory {

/**
 * @brief 蓝图管理器，用于存储、管理所有蓝图
 * 
 * 它具有从json数据中加载蓝图并保存到容器，和获取指定ID的蓝图的功能。蓝图信息将由实体工厂使用。
 */
class BlueprintManager
{
public:
    BlueprintManager(engine::resource::ResourceManager& resourceManager);

private:
    engine::resource::ResourceManager& m_resourceManager;
};

} // namespace game::factory

#include "audio_system.h"
#include "../audio/audio_player.h"
#include "../component/audio_component.h"
#include "../core/context.h"
#include "../utils/events.h"

#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <spdlog/spdlog.h>

namespace engine::system {

AudioSystem::AudioSystem(entt::registry& registry, engine::core::Context& context)
    : m_registry{ registry }
    , m_context{ context }
{
    auto& dispatcher = m_context.dispatcher();
    dispatcher.sink<engine::utils::PlaySoundEvent>().connect<&AudioSystem::playSound>(this);
}

AudioSystem::~AudioSystem()
{
    auto& dispatcher = m_context.dispatcher();
    dispatcher.disconnect(this);
}

void AudioSystem::playSound(const engine::utils::PlaySoundEvent& event)
{
    // 如果没有传入目标实体，则直接播放全局音效
    if (event.m_entity == entt::null) {
        spdlog::info("播放全局音效 ID: {}", event.m_soundPathId);
        m_context.audioPlayer().playSound(event.m_soundPathId);
    } else if (auto* audioComponent = m_registry.try_get<engine::component::AudioComponent>(
                   event.m_entity);
               audioComponent != nullptr) { // 如果有传入目标实体，且实体拥有音频组件
        // 先尝试在目标实体的音效集合中查找
        auto it = audioComponent->m_sounds.find(event.m_soundPathId);
        if (it != audioComponent->m_sounds.end()) {
            spdlog::info("实体 ID: {} 中找到了音效: {}",
                         entt::to_integral(event.m_entity),
                         it->second);
            m_context.audioPlayer().playSound(it->second);
        } else { // 如果没找到，则播放全局音效
            spdlog::info("实体 ID: {} 中没有找到音效 ID: {}",
                         entt::to_integral(event.m_entity),
                         event.m_soundPathId);
            m_context.audioPlayer().playSound(event.m_soundPathId);
        }
    } else { // 如果有传入目标实体，但实体没有音频组件，也尝试播放全局音效
        spdlog::info("实体 ID: {} 中没有音频组件，尝试播放全局音效 ID: {}",
                     entt::to_integral(event.m_entity),
                     event.m_soundPathId);
        m_context.audioPlayer().playSound(event.m_soundPathId);
    }
}

} // namespace engine::system

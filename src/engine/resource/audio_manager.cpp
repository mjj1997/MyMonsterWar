#include "audio_manager.h"

#include <entt/core/hashed_string.hpp>
#include <spdlog/spdlog.h>

#include <stdexcept>

namespace engine::resource {

AudioManager::AudioManager()
{
    // 初始化SDL_mixer
    // SDL_mixer 3.2.0 不再需要传入格式标志
    if (!MIX_Init()) {
        throw std::runtime_error{ "AudioManager 错误: MIX_Init 失败: "
                                  + std::string(SDL_GetError()) };
    }

    // 创建连接到默认音频输出设备的混音器
    m_mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
    if (m_mixer == nullptr) {
        MIX_Quit();
        throw std::runtime_error{ "AudioManager 错误: MIX_CreateMixerDevice 失败: "
                                  + std::string(SDL_GetError()) };
    }

    spdlog::trace("AudioManager 构造成功。");
}

AudioManager::~AudioManager()
{
    // 立即停止所有音频播放
    MIX_StopAllTracks(m_mixer, 0);

    // 清理资源映射（unique_ptrs会调用删除器）
    clearAudio();

    // 销毁混音器（同时关闭音频设备）
    MIX_DestroyMixer(m_mixer);

    // 退出SDL_mixer子系统
    MIX_Quit();
    spdlog::trace("AudioManager 析构成功。");
}

// --- 音效管理 ---
MIX_Audio* AudioManager::loadSound(entt::id_type id, std::string_view filePath)
{
    // 首先检查缓存
    auto iter = m_sounds.find(id);
    if (iter != m_sounds.end()) {
        return iter->second.get();
    }

    // 加载音效（predecode=true，预先解码为PCM，适合短音效）
    spdlog::debug("加载音效: {}", filePath);
    MIX_Audio* sound{ MIX_LoadAudio(m_mixer, filePath.data(), true) };
    if (sound == nullptr) {
        spdlog::error("加载音效失败: '{}': {}", filePath, SDL_GetError());
        return nullptr;
    }

    // 使用unique_ptr存储在缓存中
    m_sounds.emplace(id, std::unique_ptr<MIX_Audio, SDLMixAudioDeletor>{ sound });
    spdlog::debug("成功加载并缓存音效: {}", filePath);
    return sound;
}

MIX_Audio* AudioManager::loadSound(entt::hashed_string hs)
{
    return loadSound(hs.value(), hs.data());
}

MIX_Audio* AudioManager::getSound(entt::id_type id, std::string_view filePath)
{
    // 查找现有音效
    auto iter = m_sounds.find(id);
    if (iter != m_sounds.end()) {
        return iter->second.get();
    }

    // 如果未找到音效，尝试加载音效
    if (filePath.empty()) {
        spdlog::error("音效 '{}' 未找到缓存，且未提供文件路径，返回 nullptr。", id);
        return nullptr;
    }

    spdlog::info("音效 '{}' 未找到缓存，尝试加载。", filePath);
    return loadSound(id, filePath);
}

MIX_Audio* AudioManager::getSound(entt::hashed_string hs)
{
    return getSound(hs.value(), hs.data());
}

void AudioManager::unloadSound(entt::id_type id)
{
    auto iter = m_sounds.find(id);
    if (iter != m_sounds.end()) {
        spdlog::debug("成功卸载音效: id = {}", id);
        m_sounds.erase(iter); // unique_ptr 处理 MIX_DestroyAudio
    } else {
        spdlog::warn("尝试卸载不存在的音效: id = {}", id);
    }
}

void AudioManager::clearSounds()
{
    if (!m_sounds.empty()) {
        spdlog::debug("正在清除所有 {} 个缓存的音效。", m_sounds.size());
        m_sounds.clear(); // unique_ptr 处理删除
    }
}

// --- 音乐管理 ---
MIX_Audio* AudioManager::loadMusic(entt::id_type id, std::string_view filePath)
{
    // 首先检查缓存
    auto iter = m_musics.find(id);
    if (iter != m_musics.end()) {
        return iter->second.get();
    }

    // 加载音乐（predecode=false，流式解码，适合长音乐）
    spdlog::debug("加载音乐: {}", filePath);
    MIX_Audio* music{ MIX_LoadAudio(m_mixer, filePath.data(), false) };
    if (music == nullptr) {
        spdlog::error("加载音乐失败: '{}': {}", filePath, SDL_GetError());
        return nullptr;
    }

    // 使用unique_ptr存储在缓存中
    m_musics.emplace(id, std::unique_ptr<MIX_Audio, SDLMixAudioDeletor>{ music });
    spdlog::debug("成功加载并缓存音乐: {}", filePath);
    return music;
}

MIX_Audio* AudioManager::loadMusic(entt::hashed_string hs)
{
    return loadMusic(hs.value(), hs.data());
}

MIX_Audio* AudioManager::getMusic(entt::id_type id, std::string_view filePath)
{
    // 查找现有音乐
    auto iter = m_musics.find(id);
    if (iter != m_musics.end()) {
        return iter->second.get();
    }

    // 如果未找到音乐，尝试加载音乐
    if (filePath.empty()) {
        spdlog::error("音乐 '{}' 未找到缓存，且未提供文件路径，返回 nullptr。", id);
        return nullptr;
    }

    spdlog::info("音乐 '{}' 未找到缓存，尝试加载。", filePath);
    return loadMusic(id, filePath);
}

MIX_Audio* AudioManager::getMusic(entt::hashed_string hs)
{
    return getMusic(hs.value(), hs.data());
}

void AudioManager::unloadMusic(entt::id_type id)
{
    auto iter = m_musics.find(id);
    if (iter != m_musics.end()) {
        spdlog::debug("卸载音乐: id = {}", id);
        m_musics.erase(iter); // unique_ptr 处理 MIX_DestroyAudio
    } else {
        spdlog::warn("尝试卸载不存在的音乐: id = {}", id);
    }
}

void AudioManager::clearMusics()
{
    if (!m_musics.empty()) {
        spdlog::debug("正在清除所有 {} 个缓存的音乐曲目。", m_musics.size());
        m_musics.clear(); // unique_ptr 处理删除
    }
}

void AudioManager::clearAudio()
{
    clearSounds();
    clearMusics();
}

} // namespace engine::resource

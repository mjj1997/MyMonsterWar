#pragma once

#include <SDL3_mixer/SDL_mixer.h> // SDL_mixer 主头文件
#include <entt/core/fwd.hpp>

#include <memory> // 用于 std::unique_ptr
#include <string_view>
#include <unordered_map>

namespace engine::resource {

/**
 * @brief 管理 SDL_mixer 音效和音乐(统一为 MIX_Audio 类型)。
 *
 * 提供音频资源的加载和缓存功能。构造失败时会抛出异常。
 * 仅供 ResourceManager 内部使用。
 */
class AudioManager final
{
    friend class ResourceManager;

public:
    /**
     * @brief 构造函数。初始化 SDL_mixer 并创建音频设备混音器。
     * @throws std::runtime_error 如果 SDL_mixer 初始化或创建混音器失败。
     */
    AudioManager();

    ~AudioManager(); ///< @brief 需要手动添加析构函数，清理资源并关闭 SDL_mixer。

    // 当前设计中，我们只需要一个AudioManager，所有权不变，所以不需要拷贝、移动相关构造及赋值运算符
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;
    AudioManager(AudioManager&&) = delete;
    AudioManager& operator=(AudioManager&&) = delete;

private: // 仅供 ResourceManager 访问的方法
    // --- 音效管理 ---
    /**
     * @brief 从文件路径加载音效（预解码）
     * @param id 音效的唯一标识符, 通过 entt::hashed_string 生成
     * @param filePath 音效文件的路径
     * @return 加载的音效的指针
     * @note 如果音效已经加载，则返回已加载音效的指针
     * @note 如果音效未加载，则从文件路径加载音效，并返回加载的音效的指针
     */
    MIX_Audio* loadSound(entt::id_type id, std::string_view filePath);

    /**
     * @brief 从字符串哈希值加载音效
     * @param hs entt::hashed_string 类型
     * @return 加载的音效的指针
     * @note 如果音效已经加载，则返回已加载音效的指针
     * @note 如果音效未加载，则从哈希字符串对应的文件路径加载音效，并返回加载的音效的指针
     */
    MIX_Audio* loadSound(entt::hashed_string hs);

    /**
     * @brief 从文件路径获取音效
     * @param id 音效的唯一标识符, 通过 entt::hashed_string 生成
     * @return 加载的音效的指针
     * @note 如果音效已经加载，则返回已加载音效的指针
     * @note 如果音效未加载，且提供了 filePath，则尝试从文件路径加载音效，并返回加载的音效的指针
     * @note 如果音效未加载，且没有提供 filePath，则返回 nullptr
     */
    MIX_Audio* getSound(entt::id_type id, std::string_view filePath = "");

    /**
     * @brief 从字符串哈希值获取音效
     * @param hs entt::hashed_string 类型
     * @return 加载的音效的指针
     * @note 如果音效已经加载，则返回已加载音效的指针
     * @note 如果音效未加载，则从哈希字符串对应的文件路径加载音效，并返回加载的音效的指针
     */
    MIX_Audio* getSound(entt::hashed_string hs);

    /**
     * @brief 卸载指定的音效资源
     * @param id 音效的唯一标识符, 通过 entt::hashed_string 生成
     */
    void unloadSound(entt::id_type id);

    /**
     * @brief 清空所有音效资源
     */
    void clearSounds();

    // --- 音乐管理 ---
    /**
     * @brief 从文件路径加载音乐（流式解码）
     * @param id 音乐的唯一标识符, 通过 entt::hashed_string 生成
     * @param filePath 音乐文件的路径
     * @return 加载的音乐的指针
     * @note 如果音乐已经加载，则返回已加载音乐的指针
     * @note 如果音乐未加载，则从文件路径加载音乐，并返回加载的音乐的指针
     */
    MIX_Audio* loadMusic(entt::id_type id, std::string_view filePath);

    /**
     * @brief 从字符串哈希值加载音乐
     * @param hs entt::hashed_string 类型
     * @return 加载的音乐的指针
     * @note 如果音乐已经加载，则返回已加载音乐的指针
     * @note 如果音乐未加载，则从哈希字符串对应的文件路径加载音乐，并返回加载的音乐的指针
     */
    MIX_Audio* loadMusic(entt::hashed_string hs);

    /**
     * @brief 从文件路径获取音乐
     * @param id 音乐的唯一标识符, 通过 entt::hashed_string 生成
     * @return 加载的音乐的指针
     * @note 如果音乐已经加载，则返回已加载音乐的指针
     * @note 如果音乐未加载，且提供了 filePath，则尝试从文件路径加载音乐，并返回加载的音乐的指针
     * @note 如果音乐未加载，且没有提供 filePath，则返回 nullptr
     */
    MIX_Audio* getMusic(entt::id_type id, std::string_view filePath = "");

    /**
     * @brief 从字符串哈希值获取音乐
     * @param hs entt::hashed_string 类型
     * @return 加载的音乐的指针
     * @note 如果音乐已经加载，则返回已加载音乐的指针
     * @note 如果音乐未加载，则从哈希字符串对应的文件路径加载音乐，并返回加载的音乐的指针
     */
    MIX_Audio* getMusic(entt::hashed_string hs);

    /**
     * @brief 卸载指定的音乐资源
     * @param id 音乐的唯一标识符, 通过 entt::hashed_string 生成
     */
    void unloadMusic(entt::id_type id);

    /**
     * @brief 清空所有音乐资源
     */
    void clearMusics();

    /**
     * @brief 清空所有音频资源
     */
    void clearAudio();

    // --- getter ---
    MIX_Mixer* mixer() const { return m_mixer; }; ///< @brief 获取 SDL_mixer 混音器指针

    // MIX_Audio 的自定义删除器
    struct SDLMixAudioDeletor
    {
        void operator()(MIX_Audio* audio) const
        {
            if (audio) {
                MIX_DestroyAudio(audio);
            }
        }
    };

    // 音效存储 (文件路径 -> MIX_Audio, 预解码)
    std::unordered_map<entt::id_type, std::unique_ptr<MIX_Audio, SDLMixAudioDeletor>> m_sounds;
    // 音乐存储 (文件路径 -> MIX_Audio, 流式解码)
    std::unordered_map<entt::id_type, std::unique_ptr<MIX_Audio, SDLMixAudioDeletor>> m_musics;

    MIX_Mixer* m_mixer{ nullptr }; ///< @brief SDL_mixer 混音器实例
};

} // namespace engine::resource

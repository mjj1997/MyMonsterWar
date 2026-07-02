#pragma once

#include "../utils/string_view_hash.h"

#include <SDL3/SDL_render.h>
#include <entt/signal/sigh.hpp>
#include <glm/vec2.hpp>

#include <array>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace engine::core {
class Configurator;
}

namespace engine::input {

enum class ActionState {
    Pressed,  ///< @brief 动作在本帧刚刚被按下
    Held,     ///< @brief 动作被持续按下
    Released, ///< @brief 动作在本帧刚刚被释放
    Inactive  ///< @brief 动作未激活
};

using InputKey = std::variant<SDL_Scancode, Uint32>;

class InputManager final
{
public:
    /**
     * @brief 构造函数
     * @param sdlRenderer 指向 SDL_Renderer 的指针
     * @param dispatcher 指向 entt::dispatcher 的指针
     * @param config 指向配置对象的指针
     * @throws std::runtime_error 如果任一指针为 nullptr。
     */
    InputManager(SDL_Renderer* sdlRenderer,
                 entt::dispatcher* dispatcher,
                 const engine::core::Configurator* config);

    void update(); ///< @brief 更新输入状态，每轮循环最先调用

    /**
     * @brief 注册一个动作的回调函数
     * @param actionName 动作名称
     * @param actionState 动作状态, 默认为按下瞬间
     * @return 一个 sink 对象，用于注册回调函数
     */
    entt::sink<entt::sigh<bool()>> actionSignal(std::string_view actionName,
                                                ActionState actionState = ActionState::Pressed);

    // 动作状态检查
    ///< @brief 动作当前是否触发 (持续按下或本帧按下)
    bool isActionDown(std::string_view action) const;
    bool isActionPressed(std::string_view action) const;  ///< @brief 动作是否在本帧刚刚按下
    bool isActionReleased(std::string_view action) const; ///< @brief 动作是否在本帧刚刚释放

    glm::vec2 mousePosition() const;        ///< @brief 获取鼠标位置 （屏幕坐标）
    glm::vec2 logicalMousePosition() const; ///< @brief 获取鼠标位置 （逻辑坐标）

private:
    ///< @brief 根据 Configurator 配置初始化输入映射表
    void initInputMappings(const engine::core::Configurator* config);

    ///< @brief 触发退出信号
    void emitQuitSignal();
    ///< @brief 处理 SDL 事件（将按键转换为动作状态）
    void processEvent(const SDL_Event& event);

    ///< @brief 将字符串键名转换为 SDL_Scancode
    static SDL_Scancode scancodeFromString(std::string_view keyName);
    ///< @brief 将字符串按钮名转换为 SDL_Button
    static Uint32 mouseButtonUint32FromString(std::string_view buttonName);
    ///< @brief 辅助更新动作状态
    void updateActionState(std::string_view action, bool isInputActive, bool isRepeatEvent);

    ///< @brief 用于获取逻辑坐标的 SDL_Renderer 指针
    SDL_Renderer* m_sdlRenderer;
    ///< @brief 用于发送事件的 entt::dispatcher 指针
    entt::dispatcher* m_dispatcher;

    ///< @brief 从键盘（Scancode）或鼠标按钮 (Uint32) 到关联的动作名称列表
    std::unordered_map<InputKey, std::vector<std::string>> m_inputKeyToActions;

    std::unordered_map<std::string, ActionState, engine::utils::StringViewHash, std::equal_to<>>
        m_actionStates; ///< @brief 存储每个动作的当前状态

    /** @brief 核心数据结构: 存储动作名称函数列表的映射
     * 
     * @note 每个动作有3个状态: Pressed, Held, Released，每个状态对应一个回调函数
     * @note 绑定动作时再插入元素（懒加载），初始化时为空
     */
    std::unordered_map<std::string, std::array<entt::sigh<bool()>, 3>> m_actionToCallbacks;

    glm::vec2 m_mousePosition{ 0.0F, 0.0F };        ///< @brief 鼠标位置 (针对屏幕坐标)
    glm::vec2 m_logicalMousePosition{ 0.0F, 0.0F }; ///< @brief 鼠标位置 (针对逻辑坐标)
};

} // namespace engine::input

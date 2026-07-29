#pragma once

/**
 * 在 entt 中, 空的结构体可以作为"标签组件"使用
 * 它不占用内存空间, 当需要给实体添加标签时, 这是最推荐的做法
 */
namespace game::defs {

struct DeadTag // 死亡标签, 用于标记实体死亡并延时删除
{};

struct FacedLeftTag // 角色图片默认朝右，如果朝左就添加一个标签，用于翻转判断
{};

struct MeleeUnitTag // 近战单位标签
{};

struct RangedUnitTag // 远程单位标签
{};

struct HealerTag // 治疗单位标签
{};

struct InjuredTag // 受伤标签，用于标记实体受伤（有HP损失）
{};

struct AttackReadyTag // 攻击就绪标签，用于标记实体可以进行攻击（冷却完成）
{};

struct ActionLockedTag // 动作锁定标签，让角色播放完当前动画再进行下一步动作（硬直）
{};

} // namespace game::defs

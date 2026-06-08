# 项目组人员编码分工

当前项目是 C11 + raylib 图形化单机贪吃蛇，已经拆分为 `main.c`、`game.c`、`snake.c`、`food.c`、`ui.c`、`controls.c` 等模块。每名成员都有明确编码任务，代码量相对均衡。

| 成员 | 编码任务 | 负责文件 | 主要内容 |
| --- | --- | --- | --- |
| 成员1 | 程序入口和游戏状态 | `src/main.c`, `src/game.c`, `src/game.h` | 窗口初始化、主循环、状态切换、暂停继续、重新开始、返回菜单、固定时间步进、本局用时、音效开关、最高分重置。 |
| 成员2 | 蛇身移动和碰撞 | `src/snake.c`, `src/snake.h` | 蛇身数组、方向队列、禁止反向、蛇头下一格、蛇身跟随、吃食物增长、撞墙和撞自身判断。 |
| 成员3 | 食物、得分和动画反馈 | `src/food.c`, `src/food.h` | 食物随机生成、避免刷到蛇身、食物脉冲、吃食物粒子效果、扩散光圈、分数反馈和食物绘制。 |
| 成员4 | UI 绘制和视觉表现 | `src/ui.c`, `src/ui.h` | 产品化视觉层级、背景、棋盘边缘光、网格、连贯蛇身、蛇头眼睛、右侧仪表板、开始菜单、暂停和结束弹窗绘制。 |
| 成员5 | 控件交互、构建和文档 | `src/controls.c`, `src/controls.h`, `build.bat`, `CMakeLists.txt`, `README.md`, `CONTRIBUTING.md`, `TASKS.md` | 主操作和次级操作按钮布局、右侧功能按钮、弹窗按钮、鼠标点击、hover/pressed 状态、交互状态更新、编译脚本和文档同步。 |

## 技术要求

1. 项目整体使用 C11 和 raylib 实现。
2. 默认窗口大小为 `960x720`。
3. 不做控制台字符版，不做网页前端，不做多人联网。
4. 每名成员优先修改自己负责的文件。
5. 跨模块修改前先同步最新代码，并说明修改原因。

## 提交前检查

每次提交前至少运行一次：

```powershell
.\build.bat
```

或者只编译不启动游戏：

```powershell
gcc src\main.c src\game.c src\snake.c src\food.c src\ui.c src\controls.c -std=c11 -O2 -Wall -Wextra -I"D:\raylib\include" -L"D:\raylib\lib" -lraylib -lopengl32 -lgdi32 -lwinmm -o snake_raylib.exe
```

## 提交建议

```powershell
git status
git diff
git add src\自己负责的文件.c src\自己负责的文件.h
git commit -m "完成某某模块"
git push origin main
```

提交信息建议：

- `拆分游戏状态模块`
- `完善蛇身移动和碰撞判断`
- `优化食物粒子和分数反馈`
- `完善菜单按钮和结束弹窗`
- `更新构建脚本和项目文档`

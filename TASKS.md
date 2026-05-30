# 项目组人员编码分工

当前项目是 C11 + raylib 图形化单机贪吃蛇，核心代码集中在 `src/main.c`。为了满足每名成员都有编码任务的要求，按函数区域划分责任；提交前要先同步最新代码，避免多人同时修改同一段函数。

| 成员 | 编码任务 | 主要负责区域 |
| --- | --- | --- |
| 成员1 | 主程序框架与游戏状态管理 | `main`、`InitGame`、`ResetGame`、`StartOrResumeGame`、`TogglePause`、`UpdateGame`，负责窗口初始化、游戏状态、固定时间步进、暂停、继续、重开和整体流程整合。 |
| 成员2 | 蛇身移动与碰撞规则 | `Snake` 数据、`QueueDirection`、`MoveSnake`、`NextHead`、`SameCell`、`SnakeContains`，负责方向控制、禁止反向、蛇身跟随、增长、撞墙和撞自身判断。 |
| 成员3 | 食物、计分、难度和本地数据 | `Food` 数据、`SpawnFood`、`LoadHighScore`、`SaveHighScore`、`UpdateDifficulty`、`DrawFood`，负责食物生成、得分、最高分保存、等级速度变化和食物动画。 |
| 成员4 | 棋盘、蛇身和背景视觉 | `DrawBackground`、`DrawHeader`、`DrawBoardFrame`、`DrawBoardGrid`、`DrawSnake`、`DrawSnakeEyes`，负责街机背景、棋盘边框、网格、蛇身层次和蛇头朝向表现。 |
| 成员5 | 面板、按钮、弹窗和构建说明 | `HandleButtons`、`ActivateButton`、`DrawSidePanel`、`DrawButton`、`DrawOverlay`、`DrawRoundedPanel`、`DrawTextFit`、`ButtonLabel`、`build.bat`、`CMakeLists.txt`，负责鼠标交互、UI 面板、弹窗按钮、文字适配和编译脚本。 |

## 技术要求

1. 项目整体使用 C11 和 raylib 实现。
2. 默认窗口大小为 `960x720`。
3. 不做控制台字符版，不做网页前端，不做多人联网。
4. 每名成员都要修改或维护自己负责的函数区域，报告中按函数说明贡献。
5. 如果需要跨区域修改，先和对应成员说明，避免覆盖代码。

## 提交前检查

每次提交前至少运行一次：

```powershell
.\build.bat
```

或者只编译不启动游戏：

```powershell
gcc src\main.c -std=c11 -O2 -Wall -Wextra -I"D:\raylib\include" -L"D:\raylib\lib" -lraylib -lopengl32 -lgdi32 -lwinmm -o snake_raylib.exe
```

## 提交建议

由于当前是单文件实现，提交前要先看自己改了哪些函数：

```powershell
git diff src\main.c
```

提交信息建议：

- `完善蛇身移动和碰撞判断`
- `优化食物动画和难度变化`
- `完善按钮和结束弹窗`
- `修正构建脚本和说明文档`

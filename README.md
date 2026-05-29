# 贪吃蛇 Raylib 图形版

这是一个使用 C11 + raylib 实现的窗口化单机贪吃蛇游戏。项目不再使用控制台字符界面，主程序为 `src/main.c`，包含完整的图形界面、输入、状态管理、固定时间步进、音效和最高分保存。

## 功能

- 标准贪吃蛇玩法：移动、吃食物、增长、加分
- 撞墙或撞到自己后游戏结束
- 支持开始、暂停、继续、重新开始
- 支持方向键和 `W/A/S/D`
- `P` 或 `Space` 暂停 / 继续
- `R` 重新开始
- 游戏结束后 `Enter` 或按钮重新开始
- 当前分数、最高分、等级、速度状态显示
- 分数提升后逐步加快速度
- 吃到食物时有动画反馈和轻微音效
- 暂停和游戏结束时有半透明覆盖弹窗
- 鼠标可点击 Start / Pause / Restart 按钮

## UI 设计

- 默认窗口：`960x720`
- 深色街机背景 + 轻微网格纹理
- 棋盘居中展示，带边框、阴影和圆角视觉效果
- 右侧信息面板显示状态、分数、最高分、等级、速度和按钮
- 蛇身使用圆角节点和颜色层次
- 蛇头带眼睛，可看出朝向
- 食物带发光和脉冲动画
- 按钮包含 hover 和 pressed 状态

## 文件结构

```text
tanchishe/
  src/
    main.c          raylib 图形版完整游戏代码
  CMakeLists.txt    CMake 构建文件
  build.bat         Windows + gcc + raylib 快速编译脚本
```

## 编译方式

### 方式一：Windows gcc

假设 raylib 安装在 `C:\raylib\raylib`：

```powershell
gcc src\main.c -std=c11 -O2 -Wall -Wextra -I"C:\raylib\raylib\src" -L"C:\raylib\raylib\src" -lraylib -lopengl32 -lgdi32 -lwinmm -o snake_raylib.exe
.\snake_raylib.exe
```

如果 raylib 在其他位置，可以先设置环境变量：

```powershell
$env:RAYLIB_PATH="D:\your\path\raylib"
.\build.bat
```

### 方式二：CMake

系统中已安装 raylib 后：

```powershell
cmake -S . -B build
cmake --build build
.\build\snake_raylib.exe
```

如果 CMake 找不到 raylib，也可以先设置：

```powershell
$env:RAYLIB_PATH="C:\raylib\raylib"
```

不同编译器的输出路径可能略有差异，例如 Visual Studio 生成器可能在 `build\Debug\snake_raylib.exe`。

## 操作按键

| 按键 | 功能 |
| --- | --- |
| 方向键 | 控制移动 |
| `W/A/S/D` | 控制移动 |
| `Space` | 开始、暂停或继续 |
| `P` | 暂停或继续 |
| `R` | 重新开始 |
| `Enter` | 游戏结束后重新开始 |

## 最高分

最高分保存在本地文件：

```text
snake_raylib_highscore.dat
```

该文件是运行时生成的本地数据，不需要提交到 GitHub。

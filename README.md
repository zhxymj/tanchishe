# 贪吃蛇 Raylib 图形版

这是一个使用 C11 + raylib 实现的窗口化单机贪吃蛇游戏。项目保持 `960x720` 默认窗口，不使用控制台字符界面、不做网页前端、不做多人联网。

## 功能

- 标准贪吃蛇玩法：移动、吃食物、增长、加分
- 撞墙或撞到自己后游戏结束
- 开始菜单：开始游戏、难度、音效、重置纪录、退出
- 界面文字已中文化，运行时加载 Windows 中文字体
- 难度选择：Easy、Normal、Hard，不同初始速度和加速曲线
- 支持开始、暂停、继续、重新开始、返回菜单
- 支持方向键和 `W/A/S/D`
- `P` 或 `Space` 暂停 / 继续
- `R` 重新开始，`M` 返回菜单，`Tab` 在菜单中切换难度
- 当前分数、最高分、本局用时、等级、速度状态显示
- 支持音效开关和最高分重置按钮
- 吃到食物时有发光、粒子、扩散光圈和分数缩放反馈
- 暂停和游戏结束时有半透明淡入弹窗
- 鼠标可点击菜单、面板和弹窗按钮

## UI 设计

- 明亮竞技场背景 + 轻微网格纹理，整体向“贪吃蛇大作战”式休闲竞技视觉靠近
- 中文标题、按钮、状态、难度、分数面板和弹窗
- 棋盘居中展示，边界弱化，保留柔和边缘光、圆角和层次阴影
- 右侧信息面板采用轻量玻璃感信息卡，显示状态、难度、分数、最高分、时间、等级、速度和功能按钮
- 蛇身使用连贯圆形节点、连接段和颜色层次
- 蛇头带眼睛，可看出朝向
- 食物使用糖果色强发光、脉冲动画、吃到后的粒子和光圈扩散
- 按钮区分主操作和次级操作，包含 hover、pressed、颜色层级、轻微位移和亮度反馈
- 默认使用 `C:\Windows\Fonts\simhei.ttf` 绘制中文 UI

## 文件结构

```text
tanchishe/
  src/
    main.c          程序入口和主循环
    game.c/.h       游戏状态、更新、重开、暂停、分数、速度、本局用时、音效开关
    snake.c/.h      蛇身数据、方向、移动、增长、撞墙和撞自身
    food.c/.h       食物生成、动画、得分反馈、粒子和光圈效果
    ui.c/.h         背景、棋盘、面板、按钮、菜单和弹窗绘制
    controls.c/.h   五按钮菜单、按钮、鼠标点击和交互状态更新
  CMakeLists.txt    CMake 构建文件
  build.bat         Windows + gcc + raylib 快速编译脚本
```

## 编译方式

### 方式一：Windows gcc

默认按本机安装路径 `D:\raylib` 编译：

```powershell
gcc src\main.c src\game.c src\snake.c src\food.c src\ui.c src\controls.c -std=c11 -O2 -Wall -Wextra -I"D:\raylib\include" -L"D:\raylib\lib" -lraylib -lopengl32 -lgdi32 -lwinmm -o snake_raylib.exe
.\snake_raylib.exe
```

也可以直接运行：

```powershell
.\build.bat
```

如果 raylib 在其他位置，可以先设置环境变量：

```powershell
$env:RAYLIB_PATH="D:\raylib"
.\build.bat
```

### 方式二：CMake

```powershell
cmake -S . -B build
cmake --build build
.\build\snake_raylib.exe
```

如果项目所在路径包含中文字符，部分 Windows 版 `ninja` 或 `mingw32-make` 可能无法解析 CMake 生成的绝对路径。遇到这种情况时，优先使用 `.\build.bat`，或者把仓库移动到纯英文路径后再使用 CMake，例如 `D:\code\tanchishe`。

## 操作按键

| 按键 | 功能 |
| --- | --- |
| 方向键 | 控制移动 |
| `W/A/S/D` | 控制移动 |
| `Space` | 开始、暂停或继续 |
| `P` | 暂停或继续 |
| `R` | 重新开始 |
| `Enter` | 菜单 / 暂停 / 游戏结束时确认 |
| `Tab` | 在菜单或结束界面切换难度 |
| `M` | 返回主菜单 |

## 最高分

最高分保存在本地文件：

```text
snake_raylib_highscore.dat
```

该文件是运行时生成的本地数据，不需要提交到 GitHub。

# 提交材料说明

本目录是可以提交给老师检查的贪吃蛇项目材料。项目使用 `C11 + raylib` 实现窗口化单机游戏，默认窗口大小为 `960x720`。

## 建议提交内容

- `src/`：完整可编译源码。
- `member_code/`：五名成员按分工拆开的代码副本和个人说明。
- `README.md`：项目功能、文件结构、编译方式和操作说明。
- `TASKS.md`：五名成员编码分工表。
- `CONTRIBUTING.md`：小组协作、同步代码、提交代码说明。
- `docs/`：项目分析、协作教程和五人详细步骤安排。
- `build.bat`、`CMakeLists.txt`：Windows gcc 和 CMake 编译入口。

## 不建议提交内容

- `build/`、`*.exe`、`*.obj` 等编译产物。
- `snake_raylib_highscore.dat` 等运行时数据。
- `.git/`、编辑器配置、临时文件。
- 文档生成脚本、测试生成脚本等非展示材料。

## 五人代码分工

| 成员 | 文件夹 | 对应代码 |
| --- | --- | --- |
| 成员1 | `member_code/member1_main_game` | `main.c`, `game.c`, `game.h` |
| 成员2 | `member_code/member2_snake` | `snake.c`, `snake.h` |
| 成员3 | `member_code/member3_food` | `food.c`, `food.h` |
| 成员4 | `member_code/member4_ui_visual` | `ui.c`, `ui.h` |
| 成员5 | `member_code/member5_controls_build_docs` | `controls.c`, `controls.h`, `build.bat`, `CMakeLists.txt` |

## 编译运行

raylib 默认安装在 `D:\raylib` 时，直接运行：

```powershell
.\build.bat
```

只编译不启动游戏时运行：

```powershell
gcc src\main.c src\game.c src\snake.c src\food.c src\ui.c src\controls.c -std=c11 -O2 -Wall -Wextra -I"D:\raylib\include" -L"D:\raylib\lib" -lraylib -lopengl32 -lgdi32 -lwinmm -o snake_raylib.exe
```

如果 raylib 不在 `D:\raylib`，先设置 `RAYLIB_PATH` 环境变量，再运行 `build.bat`。

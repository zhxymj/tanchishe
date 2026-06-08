# 小组提交说明

这个项目是 `C11 + raylib` 图形化单机贪吃蛇，代码已经拆分成多个模块。提交代码时优先只提交自己负责的文件，避免多人互相覆盖。

## 写代码前同步

```powershell
git pull origin main
git status
```

如果本地已经改了代码，先确认修改范围，再决定提交或 `git stash`。

## 编译检查

推荐直接运行：

```powershell
.\build.bat
```

只想编译、不想通过脚本启动时运行：

```powershell
gcc src\main.c src\game.c src\snake.c src\food.c src\ui.c src\controls.c -std=c11 -O2 -Wall -Wextra -I"D:\raylib\include" -L"D:\raylib\lib" -lraylib -lopengl32 -lgdi32 -lwinmm -o snake_raylib.exe
```

## 文件分工

| 成员 | 负责文件 |
| --- | --- |
| 成员1 | `src/main.c`, `src/game.c`, `src/game.h` |
| 成员2 | `src/snake.c`, `src/snake.h` |
| 成员3 | `src/food.c`, `src/food.h` |
| 成员4 | `src/ui.c`, `src/ui.h` |
| 成员5 | `src/controls.c`, `src/controls.h`, `build.bat`, `CMakeLists.txt`, `README.md`, `TASKS.md`, `CONTRIBUTING.md` |

## 提交流程

```powershell
git status
git diff
git add src\自己负责的文件.c src\自己负责的文件.h
git commit -m "说明本次完成的功能"
git push origin main
```

如果只改了文档或构建脚本：

```powershell
git add build.bat CMakeLists.txt README.md TASKS.md CONTRIBUTING.md
git commit -m "更新构建和协作说明"
git push origin main
```

## 注意事项

- 不要提交 `snake_raylib.exe`、`build/`、最高分数据文件等运行产物。
- 提交前必须至少编译一次。
- 修改公共结构 `src/game.h` 前先确认其他模块是否受影响。
- 修改按钮数量时，需要同时更新 `MENU_BUTTON_COUNT`、`ControlsInit`、`ControlsUpdate` 和 UI 绘制循环。
- 新增中文 UI 文案时，需要同步更新 `src/ui.c` 中的 `UI_FONT_CHARS`，否则新增汉字可能无法显示。
- 如果多人同时修改公共头文件 `src/game.h` 产生冲突，保留双方有效代码后再提交。

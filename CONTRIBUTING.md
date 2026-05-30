# 小组提交说明

这个项目现在是 `C11 + raylib` 图形化单机贪吃蛇。核心代码在 `src/main.c`，小组成员按函数区域协作，不再使用旧的 `game.c`、`snake.c`、`food.c`、`collision.c`、`ui.c` 拆分。

## 写代码前同步

```powershell
git pull origin main
```

如果本地已经改了代码，先运行：

```powershell
git status
```

确认没有误改其他成员负责的函数区域。

## 编译检查

推荐直接运行：

```powershell
.\build.bat
```

只想编译、不想通过脚本启动时运行：

```powershell
gcc src\main.c -std=c11 -O2 -Wall -Wextra -I"D:\raylib\include" -L"D:\raylib\lib" -lraylib -lopengl32 -lgdi32 -lwinmm -o snake_raylib.exe
```

## 函数区域分工

| 成员 | 负责内容 |
| --- | --- |
| 成员1 | 主循环、初始化、重置、暂停继续、固定时间步进和游戏状态流转 |
| 成员2 | 蛇身结构、方向控制、移动增长、撞墙和撞自身规则 |
| 成员3 | 食物生成、分数、等级、速度、最高分保存和食物反馈 |
| 成员4 | 背景、棋盘、网格、蛇身、蛇头眼睛等主要视觉绘制 |
| 成员5 | 鼠标按钮、右侧面板、覆盖弹窗、文字适配、构建脚本和说明文档 |

## 提交流程

```powershell
git status
git diff src\main.c
git add src\main.c
git commit -m "说明本次完成的功能"
git push origin main
```

如果只改了构建或说明文档，也只提交对应文件：

```powershell
git add build.bat CMakeLists.txt README.md TASKS.md CONTRIBUTING.md
git commit -m "更新构建和协作说明"
git push origin main
```

## 注意事项

- 不要提交 `snake_raylib.exe`、`build/`、最高分数据文件等运行产物。
- 修改 `src/main.c` 前先确认自己负责的函数区域。
- 提交前必须至少编译一次。
- 如果多人同时修改 `src/main.c` 产生冲突，保留双方有效代码后再提交。

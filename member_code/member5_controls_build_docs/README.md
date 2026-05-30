# 成员5：控件交互、构建和文档

## 负责文件

- `controls.c`
- `controls.h`
- `build.bat`
- `CMakeLists.txt`
- `PROJECT_README.md`
- `PROJECT_TASKS.md`
- `PROJECT_CONTRIBUTING.md`

## 主要任务

- 初始化菜单按钮、右侧功能按钮和弹窗按钮。
- 处理鼠标 hover、pressed 和点击事件。
- 将按钮点击转换为游戏操作，例如开始、暂停、重开、切换难度、开关音效、重置最高分。
- 维护按钮动画状态。
- 维护 Windows gcc 编译脚本和 CMake 配置。
- 保证项目说明和成员分工文档与代码一致。

## 需要说明的核心函数

- `ControlsInit`
- `ControlsUpdate`
- `UpdateButton`
- `ResetButton`

## 合并说明

修改完成后，将 `controls.c` 和 `controls.h` 合并回 `src/`，将构建脚本和项目说明合并回仓库根目录对应文件。

# 成员1：程序入口和游戏状态

## 负责文件

- `main.c`
- `game.c`
- `game.h`

## 主要任务

- 初始化 raylib 窗口和游戏数据。
- 维护中文窗口标题、中文游戏状态文本和中文难度文本。
- 维护主循环和固定时间步进。
- 管理主菜单、运行中、暂停、游戏结束状态。
- 处理开始、暂停、继续、重开、返回菜单。
- 管理分数、等级、速度、本局用时、最高分和纪录重置。
- 维护吃食物后的全局反馈计时，例如分数缩放和光圈持续时间。

## 需要说明的核心函数

- `main`
- `GameInit`
- `GameStart`
- `GameRestart`
- `GameTogglePause`
- `GameUpdate`
- `GameActivateButton`
- `GameSaveHighScore`

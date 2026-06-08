# 成员4：UI 绘制和视觉表现

## 负责文件

- `ui.c`
- `ui.h`

## 主要任务

- 绘制深色街机背景、网格纹理和产品化视觉层级。
- 加载中文字体，保证中文标题、按钮、状态和弹窗正常显示。
- 绘制棋盘边框、阴影、柔和边缘光和格子。
- 绘制连贯圆形蛇身、连接段、蛇头眼睛和方向表现。
- 绘制包含本局时间、速度、等级和最高分的右侧仪表板式信息面板。
- 绘制开始菜单、暂停弹窗和游戏结束弹窗。
- 保证文字、按钮、面板不重叠。

## 需要说明的核心函数

- `UIDrawGame`
- `DrawBackground`
- `DrawBoardFrame`
- `DrawBoardGrid`
- `DrawSnake`
- `DrawSidePanel`
- `DrawMenu`
- `DrawStateOverlay`

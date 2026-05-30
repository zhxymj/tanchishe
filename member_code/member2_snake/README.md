# 成员2：蛇身移动和碰撞

## 负责文件

- `snake.c`
- `snake.h`

## 主要任务

- 维护蛇身数组，约定 `body[0]` 为蛇头。
- 处理方向输入和禁止直接反向。
- 计算下一步蛇头位置。
- 移动蛇身，吃到食物时增长。
- 判断撞墙和撞到自身。

## 需要说明的核心函数

- `SnakeInit`
- `SnakeQueueDirection`
- `SnakeNextHead`
- `SnakeAdvance`
- `SnakeHitsWall`
- `SnakeHitsSelf`
- `SnakeContains`

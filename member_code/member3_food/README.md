# 成员3：食物、得分和动画反馈

## 负责文件

- `food.c`
- `food.h`

## 主要任务

- 随机生成食物，避免食物刷在蛇身上。
- 更新食物脉冲动画。
- 吃到食物后增加分数并触发反馈。
- 绘制糖果色强发光和脉冲效果。
- 生成吃食物后的少量粒子扩散动画和光圈扩散动画，保持 60 FPS。

## 需要说明的核心函数

- `FoodSpawn`
- `FoodUpdate`
- `FoodOnEaten`
- `FoodDraw`
- `EmitFoodParticles`

# 小组提交说明

这个项目已经拆成 5 个模块，方便每名成员分别提交自己负责的代码。

## 推荐流程

1. 写代码前先拉取最新版本：

```powershell
git pull origin main
```

2. 尽量只修改自己负责的模块文件。

3. 提交前先编译：

```powershell
gcc src\main.c src\game.c src\snake.c src\food.c src\collision.c src\ui.c -o snake.exe
```

4. 只提交自己负责的文件，例如：

```powershell
git add src\your_module.c src\your_module.h
git commit -m "完成某某模块"
git push origin main
```

## 文件分工

| 成员 | 负责文件 |
| --- | --- |
| 成员1 | `src/main.c`, `src/game.c`, `src/game.h` |
| 成员2 | `src/snake.c`, `src/snake.h` |
| 成员3 | `src/food.c`, `src/food.h` |
| 成员4 | `src/collision.c`, `src/collision.h` |
| 成员5 | `src/ui.c`, `src/ui.h` |

## 规则

- 不要随便改公共函数名，否则其他模块可能编译失败。
- 如果需要修改 `src/types.h` 或 `src/config.h`，先和组员说明。
- 每次提交前至少保证项目可以编译。
- 不要提交 `snake.exe`，它已经被 `.gitignore` 忽略。

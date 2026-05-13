from pathlib import Path

from reportlab.lib import colors
from reportlab.lib.enums import TA_CENTER, TA_LEFT
from reportlab.lib.pagesizes import A4
from reportlab.lib.styles import ParagraphStyle, getSampleStyleSheet
from reportlab.lib.units import cm, mm
from reportlab.pdfbase import pdfmetrics
from reportlab.pdfbase.ttfonts import TTFont
from reportlab.platypus import (
    PageBreak,
    Paragraph,
    Preformatted,
    SimpleDocTemplate,
    Spacer,
    Table,
    TableStyle,
)


ROOT = Path(__file__).resolve().parents[1]
OUTPUT = ROOT / "docs" / "贪吃蛇项目协作使用教程.pdf"
FONT_PATH = Path(r"C:\Windows\Fonts\simhei.ttf")


def register_fonts() -> str:
    if FONT_PATH.exists():
        pdfmetrics.registerFont(TTFont("SimHei", str(FONT_PATH)))
        return "SimHei"
    return "Helvetica"


def para(text, style):
    return Paragraph(text, style)


def code(text, styles):
    return Preformatted(text.strip(), styles["CodeBlock"])


def section(title, styles):
    return [Spacer(1, 8), Paragraph(title, styles["Heading1"]), Spacer(1, 4)]


def subsection(title, styles):
    return [Spacer(1, 6), Paragraph(title, styles["Heading2"]), Spacer(1, 2)]


def bullet(text, styles):
    return Paragraph(f"• {text}", styles["Body"])


def page_footer(canvas, doc):
    canvas.saveState()
    canvas.setFont("SimHei" if "SimHei" in pdfmetrics.getRegisteredFontNames() else "Helvetica", 9)
    canvas.setFillColor(colors.HexColor("#666666"))
    canvas.drawString(2.0 * cm, 1.2 * cm, "贪吃蛇项目协作使用教程")
    canvas.drawRightString(A4[0] - 2.0 * cm, 1.2 * cm, f"第 {doc.page} 页")
    canvas.restoreState()


def build_styles(font_name: str):
    base = getSampleStyleSheet()
    styles = {
        "Title": ParagraphStyle(
            "Title",
            parent=base["Title"],
            fontName=font_name,
            fontSize=24,
            leading=32,
            alignment=TA_CENTER,
            textColor=colors.HexColor("#1f2937"),
            spaceAfter=12,
        ),
        "Subtitle": ParagraphStyle(
            "Subtitle",
            parent=base["BodyText"],
            fontName=font_name,
            fontSize=12,
            leading=18,
            alignment=TA_CENTER,
            textColor=colors.HexColor("#4b5563"),
            wordWrap="CJK",
        ),
        "Heading1": ParagraphStyle(
            "Heading1",
            parent=base["Heading1"],
            fontName=font_name,
            fontSize=16,
            leading=22,
            textColor=colors.HexColor("#0f766e"),
            spaceBefore=10,
            spaceAfter=6,
            wordWrap="CJK",
        ),
        "Heading2": ParagraphStyle(
            "Heading2",
            parent=base["Heading2"],
            fontName=font_name,
            fontSize=12.5,
            leading=18,
            textColor=colors.HexColor("#374151"),
            spaceBefore=6,
            spaceAfter=4,
            wordWrap="CJK",
        ),
        "Body": ParagraphStyle(
            "Body",
            parent=base["BodyText"],
            fontName=font_name,
            fontSize=10.5,
            leading=17,
            alignment=TA_LEFT,
            textColor=colors.HexColor("#111827"),
            spaceAfter=5,
            wordWrap="CJK",
        ),
        "Small": ParagraphStyle(
            "Small",
            parent=base["BodyText"],
            fontName=font_name,
            fontSize=9,
            leading=14,
            textColor=colors.HexColor("#4b5563"),
            wordWrap="CJK",
        ),
        "CodeBlock": ParagraphStyle(
            "CodeBlock",
            parent=base["Code"],
            fontName=font_name,
            fontSize=9.2,
            leading=13.5,
            textColor=colors.HexColor("#111827"),
            backColor=colors.HexColor("#f3f4f6"),
            borderColor=colors.HexColor("#d1d5db"),
            borderWidth=0.4,
            borderPadding=6,
            leftIndent=0,
            rightIndent=0,
            wordWrap="CJK",
        ),
        "TableHead": ParagraphStyle(
            "TableHead",
            parent=base["BodyText"],
            fontName=font_name,
            fontSize=9.5,
            leading=13,
            textColor=colors.white,
            alignment=TA_CENTER,
            wordWrap="CJK",
        ),
        "TableCell": ParagraphStyle(
            "TableCell",
            parent=base["BodyText"],
            fontName=font_name,
            fontSize=9,
            leading=13,
            textColor=colors.HexColor("#111827"),
            wordWrap="CJK",
        ),
    }
    return styles


def table(data, col_widths):
    t = Table(data, colWidths=col_widths, repeatRows=1, hAlign="LEFT")
    t.setStyle(
        TableStyle(
            [
                ("BACKGROUND", (0, 0), (-1, 0), colors.HexColor("#0f766e")),
                ("TEXTCOLOR", (0, 0), (-1, 0), colors.white),
                ("GRID", (0, 0), (-1, -1), 0.4, colors.HexColor("#d1d5db")),
                ("BACKGROUND", (0, 1), (-1, -1), colors.HexColor("#ffffff")),
                ("VALIGN", (0, 0), (-1, -1), "MIDDLE"),
                ("LEFTPADDING", (0, 0), (-1, -1), 6),
                ("RIGHTPADDING", (0, 0), (-1, -1), 6),
                ("TOPPADDING", (0, 0), (-1, -1), 5),
                ("BOTTOMPADDING", (0, 0), (-1, -1), 5),
            ]
        )
    )
    return t


def build_story(styles):
    story = []

    story.append(Spacer(1, 4.2 * cm))
    story.append(Paragraph("贪吃蛇项目协作使用教程", styles["Title"]))
    story.append(
        Paragraph(
            "面向小组成员：如何拉取仓库、认领模块、编译运行、提交代码和处理常见问题",
            styles["Subtitle"],
        )
    )
    story.append(Spacer(1, 18))
    story.append(
        table(
            [
                [para("项目", styles["TableHead"]), para("内容", styles["TableHead"])],
                [para("仓库地址", styles["TableCell"]), para("https://github.com/zhxymj/tanchishe", styles["TableCell"])],
                [para("实现语言", styles["TableCell"]), para("C 语言，源码统一为 .c 和 .h 文件", styles["TableCell"])],
                [para("推荐编译器", styles["TableCell"]), para("Windows + GCC", styles["TableCell"])],
                [para("协作方式", styles["TableCell"]), para("每人负责一个模块，尽量只修改自己负责的文件", styles["TableCell"])],
            ],
            [3.0 * cm, 11.2 * cm],
        )
    )
    story.append(PageBreak())

    story.extend(section("1. 项目目标", styles))
    story.append(
        para(
            "本项目是 C 语言控制台版贪吃蛇。当前仓库已经搭好整体框架，主要目的是让 5 名成员可以分别完成自己的模块，而不是所有人挤在一个 main.c 文件里修改。",
            styles["Body"],
        )
    )
    story.append(bullet("项目整体用 C 语言实现，不使用 C++、Python 或其他语言替代核心功能。", styles))
    story.append(bullet("每个成员都有独立编码任务，提交时能体现自己的代码贡献。", styles))
    story.append(bullet("公共结构体和函数接口已经建立，成员之间通过这些接口协作。", styles))
    story.append(bullet("当前版本是可编译框架，核心逻辑由各成员根据 TODO 继续完善。", styles))

    story.extend(section("2. 文件分工", styles))
    story.append(
        table(
            [
                [para("成员", styles["TableHead"]), para("负责文件", styles["TableHead"]), para("主要任务", styles["TableHead"])],
                [para("成员1", styles["TableCell"]), para("src/main.c<br/>src/game.c<br/>src/game.h", styles["TableCell"]), para("主程序、游戏状态、初始化、暂停、重开、退出和模块整合", styles["TableCell"])],
                [para("成员2", styles["TableCell"]), para("src/snake.c<br/>src/snake.h", styles["TableCell"]), para("蛇身数据结构、方向控制、蛇头移动、蛇身跟随和增长", styles["TableCell"])],
                [para("成员3", styles["TableCell"]), para("src/food.c<br/>src/food.h", styles["TableCell"]), para("食物随机生成、避免与蛇身重合、得分、等级和速度变化", styles["TableCell"])],
                [para("成员4", styles["TableCell"]), para("src/collision.c<br/>src/collision.h", styles["TableCell"]), para("撞墙检测、撞自身检测、失败判断和胜利判断", styles["TableCell"])],
                [para("成员5", styles["TableCell"]), para("src/ui.c<br/>src/ui.h", styles["TableCell"]), para("地图绘制、蛇和食物显示、分数显示、键盘输入和界面刷新", styles["TableCell"])],
            ],
            [2.2 * cm, 4.2 * cm, 8.6 * cm],
        )
    )
    story.append(
        para(
            "原则：每个人优先只改自己负责的文件。如果必须改 src/types.h 或 src/config.h，要先和组员说明，因为这些文件会影响所有模块。",
            styles["Body"],
        )
    )

    story.extend(section("3. 第一次使用前准备", styles))
    story.extend(subsection("3.1 安装 Git", styles))
    story.append(para("Git 用来下载项目、提交代码和推送到 GitHub。安装后在 PowerShell 输入下面命令，能显示版本号就说明安装成功。", styles["Body"]))
    story.append(code("git --version", styles))

    story.extend(subsection("3.2 安装 GCC", styles))
    story.append(para("GCC 用来编译 C 语言代码。安装完成后输入：", styles["Body"]))
    story.append(code("gcc --version", styles))
    story.append(para("如果提示找不到 gcc，需要把 GCC 的 bin 目录加入系统 Path，或者使用已经配置好 GCC 的终端。", styles["Body"]))

    story.extend(subsection("3.3 GitHub 网络和代理", styles))
    story.append(para("如果在国内访问 GitHub 很慢，可以先打开 v2rayN。你的代理常见端口如下：HTTP 为 10809，SOCKS 为 10808。PowerShell 需要手动设置代理：", styles["Body"]))
    story.append(code('$env:HTTP_PROXY="http://127.0.0.1:10809"\n$env:HTTPS_PROXY="http://127.0.0.1:10809"', styles))
    story.append(para("验证 GitHub 是否能访问：", styles["Body"]))
    story.append(code("curl.exe -I -x http://127.0.0.1:10809 https://github.com", styles))
    story.append(para("如果返回 HTTP/1.1 200 OK 或 200 Connection established，说明代理基本可用。", styles["Body"]))

    story.extend(section("4. 克隆项目", styles))
    story.append(para("选择一个你想保存项目的目录，例如桌面或 D 盘项目目录，然后运行：", styles["Body"]))
    story.append(code("git clone https://github.com/zhxymj/tanchishe.git\ncd tanchishe", styles))
    story.append(para("拉取后应能看到这些文件：README.md、TASKS.md、CONTRIBUTING.md、build.bat、src 文件夹。", styles["Body"]))

    story.extend(section("5. 编译和运行", styles))
    story.append(para("在 tanchishe 项目根目录运行：", styles["Body"]))
    story.append(code("gcc src\\main.c src\\game.c src\\snake.c src\\food.c src\\collision.c src\\ui.c -o snake.exe\n.\\snake.exe", styles))
    story.append(para("也可以直接运行 build.bat：", styles["Body"]))
    story.append(code(".\\build.bat", styles))
    story.append(para("当前是协作框架版本，能运行不代表游戏功能已经完整。每个模块里的 TODO 完成后，游戏才会逐步变完整。", styles["Body"]))

    story.extend(section("6. 如何完成自己的模块", styles))
    story.append(para("打开自己负责的 .c 文件，搜索 TODO(Member X)。例如成员2负责蛇身模块，可以在 src/snake.c 里看到：", styles["Body"]))
    story.append(code("TODO(Member 2): initialize the full starting snake body\nTODO(Member 2): handle direction update rules\nTODO(Member 2): complete snake movement and growth", styles))
    story.append(para("实现时注意：", styles["Body"]))
    story.append(bullet("优先保持 .h 文件里的函数名不变。", styles))
    story.append(bullet("不要把所有代码写进 main.c，要放到自己负责的模块中。", styles))
    story.append(bullet("写完一个小功能就编译一次，避免最后一次性出现很多错误。", styles))
    story.append(bullet("如果需要新增公共字段，先确认其他成员不会受影响。", styles))

    story.extend(section("7. 提交代码流程", styles))
    story.extend(subsection("7.1 写代码前先同步", styles))
    story.append(code("git pull origin main", styles))
    story.append(para("这一步可以减少和别人代码冲突的概率。", styles["Body"]))

    story.extend(subsection("7.2 查看自己改了哪些文件", styles))
    story.append(code("git status", styles))
    story.append(para("确认只改了自己负责的文件。如果看到别人的文件也被改了，不要急着提交，先检查是不是误改。", styles["Body"]))

    story.extend(subsection("7.3 编译检查", styles))
    story.append(code("gcc src\\main.c src\\game.c src\\snake.c src\\food.c src\\collision.c src\\ui.c -o snake.exe", styles))
    story.append(para("编译成功后再提交。snake.exe 不需要提交，仓库已经用 .gitignore 忽略它。", styles["Body"]))

    story.extend(subsection("7.4 只提交自己负责的文件", styles))
    story.append(para("例如成员2完成蛇身模块：", styles["Body"]))
    story.append(code('git add src\\snake.c src\\snake.h\ngit commit -m "finish snake movement module"\ngit push origin main', styles))
    story.append(para("例如成员3完成食物和计分模块：", styles["Body"]))
    story.append(code('git add src\\food.c src\\food.h\ngit commit -m "finish food and score module"\ngit push origin main', styles))

    story.extend(section("8. 每个成员的建议实现顺序", styles))
    story.append(
        table(
            [
                [para("成员", styles["TableHead"]), para("建议顺序", styles["TableHead"])],
                [para("成员1", styles["TableCell"]), para("先保证 game_init、game_handle_key、game_update 的调用顺序清楚，再配合其他模块整合。", styles["TableCell"])],
                [para("成员2", styles["TableCell"]), para("先完成蛇初始化，再完成方向控制，最后完成蛇身移动和增长。", styles["TableCell"])],
                [para("成员3", styles["TableCell"]), para("先完成食物随机生成，再处理食物不与蛇身重合，最后完成计分、等级和速度调整。", styles["TableCell"])],
                [para("成员4", styles["TableCell"]), para("先完成撞墙检测，再完成撞自身检测，最后补充失败和胜利判断。", styles["TableCell"])],
                [para("成员5", styles["TableCell"]), para("先画基础地图，再显示蛇和食物，最后完善分数、提示文字、暂停和结束界面。", styles["TableCell"])],
            ],
            [2.2 * cm, 12.8 * cm],
        )
    )

    story.extend(section("9. 常见问题处理", styles))
    story.extend(subsection("9.1 git push 失败，提示网络超时", styles))
    story.append(para("通常是 PowerShell 没有走代理。先执行：", styles["Body"]))
    story.append(code('$env:HTTP_PROXY="http://127.0.0.1:10809"\n$env:HTTPS_PROXY="http://127.0.0.1:10809"', styles))
    story.append(para("如果还是失败，检查 v2rayN 是否正在运行，并确认 HTTP 端口是否为 10809。", styles["Body"]))

    story.extend(subsection("9.2 git push 失败，提示先 pull", styles))
    story.append(para("说明别人已经先提交了代码，你本地落后于远程仓库。先运行：", styles["Body"]))
    story.append(code("git pull origin main", styles))
    story.append(para("如果没有冲突，再重新 push。", styles["Body"]))

    story.extend(subsection("9.3 出现代码冲突", styles))
    story.append(para("如果 Git 提示 conflict，说明两个人改到了同一段代码。先不要乱删内容，打开冲突文件，找到类似下面的标记：", styles["Body"]))
    story.append(code("<<<<<<< HEAD\n你的代码\n=======\n别人提交的代码\n>>>>>>> 分支名", styles))
    story.append(para("和对应成员确认保留哪部分，删掉这些冲突标记后重新编译、提交。", styles["Body"]))

    story.extend(subsection("9.4 编译提示 undefined reference", styles))
    story.append(para("通常是函数声明和实现不一致，或者编译命令漏了某个 .c 文件。请使用教程里的完整编译命令。", styles["Body"]))

    story.extend(subsection("9.5 中文显示乱码", styles))
    story.append(para("当前控制台代码使用了 SetConsoleOutputCP(65001)。如果仍然乱码，可以先把终端编码切到 UTF-8：", styles["Body"]))
    story.append(code("chcp 65001", styles))

    story.extend(section("10. 提交前检查清单", styles))
    story.append(bullet("只修改了自己负责的模块文件。", styles))
    story.append(bullet("运行 git status 看过改动范围。", styles))
    story.append(bullet("运行 GCC 编译命令，没有编译错误。", styles))
    story.append(bullet("没有提交 snake.exe、临时文件、截图草稿或无关文件。", styles))
    story.append(bullet("commit message 能说明这次完成了什么模块或功能。", styles))
    story.append(bullet("push 后在 GitHub 页面刷新，确认自己的提交已经出现。", styles))

    story.extend(section("11. 最简命令汇总", styles))
    story.append(code(
        r"""
# 第一次下载项目
git clone https://github.com/zhxymj/tanchishe.git
cd tanchishe

# 每次写代码前
git pull origin main

# 编译
gcc src\main.c src\game.c src\snake.c src\food.c src\collision.c src\ui.c -o snake.exe

# 查看改动
git status

# 只提交自己的模块，以成员2为例
git add src\snake.c src\snake.h
git commit -m "finish snake movement module"
git push origin main
""",
        styles,
    ))

    story.append(Spacer(1, 8))
    story.append(para("结论：每名成员按自己的文件分工完成 TODO，提交前先编译，提交时只提交自己的模块文件。这样项目能保持清晰，也方便报告中说明每个人的代码贡献。", styles["Body"]))
    return story


def main():
    font_name = register_fonts()
    styles = build_styles(font_name)
    doc = SimpleDocTemplate(
        str(OUTPUT),
        pagesize=A4,
        rightMargin=2.0 * cm,
        leftMargin=2.0 * cm,
        topMargin=2.0 * cm,
        bottomMargin=2.0 * cm,
        title="贪吃蛇项目协作使用教程",
        author="zhxymj",
    )
    doc.build(build_story(styles), onFirstPage=page_footer, onLaterPages=page_footer)
    print(OUTPUT)


if __name__ == "__main__":
    main()

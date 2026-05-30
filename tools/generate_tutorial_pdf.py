from pathlib import Path

from reportlab.lib import colors
from reportlab.lib.enums import TA_CENTER, TA_LEFT
from reportlab.lib.pagesizes import A4
from reportlab.lib.styles import ParagraphStyle, getSampleStyleSheet
from reportlab.lib.units import cm
from reportlab.pdfbase import pdfmetrics
from reportlab.pdfbase.ttfonts import TTFont
from reportlab.platypus import Paragraph, Preformatted, SimpleDocTemplate, Spacer, Table, TableStyle


ROOT = Path(__file__).resolve().parents[1]
OUTPUT = ROOT / "docs" / "贪吃蛇项目协作使用教程.pdf"
FONT_PATH = Path(r"C:\Windows\Fonts\simhei.ttf")


def font_name():
    if FONT_PATH.exists():
        pdfmetrics.registerFont(TTFont("SimHei", str(FONT_PATH)))
        return "SimHei"
    return "Helvetica"


def styles(name):
    base = getSampleStyleSheet()
    return {
        "Title": ParagraphStyle("Title", parent=base["Title"], fontName=name, fontSize=23, leading=30, alignment=TA_CENTER, textColor=colors.HexColor("#1f2937"), spaceAfter=10),
        "Sub": ParagraphStyle("Sub", parent=base["BodyText"], fontName=name, fontSize=11, leading=16, alignment=TA_CENTER, textColor=colors.HexColor("#4b5563"), wordWrap="CJK"),
        "H1": ParagraphStyle("H1", parent=base["Heading1"], fontName=name, fontSize=15, leading=21, textColor=colors.HexColor("#2e74b5"), spaceBefore=10, spaceAfter=6, wordWrap="CJK"),
        "H2": ParagraphStyle("H2", parent=base["Heading2"], fontName=name, fontSize=12, leading=17, textColor=colors.HexColor("#1f4d78"), spaceBefore=6, spaceAfter=4, wordWrap="CJK"),
        "Body": ParagraphStyle("Body", parent=base["BodyText"], fontName=name, fontSize=10.2, leading=16, alignment=TA_LEFT, textColor=colors.HexColor("#111827"), spaceAfter=5, wordWrap="CJK"),
        "Code": ParagraphStyle("Code", parent=base["Code"], fontName=name, fontSize=8.8, leading=12.5, backColor=colors.HexColor("#f3f4f6"), borderColor=colors.HexColor("#d1d5db"), borderWidth=0.35, borderPadding=6),
        "TH": ParagraphStyle("TH", parent=base["BodyText"], fontName=name, fontSize=9, leading=12, textColor=colors.HexColor("#1f4d78"), alignment=TA_CENTER, wordWrap="CJK"),
        "TD": ParagraphStyle("TD", parent=base["BodyText"], fontName=name, fontSize=8.7, leading=12.5, textColor=colors.HexColor("#111827"), wordWrap="CJK"),
    }


def p(text, st):
    return Paragraph(text, st)


def code(text, st):
    return Preformatted(text.strip(), st["Code"])


def section(title, st):
    return [Spacer(1, 6), p(title, st["H1"])]


def table(rows, widths, st):
    data = [[p(cell, st["TH"] if row_index == 0 else st["TD"]) for cell in row] for row_index, row in enumerate(rows)]
    t = Table(data, colWidths=widths, repeatRows=1)
    t.setStyle(
        TableStyle(
            [
                ("BACKGROUND", (0, 0), (-1, 0), colors.HexColor("#e8eef5")),
                ("GRID", (0, 0), (-1, -1), 0.35, colors.HexColor("#cbd5e1")),
                ("VALIGN", (0, 0), (-1, -1), "MIDDLE"),
                ("LEFTPADDING", (0, 0), (-1, -1), 5),
                ("RIGHTPADDING", (0, 0), (-1, -1), 5),
                ("TOPPADDING", (0, 0), (-1, -1), 4),
                ("BOTTOMPADDING", (0, 0), (-1, -1), 4),
            ]
        )
    )
    return t


def bullet(text, st):
    return p("• " + text, st["Body"])


def build_story(st):
    story = [
        Spacer(1, 1.2 * cm),
        p("贪吃蛇项目协作使用教程", st["Title"]),
        p("C11 + raylib 图形化单机游戏 · GitHub 提交与同步说明", st["Sub"]),
        Spacer(1, 12),
    ]

    story += section("1. 项目基本信息", st)
    story.append(
        table(
            [
                ["项目", "内容"],
                ["仓库地址", "https://github.com/zhxymj/tanchishe"],
                ["实现语言", "C11"],
                ["图形库", "raylib 6.0，安装路径 D:\\raylib"],
                ["主程序", "src/main.c"],
                ["编译方式", "优先使用 .\\build.bat，也可使用 gcc 命令"],
            ],
            [3.2 * cm, 12.0 * cm],
            st,
        )
    )

    story += section("2. 编译和运行", st)
    story.append(p("在项目根目录运行：", st["Body"]))
    story.append(code(r"""
.\build.bat
""", st))
    story.append(p("只想编译、不想通过脚本启动时运行：", st["Body"]))
    story.append(code(r"""
gcc src\main.c -std=c11 -O2 -Wall -Wextra -I"D:\raylib\include" -L"D:\raylib\lib" -lraylib -lopengl32 -lgdi32 -lwinmm -o snake_raylib.exe
.\snake_raylib.exe
""", st))
    story.append(p("如果使用 CMake，建议把仓库放在纯英文路径，例如 D:\\code\\tanchishe；当前中文路径下部分 Windows make/ninja 可能无法解析绝对路径。", st["Body"]))

    story += section("3. 五人函数分工", st)
    story.append(
        table(
            [
                ["成员", "负责内容"],
                ["成员1", "main、InitGame、ResetGame、StartOrResumeGame、TogglePause、UpdateGame：主流程和状态管理"],
                ["成员2", "Snake、QueueDirection、MoveSnake、NextHead、SameCell、SnakeContains：蛇身移动和碰撞"],
                ["成员3", "Food、SpawnFood、DrawFood、UpdateDifficulty、LoadHighScore、SaveHighScore：食物、计分和最高分"],
                ["成员4", "DrawBackground、DrawHeader、DrawBoardFrame、DrawBoardGrid、DrawSnake、DrawSnakeEyes：主要视觉绘制"],
                ["成员5", "HandleButtons、DrawSidePanel、DrawButton、DrawOverlay、DrawTextFit、ButtonLabel、build.bat、CMakeLists.txt：交互、弹窗和构建"],
            ],
            [2.0 * cm, 13.2 * cm],
            st,
        )
    )

    story += section("4. 写代码前同步最新版本", st)
    story.append(code(r"""
git status
git pull origin main
""", st))
    story.append(bullet("如果 git status 显示 working tree clean，可以直接 pull。", st))
    story.append(bullet("如果已有本地修改，先提交或 stash，再 pull。", st))

    story += section("5. 提交代码", st)
    story.append(p("由于核心代码集中在 src/main.c，提交前必须确认自己只改了负责的函数区域。", st["Body"]))
    story.append(code(r"""
git status
git diff src\main.c
.\build.bat
git add src\main.c
git commit -m "完成某某功能"
git push origin main
""", st))
    story.append(p("如果只改了文档或构建脚本，就只 add 对应文件。不要提交 snake_raylib.exe、build/ 或最高分数据文件。", st["Body"]))

    story += section("6. 常见问题", st)
    story.append(bullet("GitHub 连接失败：先打开代理，再设置 HTTP_PROXY 和 HTTPS_PROXY。", st))
    story.append(code(r"""
$env:HTTP_PROXY="http://127.0.0.1:10809"
$env:HTTPS_PROXY="http://127.0.0.1:10809"
git push origin main
""", st))
    story.append(bullet("CMake 在中文路径失败：使用 .\\build.bat，或者把仓库复制到纯英文路径。", st))
    story.append(bullet("raylib 找不到：确认 D:\\raylib\\include\\raylib.h 和 D:\\raylib\\lib\\libraylib.a 存在。", st))

    story += section("7. 完成度检查清单", st)
    for item in [
        "游戏能启动窗口，不是控制台字符版。",
        "方向键和 W/A/S/D 能控制移动。",
        "支持开始、暂停、继续、重新开始和游戏结束。",
        "吃食物后增长、加分、速度逐步提高。",
        "撞墙或撞自己后游戏结束。",
        "分数、最高分、等级、速度和状态显示在 UI 面板中。",
        "按钮有 hover/pressed 状态，暂停和结束有半透明弹窗。",
        "README、TASKS、CONTRIBUTING 和本教程与 raylib 版本一致。",
    ]:
        story.append(bullet(item, st))

    return story


def footer(canvas, doc):
    canvas.saveState()
    canvas.setFont("SimHei" if "SimHei" in pdfmetrics.getRegisteredFontNames() else "Helvetica", 8)
    canvas.setFillColor(colors.HexColor("#6b7280"))
    canvas.drawString(2 * cm, 1.2 * cm, "贪吃蛇项目协作使用教程")
    canvas.drawRightString(A4[0] - 2 * cm, 1.2 * cm, f"第 {doc.page} 页")
    canvas.restoreState()


def main():
    name = font_name()
    st = styles(name)
    OUTPUT.parent.mkdir(parents=True, exist_ok=True)
    doc = SimpleDocTemplate(str(OUTPUT), pagesize=A4, leftMargin=2 * cm, rightMargin=2 * cm, topMargin=1.8 * cm, bottomMargin=1.8 * cm)
    doc.build(build_story(st), onFirstPage=footer, onLaterPages=footer)
    print(OUTPUT)


if __name__ == "__main__":
    main()

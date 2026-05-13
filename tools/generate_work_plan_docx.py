from pathlib import Path

from docx import Document
from docx.enum.section import WD_SECTION
from docx.enum.table import WD_ALIGN_VERTICAL, WD_TABLE_ALIGNMENT
from docx.enum.text import WD_ALIGN_PARAGRAPH
from docx.oxml import OxmlElement
from docx.oxml.ns import qn
from docx.shared import Cm, Pt, RGBColor


ROOT = Path(__file__).resolve().parents[1]
OUTPUT = ROOT / "docs" / "贪吃蛇项目五人详细工作步骤安排.docx"


def set_run_font(run, font="微软雅黑", size=None, bold=None, color=None):
    run.font.name = font
    run._element.rPr.rFonts.set(qn("w:eastAsia"), font)
    if size is not None:
        run.font.size = Pt(size)
    if bold is not None:
        run.bold = bold
    if color is not None:
        run.font.color.rgb = RGBColor(*color)


def set_para_font(paragraph, font="微软雅黑", size=10.5):
    for run in paragraph.runs:
        set_run_font(run, font=font, size=size)


def shade_cell(cell, fill):
    tc_pr = cell._tc.get_or_add_tcPr()
    shd = OxmlElement("w:shd")
    shd.set(qn("w:fill"), fill)
    tc_pr.append(shd)


def set_cell_text(cell, text, bold=False, color=None, size=9.5, align=None):
    cell.text = ""
    p = cell.paragraphs[0]
    if align is not None:
        p.alignment = align
    run = p.add_run(text)
    set_run_font(run, size=size, bold=bold, color=color)
    cell.vertical_alignment = WD_ALIGN_VERTICAL.CENTER


def set_table_style(table, header=True):
    table.alignment = WD_TABLE_ALIGNMENT.CENTER
    table.style = "Table Grid"
    for row_index, row in enumerate(table.rows):
        for cell in row.cells:
            cell.vertical_alignment = WD_ALIGN_VERTICAL.CENTER
            for paragraph in cell.paragraphs:
                paragraph.paragraph_format.space_before = Pt(0)
                paragraph.paragraph_format.space_after = Pt(2)
                paragraph.paragraph_format.line_spacing = 1.15
        if header and row_index == 0:
            for cell in row.cells:
                shade_cell(cell, "0F766E")
                for paragraph in cell.paragraphs:
                    for run in paragraph.runs:
                        set_run_font(run, bold=True, color=(255, 255, 255), size=9.5)


def add_heading(doc, text, level=1):
    paragraph = doc.add_heading(text, level=level)
    paragraph.paragraph_format.space_before = Pt(8 if level == 1 else 5)
    paragraph.paragraph_format.space_after = Pt(5)
    for run in paragraph.runs:
        set_run_font(run, size=16 if level == 1 else 12.5, bold=True, color=(15, 118, 110) if level == 1 else (55, 65, 81))
    return paragraph


def add_body(doc, text, bold_prefix=None):
    p = doc.add_paragraph()
    p.paragraph_format.first_line_indent = Cm(0.74)
    p.paragraph_format.line_spacing = 1.25
    p.paragraph_format.space_after = Pt(3)
    if bold_prefix and text.startswith(bold_prefix):
        r1 = p.add_run(bold_prefix)
        set_run_font(r1, bold=True)
        r2 = p.add_run(text[len(bold_prefix):])
        set_run_font(r2)
    else:
        r = p.add_run(text)
        set_run_font(r)
    return p


def add_bullet(doc, text):
    p = doc.add_paragraph(style=None)
    p.paragraph_format.left_indent = Cm(0.65)
    p.paragraph_format.first_line_indent = Cm(-0.35)
    p.paragraph_format.line_spacing = 1.2
    p.paragraph_format.space_after = Pt(2)
    r = p.add_run("• " + text)
    set_run_font(r, size=10.5)
    return p


def add_numbered_list(doc, items):
    for index, item in enumerate(items, 1):
        p = doc.add_paragraph()
        p.paragraph_format.left_indent = Cm(0.8)
        p.paragraph_format.first_line_indent = Cm(-0.5)
        p.paragraph_format.line_spacing = 1.2
        p.paragraph_format.space_after = Pt(2)
        r = p.add_run(f"{index}. {item}")
        set_run_font(r, size=10.2)


def add_code_block(doc, text):
    table = doc.add_table(rows=1, cols=1)
    table.alignment = WD_TABLE_ALIGNMENT.CENTER
    table.style = "Table Grid"
    cell = table.cell(0, 0)
    shade_cell(cell, "F3F4F6")
    cell.vertical_alignment = WD_ALIGN_VERTICAL.CENTER
    p = cell.paragraphs[0]
    p.paragraph_format.space_before = Pt(2)
    p.paragraph_format.space_after = Pt(2)
    for line_index, line in enumerate(text.strip().splitlines()):
        if line_index > 0:
            p.add_run().add_break()
        run = p.add_run(line)
        set_run_font(run, font="Consolas", size=9.2, color=(17, 24, 39))
    doc.add_paragraph()


def add_info_table(doc, rows, widths=None):
    table = doc.add_table(rows=1, cols=len(rows[0]))
    hdr = table.rows[0].cells
    for i, text in enumerate(rows[0]):
        set_cell_text(hdr[i], text, bold=True, color=(255, 255, 255), align=WD_ALIGN_PARAGRAPH.CENTER)
        shade_cell(hdr[i], "0F766E")
    for row_data in rows[1:]:
        cells = table.add_row().cells
        for i, text in enumerate(row_data):
            set_cell_text(cells[i], text, size=9.2, align=WD_ALIGN_PARAGRAPH.LEFT)
    set_table_style(table, header=False)
    if widths:
        for row in table.rows:
            for i, width in enumerate(widths):
                row.cells[i].width = Cm(width)
    doc.add_paragraph()
    return table


def setup_document():
    doc = Document()
    section = doc.sections[0]
    section.top_margin = Cm(2.2)
    section.bottom_margin = Cm(2.0)
    section.left_margin = Cm(2.1)
    section.right_margin = Cm(2.1)

    styles = doc.styles
    styles["Normal"].font.name = "微软雅黑"
    styles["Normal"]._element.rPr.rFonts.set(qn("w:eastAsia"), "微软雅黑")
    styles["Normal"].font.size = Pt(10.5)

    return doc


def add_cover(doc):
    p = doc.add_paragraph()
    p.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p.paragraph_format.space_before = Pt(100)
    r = p.add_run("贪吃蛇项目五人详细工作步骤安排")
    set_run_font(r, size=22, bold=True, color=(31, 41, 55))

    p = doc.add_paragraph()
    p.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p.paragraph_format.space_before = Pt(10)
    r = p.add_run("C 语言控制台贪吃蛇项目 · 小组协作版")
    set_run_font(r, size=12, color=(75, 85, 99))

    p = doc.add_paragraph()
    p.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p.paragraph_format.space_before = Pt(20)
    r = p.add_run("仓库地址：https://github.com/zhxymj/tanchishe")
    set_run_font(r, size=11, color=(55, 65, 81))

    doc.add_paragraph()
    add_info_table(
        doc,
        [
            ["项目要求", "说明"],
            ["实现语言", "整体使用 C 语言，核心代码统一写在 .c 和 .h 文件中"],
            ["协作方式", "5 名成员分别负责 5 个模块，尽量只修改自己负责的文件"],
            ["提交要求", "提交前先同步最新代码、编译通过后再提交、只提交本人负责文件"],
            ["文档用途", "发给组员作为开发步骤和提交代码参考"],
        ],
        widths=[3.2, 11.5],
    )
    doc.add_page_break()


def add_common_workflow(doc):
    add_heading(doc, "一、全体成员统一要求", 1)
    add_body(doc, "本项目采用模块化分工，每个成员负责独立的 .c/.h 文件。开发时不要把所有代码都写进 main.c，也不要随意修改其他成员负责的文件。")
    add_body(doc, "每次写代码前先同步 GitHub 上的最新版本，写完后先编译，确认没有错误再提交。")

    add_heading(doc, "1. 每次写代码前先同步", 2)
    add_code_block(
        doc,
        r"""
cd tanchishe
git status
git pull origin main
""",
    )
    add_bullet(doc, "如果 git status 显示 working tree clean，说明当前没有未提交修改，可以直接 pull。")
    add_bullet(doc, "如果已经改了代码但没提交，优先先 commit，再 pull；或者用 git stash 临时保存。")

    add_heading(doc, "2. 提交前必须编译", 2)
    add_code_block(
        doc,
        r"""
gcc src\main.c src\game.c src\snake.c src\food.c src\collision.c src\ui.c -o snake.exe
""",
    )
    add_bullet(doc, "编译成功后再提交。")
    add_bullet(doc, "snake.exe 是编译产物，不需要提交。")

    add_heading(doc, "3. 提交代码的标准流程", 2)
    add_code_block(
        doc,
        r"""
git status
git add src\自己负责的文件.c src\自己负责的文件.h
git commit -m "完成某某模块"
git push origin main
""",
    )
    add_bullet(doc, "不要习惯性使用 git add .，避免把无关文件或别人文件一起提交。")
    add_bullet(doc, "push 成功后打开 GitHub 仓库刷新，确认自己的提交已经出现。")

    add_heading(doc, "4. 五人模块总览", 2)
    add_info_table(
        doc,
        [
            ["成员", "负责文件", "主要工作"],
            ["成员1", "src/main.c\nsrc/game.c\nsrc/game.h", "主程序、初始化、主循环、状态切换、暂停、重开、退出和模块整合"],
            ["成员2", "src/snake.c\nsrc/snake.h", "蛇身坐标、方向控制、蛇头移动、蛇身跟随和增长"],
            ["成员3", "src/food.c\nsrc/food.h", "食物随机生成、避免重合、得分、等级和速度变化"],
            ["成员4", "src/collision.c\nsrc/collision.h", "撞墙、撞自身、失败判断和胜利判断"],
            ["成员5", "src/ui.c\nsrc/ui.h", "地图绘制、蛇和食物显示、键盘输入、分数显示和界面刷新"],
        ],
        widths=[2.0, 4.0, 8.4],
    )


def add_member_section(doc, member):
    add_heading(doc, member["title"], 1)
    add_info_table(
        doc,
        [
            ["项目", "内容"],
            ["负责文件", "\n".join(member["files"])],
            ["模块目标", member["goal"]],
            ["交付标准", member["standard"]],
        ],
        widths=[2.8, 11.7],
    )

    add_heading(doc, "详细实施步骤", 2)
    add_numbered_list(doc, member["steps"])

    add_heading(doc, "自测重点", 2)
    for item in member["tests"]:
        add_bullet(doc, item)

    add_heading(doc, "建议提交命令", 2)
    add_code_block(doc, member["commands"])


def build_document():
    doc = setup_document()
    add_cover(doc)
    add_common_workflow(doc)

    members = [
        {
            "title": "二、成员1：主程序框架与游戏状态管理",
            "files": ["src/main.c", "src/game.c", "src/game.h"],
            "goal": "负责整个游戏流程的调度，把蛇身、食物、碰撞和界面模块按照正确顺序整合起来。",
            "standard": "程序能正常初始化、运行、暂停、继续、重新开始和退出；其他成员完成模块后可以顺利接入。",
            "steps": [
                "阅读 src/types.h 中的 Game、Snake、Food、GameStatus 等结构体和枚举，确认游戏数据如何在各模块之间传递。",
                "完善 game_init：初始化随机数、调用 snake_init 初始化蛇、设置 score、level、speed_ms、status，并调用 food_spawn 生成初始食物。",
                "完善 game_handle_key：处理 Q 退出、R 重开、P 暂停/继续；当游戏处于运行状态时，将 W/A/S/D 转交给 snake_set_direction。",
                "完善 game_update：先判断游戏是否处于 GAME_RUNNING；再计算下一步蛇头位置；然后判断是否吃到食物、是否撞墙或撞自身。",
                "在 game_update 中按顺序调用 collision、snake_move、food_apply_score、food_spawn，保证游戏逻辑清晰。",
                "处理 GAME_OVER 和 GAME_VICTORY 状态，确保失败或胜利后不会继续自动移动。",
                "最后和其他成员联调，检查各模块函数返回值和调用顺序是否一致。",
            ],
            "tests": [
                "启动游戏后分数为 0、等级为 1、状态为运行中。",
                "按 P 能暂停，再按 P 能继续。",
                "按 R 能重新开始，蛇、分数、速度和食物都恢复初始状态。",
                "按 Q 能退出程序。",
                "撞墙或撞自身后状态切换为 GAME_OVER。",
            ],
            "commands": r"""
git add src\main.c src\game.c src\game.h
git commit -m "完成主程序和游戏状态管理模块"
git push origin main
""",
        },
        {
            "title": "三、成员2：蛇身移动与方向控制",
            "files": ["src/snake.c", "src/snake.h"],
            "goal": "负责蛇的核心移动逻辑，包括蛇身数据、方向切换、蛇头前进、蛇身跟随和吃食物后的增长。",
            "standard": "蛇可以按照方向连续移动，不能直接反向移动，吃到食物后长度增加，蛇身坐标维护正确。",
            "steps": [
                "阅读 Snake 结构体，理解 body 数组中 body[0] 表示蛇头，后续元素表示蛇身。",
                "完善 snake_init：设置初始长度为 INITIAL_SNAKE_LENGTH，把蛇放在地图中心附近，默认方向为 DIR_RIGHT。",
                "完善 snake_set_direction：判断新方向是否与当前方向相反；如果相反则忽略，避免蛇直接掉头撞到自己。",
                "完善 snake_next_head：根据 next_direction 返回下一步蛇头坐标，不直接修改蛇身。",
                "完善 snake_move：从尾部开始倒序移动蛇身，使每一节跟随前一节；最后把 body[0] 设置为新蛇头。",
                "处理 grow 参数：grow 为真时长度加 1，尾巴保留；grow 为假时正常移动，长度不变。",
                "完善 snake_contains_point 和 snake_contains_point_from_index，用于判断食物是否生成在蛇身上、碰撞是否发生。",
            ],
            "tests": [
                "蛇能向上、下、左、右移动。",
                "向右移动时按 A 不会立刻反向；向上移动时按 S 不会立刻反向。",
                "吃到食物后蛇身长度增加 1。",
                "连续移动时蛇身跟随自然，没有断裂或重叠异常。",
                "snake_contains_point 能正确识别蛇头和蛇身坐标。",
            ],
            "commands": r"""
git add src\snake.c src\snake.h
git commit -m "完成蛇身移动和方向控制模块"
git push origin main
""",
        },
        {
            "title": "四、成员3：食物生成、得分与难度变化",
            "files": ["src/food.c", "src/food.h"],
            "goal": "负责食物随机刷新、避免与蛇身重合、吃食物判断、加分、等级和速度变化。",
            "standard": "食物能随机出现在有效地图位置，不能刷在蛇身上；吃到食物后分数增加，等级和速度按规则变化。",
            "steps": [
                "阅读 config.h 中 BOARD_WIDTH、BOARD_HEIGHT、POINTS_PER_FOOD、LEVEL_SCORE_STEP、SPEED_STEP_MS 等常量。",
                "完善 food_spawn：用 rand() 在地图范围内生成 x、y 坐标。",
                "调用 snake_contains_point 判断生成点是否在蛇身上；如果重合就重新生成，直到位置合法。",
                "处理特殊情况：当蛇身长度达到 MAX_CELLS 时，将 food 坐标设置为无效值，交给胜利判断处理。",
                "完善 food_is_eaten：判断蛇头坐标是否等于食物坐标。",
                "完善 food_apply_score：每吃一次食物增加 POINTS_PER_FOOD 分。",
                "根据分数更新等级，并在升级时减少 speed_ms，但不能低于 MIN_SPEED_MS。",
            ],
            "tests": [
                "食物坐标始终在地图内部。",
                "食物不会生成在蛇头或蛇身上。",
                "蛇头碰到食物时 food_is_eaten 返回真。",
                "吃食物后分数正确增加。",
                "达到升级分数后等级提升、速度变快，并且速度不会低于最低值。",
            ],
            "commands": r"""
git add src\food.c src\food.h
git commit -m "完成食物生成和计分难度模块"
git push origin main
""",
        },
        {
            "title": "五、成员4：碰撞检测与胜负判断",
            "files": ["src/collision.c", "src/collision.h"],
            "goal": "负责判断蛇是否撞墙、撞到自身，以及游戏是否失败或胜利。",
            "standard": "碰撞判断准确，不误判正常移动；失败和胜利条件能被成员1的游戏流程正确使用。",
            "steps": [
                "完善 collision_hits_wall：当蛇头 x 小于 0、x 大于等于 BOARD_WIDTH、y 小于 0、y 大于等于 BOARD_HEIGHT 时返回真。",
                "完善 collision_hits_self：遍历蛇身坐标，判断下一步蛇头是否和蛇身重合。",
                "注意普通移动时尾巴会离开原位置；如果 grow 为假，可以不把当前尾巴位置算作碰撞。",
                "如果 grow 为真，尾巴不会离开，此时撞到任何蛇身位置都应算碰撞。",
                "完善 collision_is_victory：当蛇身长度达到 MAX_CELLS，说明地图被填满，返回胜利。",
                "和成员1联调，确认碰撞后 game->status 能正确变为 GAME_OVER 或 GAME_VICTORY。",
            ],
            "tests": [
                "蛇头越过上、下、左、右边界都会失败。",
                "蛇头撞到自己的身体时失败。",
                "普通移动到原尾巴位置时不被误判为撞自己。",
                "吃食物增长时如果撞到身体，应判定失败。",
                "蛇身占满地图时能触发胜利。",
            ],
            "commands": r"""
git add src\collision.c src\collision.h
git commit -m "完成碰撞检测和胜负判断模块"
git push origin main
""",
        },
        {
            "title": "六、成员5：界面绘制与键盘输入输出",
            "files": ["src/ui.c", "src/ui.h"],
            "goal": "负责控制台界面显示、键盘输入读取、地图刷新、分数提示和结束界面显示。",
            "standard": "界面能清楚显示地图、蛇、食物、分数和状态提示；键盘输入能被主程序正确读取。",
            "steps": [
                "完善 ui_setup：设置控制台 UTF-8 编码，必要时隐藏光标，减少刷新闪烁。",
                "完善 ui_draw：先清屏，再绘制游戏标题、分数、等级、速度和操作提示。",
                "绘制地图边框，地图内部根据坐标显示蛇头、蛇身、食物或空格。",
                "建议用 @ 表示蛇头，o 表示蛇身，* 表示食物，# 表示边框。",
                "根据 game->status 显示暂停、失败、胜利等提示信息。",
                "完善 ui_read_key：读取 W/A/S/D/P/R/Q；如果扩展方向键，需要处理 _getch 返回的特殊按键码。",
                "完善 ui_sleep：控制刷新间隔，使速度变化能体现在游戏运行中。",
            ],
            "tests": [
                "地图边框完整，蛇和食物显示位置正确。",
                "分数、等级、速度信息能实时显示。",
                "W/A/S/D/P/R/Q 按键都能被读取并产生效果。",
                "暂停、失败、胜利提示能正常显示。",
                "界面刷新时没有严重乱码或残影。",
            ],
            "commands": r"""
git add src\ui.c src\ui.h
git commit -m "完成界面绘制和键盘交互模块"
git push origin main
""",
        },
    ]

    for member in members:
        doc.add_page_break()
        add_member_section(doc, member)

    doc.add_page_break()
    add_heading(doc, "七、推荐联调顺序", 1)
    add_numbered_list(
        doc,
        [
            "成员2先提交蛇身初始化、方向控制和基础移动，让游戏有可移动对象。",
            "成员3提交食物生成和计分逻辑，让蛇吃食物后能产生变化。",
            "成员4提交碰撞检测，让游戏具备失败和胜利判断。",
            "成员5提交界面绘制和键盘读取，让游戏显示更完整。",
            "成员1最后进行整体联调，检查模块调用顺序、状态切换和异常情况。",
        ],
    )

    add_heading(doc, "八、最终检查清单", 1)
    for item in [
        "项目整体仍然使用 C 语言实现，没有引入 C++ 或 Python 核心代码。",
        "每个人都有对应的 .c/.h 文件修改记录。",
        "所有模块合并后能通过 GCC 编译。",
        "游戏能启动、移动、吃食物、加分、失败、重开和退出。",
        "README、TASKS 或报告中的分工与实际代码文件一致。",
        "最终答辩或报告中，每个人能说明自己模块的核心函数和实现思路。",
    ]:
        add_bullet(doc, item)

    section = doc.add_section(WD_SECTION.CONTINUOUS)
    footer = section.footer.paragraphs[0]
    footer.alignment = WD_ALIGN_PARAGRAPH.CENTER
    run = footer.add_run("贪吃蛇项目五人详细工作步骤安排")
    set_run_font(run, size=9, color=(107, 114, 128))

    return doc


def main():
    OUTPUT.parent.mkdir(parents=True, exist_ok=True)
    doc = build_document()
    doc.save(OUTPUT)
    print(OUTPUT)


if __name__ == "__main__":
    main()

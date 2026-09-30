/*============================================================
 * 校园导航系统 - Step 1: 验证 EasyX 环境
 * 目标: 弹出一个 1280x800 的窗口, 画标题、几个测试点,
 *       鼠标点一下就在控制台打印坐标.
 * 跑通这一步, 说明环境没问题, 再往下做图结构和算法.
 *===========================================================*/
#include <graphics.h>   /* EasyX 头文件, 装完 EasyX 后才有 */
#include <conio.h>
#include <stdio.h>
#include <windows.h>

/* 窗口尺寸 */
#define WIN_W 1280
#define WIN_H 800

int main(void)
{
    /* 1. 创建图形窗口: 像素宽 x 像素高 */
    initgraph(WIN_W, WIN_H);

    /* 2. 设置背景为白色, 清屏 */
    setbkcolor(WHITE);
    cleardevice();

    /* 3. 画标题 */
    settextstyle(40, 0, "微软雅黑");
    setbkmode(TRANSPARENT);          /* 文字背景透明 */
    outtextxy(450, 30, L"校园导航系统 Step1 - 环境测试");

    /* 4. 画 3 个测试点 (模拟教学楼/食堂/宿舍) */
    struct { int x, y; const wchar_t* name; } pts[] = {
        { 400, 300, L"教学楼" },
        { 800, 300, L"食堂"   },
        { 600, 550, L"宿舍"   },
    };

    setfillcolor(BLUE);
    for (int i = 0; i < 3; i++) {
        solidcircle(pts[i].x, pts[i].y, 15);          /* 画实心圆 */
        outtextxy(pts[i].x - 30, pts[i].y + 20, pts[i].name);
    }

    /* 5. 画一条测试连线 */
    setlinecolor(RED);
    setlinestyle(PS_SOLID, 3);
    line(400, 300, 800, 300);

    /* 6. 提示文字 */
    settextstyle(20, 0, "微软雅黑");
    outtextxy(450, 720, L"用鼠标在窗口里点几下, 坐标会打印在控制台; 按任意键退出.");

    /* 7. 消息循环: 等鼠标, 点一下打印一次坐标 */
    MOUSEMSG m;
    while (true) {
        if (MouseHit()) {
            m = GetMouseMsg();
            if (m.uMsg == WM_LBUTTONDOWN) {
                printf("你点击了: (%d, %d)\n", m.x, m.y);
                /* 点过的地方画个小绿点做标记 */
                setfillcolor(GREEN);
                solidcircle(m.x, m.y, 5);
            }
        }
        if (_kbhit()) break;   /* 按任意键退出 */
    }

    closegraph();
    return 0;
}

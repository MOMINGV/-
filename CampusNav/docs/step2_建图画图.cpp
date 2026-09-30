/*============================================================
 * 校园导航系统 - Step 2: 建图 + 画静态校园地图
 * 数据结构: 邻接矩阵 (顶点表 vexs[] + 邻接矩阵 adj[][])
 * 本步目标:
 *   1) 把校园抽象成图 (16 个地点, 22 条路)
 *   2) 在 EasyX 窗口里画出所有点和边
 *   3) 控制台打印邻接矩阵, 验证数据正确
 *===========================================================*/
#include <graphics.h>
#include <conio.h>
#include <stdio.h>
#include <math.h>
#include <windows.h>

#define MAXV 16          /* 顶点数 */
#define INF  0x3f3f3f3f  /* 表示不连通 */

#define WIN_W 1280
#define WIN_H 800

/* ---------- 顶点结构: 名字 + 在窗口里的坐标 ---------- */
typedef struct {
    wchar_t name[32];
    int x, y;
} Vertex;

/* ---------- 全局: 顶点表 + 邻接矩阵 ---------- */
Vertex vexs[MAXV];
int   adj[MAXV][MAXV];

/* ---------- 两点欧氏距离 (作为边权, 单位: 像素) ---------- */
int dist(int i, int j) {
    double dx = vexs[i].x - vexs[j].x;
    double dy = vexs[i].y - vexs[j].y;
    return (int)sqrt(dx * dx + dy * dy);
}

/* ---------- 录入校园数据 ---------- */
void initCampus() {
    /* 1. 顶点表: (编号, 名称, x, y) —— 坐标按哈工大威海地图相对位置映射 */
    Vertex v[MAXV] = {
        { L"西门",           140, 460 },
        { L"电子工程研究所", 220, 250 },
        { L"探海楼",         560, 250 },
        { L"图书馆",         790, 280 },
        { L"宁学楼",         560, 370 },
        { L"大学生活动中心", 730, 380 },
        { L"海棠园",         660, 460 },
        { L"学子园",         430, 490 },
        { L"七公寓",         560, 560 },
        { L"五公寓",         720, 560 },
        { L"主楼",           890, 620 },
        { L"体育场",         680, 680 },
        { L"桃苑",           560, 740 },
        { L"南校门",         880, 750 },
        { L"东门",          1120, 620 },
        { L"西南门",         360, 680 },
    };
    for (int i = 0; i < MAXV; i++) vexs[i] = v[i];

    /* 2. 邻接矩阵初始化: 全 INF, 对角线 0 */
    for (int i = 0; i < MAXV; i++)
        for (int j = 0; j < MAXV; j++)
            adj[i][j] = (i == j) ? 0 : INF;

    /* 3. 录入边 (双向, 无向图). 这里一对一对写, 方便你以后自己改路 */
    int edges[][2] = {
        { 0, 1 },   /* 西门 -- 电子工程研究所 */
        { 0, 15 },  /* 西门 -- 西南门 */
        { 0, 7 },   /* 西门 -- 学子园 */
        { 1, 2 },   /* 电子工程研究所 -- 探海楼 */
        { 2, 3 },   /* 探海楼 -- 图书馆 */
        { 2, 4 },   /* 探海楼 -- 宁学楼 */
        { 3, 5 },   /* 图书馆 -- 大学生活动中心 */
        { 4, 6 },   /* 宁学楼 -- 海棠园 */
        { 5, 6 },   /* 大学生活动中心 -- 海棠园 */
        { 7, 6 },   /* 学子园 -- 海棠园 */
        { 7, 8 },   /* 学子园 -- 七公寓 */
        { 7, 15 },  /* 学子园 -- 西南门 */
        { 8, 9 },   /* 七公寓 -- 五公寓 */
        { 8, 11 },  /* 七公寓 -- 体育场 */
        { 6, 9 },   /* 海棠园 -- 五公寓 */
        { 9, 10 },  /* 五公寓 -- 主楼 */
        { 10, 11 }, /* 主楼 -- 体育场 */
        { 10, 14 }, /* 主楼 -- 东门 */
        { 10, 13 }, /* 主楼 -- 南校门 */
        { 11, 12 }, /* 体育场 -- 桃苑 */
        { 12, 13 }, /* 桃苑 -- 南校门 */
        { 15, 11 }, /* 西南门 -- 体育场 */
    };
    int m = sizeof(edges) / sizeof(edges[0]);
    for (int k = 0; k < m; k++) {
        int a = edges[k][0], b = edges[k][1];
        int d = dist(a, b);
        adj[a][b] = adj[b][a] = d;
    }
}

/* ---------- 画静态地图: 边 + 顶点 + 名字 ---------- */
void drawMap() {
    cleardevice();

    /* 标题 */
    settextstyle(36, 0, L"微软雅黑");
    setbkmode(TRANSPARENT);
    outtextxy(440, 20, L"哈尔滨工业大学(威海) 校园导航图");

    /* 1. 先画边 (灰色细线) */
    setlinecolor(RGB(120, 120, 120));
    setlinestyle(PS_SOLID, 2);
    for (int i = 0; i < MAXV; i++)
        for (int j = i + 1; j < MAXV; j++)
            if (adj[i][j] != INF && adj[i][j] != 0)
                line(vexs[i].x, vexs[i].y, vexs[j].x, vexs[j].y);

    /* 2. 再画顶点 (蓝色实心圆) + 名字 */
    setfillcolor(BLUE);
    settextstyle(18, 0, L"微软雅黑");
    for (int i = 0; i < MAXV; i++) {
        solidcircle(vexs[i].x, vexs[i].y, 10);
        /* 名字标在点旁边, 简单偏移避免压住点 */
        outtextxy(vexs[i].x - 20, vexs[i].y + 12, vexs[i].name);
    }

    /* 底部提示 */
    settextstyle(18, 0, L"微软雅黑");
    outtextxy(450, 760, L"Step2 静态地图已加载, 按任意键退出");
}

/* ---------- 控制台打印邻接矩阵, 验证数据 ---------- */
void printAdj() {
    printf("===== 邻接矩阵 (INF=%d 表示不连通) =====\n", INF);
    printf("     ");
    for (int j = 0; j < MAXV; j++) printf("%6d", j);
    printf("\n");
    for (int i = 0; i < MAXV; i++) {
        printf("%3d  ", i);
        for (int j = 0; j < MAXV; j++) {
            if (adj[i][j] == INF) printf("   INF");
            else                  printf("%6d", adj[i][j]);
        }
        printf("\n");
    }
}

int main(void) {
    initCampus();
    printAdj();          /* 控制台看一眼数据 */

    initgraph(WIN_W, WIN_H);
    setbkcolor(RGB(240, 248, 240));  /* 浅绿背景, 像地图 */
    drawMap();

    _getch();
    closegraph();
    return 0;
}

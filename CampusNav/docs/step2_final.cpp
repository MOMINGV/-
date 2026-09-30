/*============================================================
 * 校园导航系统 - Step 2: 建图 + 画静态校园地图
 * 数据结构: 邻接矩阵 (顶点表 vexs[] + 邻接矩阵 adj[][])
 *===========================================================*/
#include <graphics.h>
#include <conio.h>
#include <stdio.h>
#include <math.h>
#include <windows.h>

#define MAXV 16
#define INF  0x3f3f3f3f

#define WIN_W 1280
#define WIN_H 800

typedef struct {
    wchar_t name[32];
    int x, y;
} Vertex;

Vertex vexs[MAXV];
int   adj[MAXV][MAXV];

int dist(int i, int j) {
    double dx = vexs[i].x - vexs[j].x;
    double dy = vexs[i].y - vexs[j].y;
    return (int)sqrt(dx * dx + dy * dy);
}

void initCampus() {
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

    for (int i = 0; i < MAXV; i++)
        for (int j = 0; j < MAXV; j++)
            adj[i][j] = (i == j) ? 0 : INF;

    int edges[][2] = {
        { 0, 1 }, { 0, 15 }, { 0, 7 },
        { 1, 2 }, { 2, 3 }, { 2, 4 },
        { 3, 5 }, { 4, 6 }, { 5, 6 },
        { 7, 6 }, { 7, 8 }, { 7, 15 },
        { 8, 9 }, { 8, 11 }, { 6, 9 },
        { 9, 10 }, { 10, 11 }, { 10, 14 },
        { 10, 13 }, { 11, 12 }, { 12, 13 },
        { 15, 11 },
    };
    int m = sizeof(edges) / sizeof(edges[0]);
    for (int k = 0; k < m; k++) {
        int a = edges[k][0], b = edges[k][1];
        int d = dist(a, b);
        adj[a][b] = adj[b][a] = d;
    }
}

void drawMap() {
    cleardevice();

    settextstyle(36, 0, L"微软雅黑");
    setbkmode(TRANSPARENT);
    outtextxy(440, 20, L"Harbin Institute of Technology (Weihai) Campus Navigation");

    setlinecolor(RGB(120, 120, 120));
    setlinestyle(PS_SOLID, 2);
    for (int i = 0; i < MAXV; i++)
        for (int j = i + 1; j < MAXV; j++)
            if (adj[i][j] != INF && adj[i][j] != 0)
                line(vexs[i].x, vexs[i].y, vexs[j].x, vexs[j].y);

    setfillcolor(BLUE);
    settextstyle(18, 0, L"微软雅黑");
    for (int i = 0; i < MAXV; i++) {
        solidcircle(vexs[i].x, vexs[i].y, 10);
        outtextxy(vexs[i].x - 20, vexs[i].y + 12, vexs[i].name);
    }

    settextstyle(18, 0, L"微软雅黑");
    outtextxy(450, 760, L"Step2 map loaded, press any key to exit");
}

void printAdj() {
    printf("===== Adjacency Matrix (INF=%d) =====\n", INF);
    for (int i = 0; i < MAXV; i++) {
        for (int j = 0; j < MAXV; j++) {
            if (adj[i][j] == INF) printf("   INF");
            else                  printf("%6d", adj[i][j]);
        }
        printf("\n");
    }
}

int main(void) {
    initCampus();
    printAdj();

    initgraph(WIN_W, WIN_H);
    setbkcolor(RGB(240, 248, 240));
    drawMap();

    _getch();
    closegraph();
    return 0;
}

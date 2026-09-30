#include <graphics.h>
#include <conio.h>
#include <stdio.h>
#include <math.h>
#include <windows.h>

#define MAXV 25
#define INF  0x3f3f3f3f
#define WIN_W 900
#define WIN_H 1000
#define MAX_HIST 10

typedef struct {
    wchar_t name[32];
    int x, y;
    wchar_t desc[128];
} Vertex;

typedef struct {
    int x, y, w, h;
    wchar_t text[32];
} Button;

typedef struct {
    int dist;
    int v;
} HeapNode;

typedef struct {
    int start, end;
    int path[MAXV];
    int pathLen;
    int dist;
} HistoryNode;

Vertex vexs[MAXV];
int   adj[MAXV][MAXV];
int gDist[MAXV];
int gPrev[MAXV];

IMAGE mapBg;
IMAGE photo;

Button btnReset = { 20,  950, 90, 36, L"重新查询" };
Button btnList  = { 120, 950, 90, 36, L"地点列表" };
Button btnInfo  = { 210, 950, 90, 36, L"地点详情" };
Button btnHist  = { 300, 950, 90, 36, L"历史记录" };
Button btnMat   = { 390, 950, 90, 36, L"邻接矩阵" };
Button btnExit  = { 480, 950, 90, 36, L"退出系统" };

HistoryNode history[MAX_HIST];
int histCount = 0;

wchar_t photoPath[MAXV][128] = {
    L"D:\\CampusNav\\photos\\0.jpg", L"", L"",
    L"D:\\CampusNav\\photos\\3.jpg", L"D:\\CampusNav\\photos\\4.jpg",
    L"D:\\CampusNav\\photos\\5.jpg", L"", L"", L"",
    L"D:\\CampusNav\\photos\\9.jpg",
    L"", L"", L"", L"",
    L"D:\\CampusNav\\photos\\15.jpg",
    L"D:\\CampusNav\\photos\\16.jpg",
    L"D:\\CampusNav\\photos\\17.jpg",
    L"D:\\CampusNav\\photos\\18.jpg",
    L"", L"", L"", L"", L"", L"",
};

HeapNode heap[MAXV * MAXV];
int heapSize = 0;

void heapSwap(int a, int b) {
    HeapNode t = heap[a]; heap[a] = heap[b]; heap[b] = t;
}

void heapPush(int dist, int v) {
    int i = heapSize++;
    heap[i].dist = dist; heap[i].v = v;
    while (i > 0) {
        int p = (i - 1) / 2;
        if (heap[p].dist <= heap[i].dist) break;
        heapSwap(p, i); i = p;
    }
}

HeapNode heapPop() {
    HeapNode top = heap[0];
    heap[0] = heap[--heapSize];
    int i = 0;
    while (true) {
        int l = 2*i+1, r = 2*i+2, min = i;
        if (l < heapSize && heap[l].dist < heap[min].dist) min = l;
        if (r < heapSize && heap[r].dist < heap[min].dist) min = r;
        if (min == i) break;
        heapSwap(i, min); i = min;
    }
    return top;
}

int dist(int i, int j) {
    double dx = vexs[i].x - vexs[j].x;
    double dy = vexs[i].y - vexs[j].y;
    return (int)sqrt(dx*dx + dy*dy);
}

void initCampus() {
    Vertex v[MAXV] = {
        { L"西门",            216, 570, L"校园正门之一, 临近金海滩." },
        { L"电子工程研究所",  241, 432, L"位于校园西北侧, 临海而建." },
        { L"探海楼",          442, 360, L"教学主楼, 可俯瞰黄海." },
        { L"图书馆",          552, 392, L"学校地标建筑, 藏书丰富." },
        { L"宁学楼",          446, 439, L"教学楼, 靠近海棠园." },
        { L"大学生活动中心",  545, 446, L"学生活动与社团中心." },
        { L"综合球类馆",      626, 434, L"室内篮球羽毛球等场馆." },
        { L"海棠园",          431, 492, L"校园中心广场花园." },
        { L"创新创业园",      318, 539, L"学生创业孵化基地." },
        { L"学子餐厅",        516, 531, L"学生食堂之一." },
        { L"学苑餐厅",        576, 592, L"学生食堂之一." },
        { L"七至十一公寓",    443, 563, L"学生宿舍区." },
        { L"一至五公寓",      545, 608, L"学生宿舍区." },
        { L"校史馆",          643, 634, L"展示学校发展历程." },
        { L"博学楼",          583, 675, L"教学楼." },
        { L"大学生服务中心",  521, 688, L"一站式学生服务大厅." },
        { L"明学楼",          522, 704, L"教学楼." },
        { L"功学楼",          578, 709, L"教学楼." },
        { L"主楼",            670, 705, L"学校行政与教学主楼." },
        { L"体育场",          520, 777, L"标准塑胶跑道操场." },
        { L"西南门",          308, 794, L"校园西南出入口." },
        { L"东门",            832, 723, L"校园东侧出入口." },
        { L"南校门",          670, 886, L"校园正门." },
        { L"山上公寓",        760, 479, L"东区学生宿舍." },
        { L"研究院",          763, 843, L"科研与研究生教学楼." },
    };
    for (int i = 0; i < MAXV; i++) vexs[i] = v[i];
    for (int i = 0; i < MAXV; i++)
        for (int j = 0; j < MAXV; j++)
            adj[i][j] = (i == j) ? 0 : INF;
    int edges[][2] = {
        {0,1},{0,20},{0,8},{1,2},{2,3},{2,4},{3,5},{5,6},{5,23},
        {4,7},{5,7},{7,8},{7,9},{9,10},{9,11},{10,12},{8,11},
        {11,12},{12,13},{12,14},{13,18},{14,15},{15,16},{16,17},
        {17,18},{16,19},{18,19},{18,21},{18,22},{18,24},{19,20},
        {20,8},{19,22},{22,24},{21,24},
    };
    int m = sizeof(edges)/sizeof(edges[0]);
    for (int k = 0; k < m; k++) {
        int a = edges[k][0], b = edges[k][1];
        adj[a][b] = adj[b][a] = dist(a, b);
    }
}

void dijkstra(int s) {
    int visited[MAXV];
    heapSize = 0;
    for (int i = 0; i < MAXV; i++) {
        gDist[i] = INF; visited[i] = 0; gPrev[i] = -1;
    }
    gDist[s] = 0;
    heapPush(0, s);
    while (heapSize > 0) {
        HeapNode node = heapPop();
        int u = node.v;
        if (visited[u]) continue;
        visited[u] = 1;
        for (int i = 0; i < MAXV; i++) {
            if (!visited[i] && adj[u][i] < INF &&
                gDist[u]+adj[u][i] < gDist[i]) {
                gDist[i] = gDist[u]+adj[u][i];
                gPrev[i] = u;
                heapPush(gDist[i], i);
            }
        }
    }
}

int pickVertex(int mx, int my) {
    for (int i = 0; i < MAXV; i++) {
        double dx = mx-vexs[i].x, dy = my-vexs[i].y;
        if (sqrt(dx*dx+dy*dy) < 30) return i;
    }
    return -1;
}

int clickButton(Button *b, int mx, int my) {
    return mx>=b->x && mx<=b->x+b->w && my>=b->y && my<=b->y+b->h;
}

void drawButton(Button *b, int hover) {
    if (hover) setfillcolor(RGB(100,149,237));
    else       setfillcolor(RGB(200,200,200));
    solidrectangle(b->x,b->y,b->x+b->w,b->y+b->h);
    settextcolor(BLACK);
    settextstyle(14,0,L"微软雅黑");
    setbkmode(TRANSPARENT);
    outtextxy(b->x+12,b->y+10,b->text);
}

int buildPath(int start, int end, int path[]) {
    int len = 0, rev[MAXV];
    int cur = end;
    while (cur != -1) {
        rev[len++] = cur;
        if (cur == start) break;
        cur = gPrev[cur];
    }
    for (int i = 0; i < len; i++) path[i] = rev[len-1-i];
    return len;
}

void printAdj() {
    printf("===== 邻接矩阵 (%d 个顶点) =====\n", MAXV);
    for (int i = 0; i < MAXV; i++) {
        for (int j = 0; j < MAXV; j++) {
            if (adj[i][j]==INF) printf("   INF");
            else                printf("%6d", adj[i][j]);
        }
        printf("\n");
    }
}

void addHistory(int start, int end, int path[], int len) {
    if (histCount >= MAX_HIST) {
        for (int i = 0; i < MAX_HIST-1; i++)
            history[i] = history[i+1];
        histCount = MAX_HIST-1;
    }
    HistoryNode *h = &history[histCount++];
    h->start = start; h->end = end;
    h->pathLen = len; h->dist = gDist[end];
    for (int i = 0; i < len; i++) h->path[i] = path[i];
}

void showInfoPanel(int v) {
    setfillcolor(WHITE); setlinecolor(BLACK);
    rectangle(620,60,880,940); solidrectangle(621,61,879,939);
    settextcolor(BLACK); setbkmode(TRANSPARENT);
    settextstyle(22,0,L"微软雅黑");
    outtextxy(640,80,vexs[v].name);
    if (photoPath[v][0] != L'\0') {
        loadimage(&photo, photoPath[v], 240, 180);
        putimage(640,120,&photo);
    } else {
        setfillcolor(RGB(220,220,220));
        solidrectangle(640,120,880,300);
        settextcolor(RGB(128,128,128));
        settextstyle(16,0,L"微软雅黑");
        outtextxy(700,200,L"暂无照片");
        settextcolor(BLACK);
    }
    settextstyle(15,0,L"微软雅黑");
    outtextxy(640,320,L"--- 简介 ---");
    outtextxy(640,350,vexs[v].desc);
}

void render(int start, int end, int pathLen, int path[], int hoverBtn,
            int showList, int infoMode, int infoV, int showHist) {
    putimage(0,0,&mapBg);
    setfillcolor(RGB(0,0,0)); solidrectangle(0,0,WIN_W,40);
    settextcolor(WHITE); settextstyle(22,0,L"微软雅黑"); setbkmode(TRANSPARENT);
    outtextxy(220,8,L"哈工大(威海) 校园导航系统");

    if (pathLen >= 2) {
        setlinecolor(YELLOW); setlinestyle(PS_SOLID,6);
        for (int k = 0; k < pathLen-1; k++) {
            int a = path[k], b = path[k+1];
            line(vexs[a].x,vexs[a].y,vexs[b].x,vexs[b].y);
        }
    }
    for (int i = 0; i < MAXV; i++) {
        if (i==start)      setfillcolor(GREEN);
        else if (i==end)   setfillcolor(RED);
        else               setfillcolor(BLUE);
        solidcircle(vexs[i].x,vexs[i].y,9);
        settextstyle(11,0,L"微软雅黑"); settextcolor(WHITE);
        wchar_t id[8]; swprintf(id,8,L"%d",i);
        outtextxy(vexs[i].x-3,vexs[i].y-4,id);
        settextstyle(12,0,L"微软雅黑");
        outtextxy(vexs[i].x-22,vexs[i].y+10,vexs[i].name);
    }

    if (showList) {
        setfillcolor(RGB(255,255,255)); setlinecolor(BLACK);
        rectangle(10,50,220,50+18*(MAXV+1));
        solidrectangle(11,51,219,50+18*(MAXV+1)-1);
        settextcolor(BLACK); settextstyle(13,0,L"微软雅黑");
        outtextxy(16,55,L"--- 全部地点 ---");
        for (int i = 0; i < MAXV; i++) {
            wchar_t line[64];
            swprintf(line,64,L"%2d. %ls",i,vexs[i].name);
            outtextxy(16,53+16*(i+1),line);
        }
    }

    if (showHist) {
        setfillcolor(RGB(255,255,255)); setlinecolor(BLACK);
        int h = 60 + histCount*24 + 30;
        rectangle(10,50,300,h);
        solidrectangle(11,51,299,h-1);
        settextcolor(BLACK); settextstyle(14,0,L"微软雅黑");
        outtextxy(16,55,L"--- 历史记录 (点选重看) ---");
        if (histCount == 0) {
            outtextxy(16,85,L"暂无查询记录");
        }
        for (int i = 0; i < histCount; i++) {
            wchar_t line[80];
            swprintf(line,80,L"%d. %ls -> %ls  距:%d米",
                i+1, vexs[history[i].start].name,
                vexs[history[i].end].name, history[i].dist*2);
            outtextxy(16,80+i*24,line);
        }
    }

    if (pathLen >= 2 && !showList && !showHist) {
        setfillcolor(RGB(255,255,255)); setlinecolor(BLACK);
        rectangle(10,50,220,50+22*(pathLen+2));
        solidrectangle(11,51,219,50+22*(pathLen+2)-1);
        settextcolor(BLACK); settextstyle(13,0,L"微软雅黑");
        outtextxy(16,55,L"--- 最短路径 ---");
        for (int k = 0; k < pathLen; k++) {
            wchar_t line[64];
            swprintf(line,64,L"%d. %ls",k+1,vexs[path[k]].name);
            outtextxy(16,53+20*(k+1),line);
        }
        wchar_t distBuf[64];
        swprintf(distBuf,64,L"总距离: %d 米",gDist[end]*2);
        outtextxy(16,53+20*(pathLen+1),distBuf);
    }

    if (infoMode && infoV >= 0) showInfoPanel(infoV);

    settextcolor(WHITE); settextstyle(14,0,L"微软雅黑");
    if (infoMode)       outtextxy(620,960,L"详情模式: 点地点看照片");
    else if (start==-1) outtextxy(620,960,L"点蓝点选起点");
    else if (end==-1)   outtextxy(620,960,L"点终点");

    drawButton(&btnReset,  hoverBtn==0);
    drawButton(&btnList,   hoverBtn==1);
    drawButton(&btnInfo,   hoverBtn==2);
    drawButton(&btnHist,   hoverBtn==3);
    drawButton(&btnMat,    hoverBtn==4);
    drawButton(&btnExit,   hoverBtn==5);
}

int main(void) {
    initCampus();
    initgraph(WIN_W, WIN_H);
    loadimage(&mapBg, L"D:\\CampusNav\\map.jpg", WIN_W, WIN_H);

    int start=-1, end=-1;
    int path[MAXV], pathLen=0;
    int hoverBtn=-1, showList=0, infoMode=0, infoV=-1, showHist=0;
    render(start,end,pathLen,path,hoverBtn,showList,infoMode,infoV,showHist);

    MOUSEMSG m;
    while (true) {
        if (_kbhit()) {
            int c = _getch();
            if (c==27) break;
            if (c=='r'||c=='R') {
                start=-1;end=-1;pathLen=0;infoMode=0;infoV=-1;showHist=0;
                render(start,end,pathLen,path,hoverBtn,showList,infoMode,infoV,showHist);
            }
        }
        if (MouseHit()) {
            m = GetMouseMsg();
            if (m.uMsg == WM_MOUSEMOVE) {
                int old = hoverBtn; hoverBtn = -1;
                if      (clickButton(&btnReset,m.x,m.y)) hoverBtn=0;
                else if (clickButton(&btnList,m.x,m.y))  hoverBtn=1;
                else if (clickButton(&btnInfo,m.x,m.y))  hoverBtn=2;
                else if (clickButton(&btnHist,m.x,m.y))  hoverBtn=3;
                else if (clickButton(&btnMat,m.x,m.y))   hoverBtn=4;
                else if (clickButton(&btnExit,m.x,m.y))  hoverBtn=5;
                if (hoverBtn != old)
                    render(start,end,pathLen,path,hoverBtn,showList,infoMode,infoV,showHist);
            }
            if (m.uMsg == WM_LBUTTONDOWN) {
                if (clickButton(&btnReset,m.x,m.y)) {
                    start=-1;end=-1;pathLen=0;showHist=0;
                    render(start,end,pathLen,path,hoverBtn,showList,infoMode,infoV,showHist);
                    continue;
                }
                if (clickButton(&btnList,m.x,m.y)) {
                    showList = !showList; showHist = 0;
                    render(start,end,pathLen,path,hoverBtn,showList,infoMode,infoV,showHist);
                    continue;
                }
                if (clickButton(&btnInfo,m.x,m.y)) {
                    infoMode = !infoMode;
                    if (!infoMode) infoV = -1;
                    render(start,end,pathLen,path,hoverBtn,showList,infoMode,infoV,showHist);
                    continue;
                }
                if (clickButton(&btnHist,m.x,m.y)) {
                    showHist = !showHist; showList = 0;
                    render(start,end,pathLen,path,hoverBtn,showList,infoMode,infoV,showHist);
                    continue;
                }
                if (clickButton(&btnMat,m.x,m.y)) { printAdj(); continue; }
                if (clickButton(&btnExit,m.x,m.y)) break;

                if (showHist) {
                    for (int i = 0; i < histCount; i++) {
                        int y0 = 80 + i*24;
                        if (m.y >= y0 && m.y <= y0+20 && m.x >= 16 && m.x <= 290) {
                            start = history[i].start;
                            end = history[i].end;
                            pathLen = history[i].pathLen;
                            for (int j = 0; j < pathLen; j++)
                                path[j] = history[i].path[j];
                            showHist = 0;
                            render(start,end,pathLen,path,hoverBtn,showList,infoMode,infoV,showHist);
                            break;
                        }
                    }
                    continue;
                }

                int v = pickVertex(m.x,m.y);
                if (v >= 0) {
                    if (infoMode) infoV = v;
                    else {
                        showList = 0;
                        if (start == -1) start = v;
                        else if (end == -1 && v != start) {
                            end = v; dijkstra(start);
                            pathLen = buildPath(start,end,path);
                            addHistory(start,end,path,pathLen);
                        } else { start = v; end = -1; pathLen = 0; }
                    }
                    render(start,end,pathLen,path,hoverBtn,showList,infoMode,infoV,showHist);
                }
            }
        }
    }
    closegraph();
    return 0;
}

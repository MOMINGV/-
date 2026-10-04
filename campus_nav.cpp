#define _CRT_SECURE_NO_WARNINGS
#include <graphics.h>
#include <conio.h>
#include <stdio.h>
#include <math.h>
#include <windows.h>

#define MAXV 60
#define INF  0x3f3f3f3f
#define WIN_W 900
#define WIN_H 1000
#define MAX_HIST 10
#define HASH_SIZE 101
#define REAL_V 25

typedef struct {
    wchar_t name[32];
    int x, y;
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

typedef struct HashNode {
    wchar_t name[32];
    wchar_t photo[128];
    wchar_t desc[128];
    struct HashNode *next;
} HashNode;

HashNode hashTable[HASH_SIZE];

int hashFunc(const wchar_t *s) {
    unsigned int h = 0;
    while (*s) { h = h * 31 + (*s); s++; }
    return h % HASH_SIZE;
}

void hashInsert(const wchar_t *name, const wchar_t *photo, const wchar_t *desc) {
    int idx = hashFunc(name);
    HashNode *node = (HashNode *)malloc(sizeof(HashNode));
    wcscpy(node->name, name);
    wcscpy(node->photo, photo);
    wcscpy(node->desc, desc);
    node->next = hashTable[idx].next;
    hashTable[idx].next = node;
}

HashNode* hashSearch(const wchar_t *name) {
    int idx = hashFunc(name);
    HashNode *p = hashTable[idx].next;
    while (p) {
        if (wcscmp(p->name, name) == 0) return p;
        p = p->next;
    }
    return NULL;
}

void initHash() {
    for (int i = 0; i < HASH_SIZE; i++) hashTable[i].next = NULL;
    hashInsert(L"西门",            L"",                                 L"校园正门之一, 临近金海滩.");
    hashInsert(L"电子工程研究所",  L"",                                 L"位于校园西北侧, 临海而建.");
    hashInsert(L"探海楼",          L"",                                 L"教学主楼, 可俯瞰黄海.");
    hashInsert(L"图书馆",          L"D:\\CampusNav\\photos\\3.jpg",     L"学校地标建筑, 藏书丰富.");
    hashInsert(L"宁学楼",          L"D:\\CampusNav\\photos\\4.jpg",     L"教学楼, 靠近海棠园.");
    hashInsert(L"大学生活动中心",  L"D:\\CampusNav\\photos\\5.jpg",     L"学生活动与社团中心.");
    hashInsert(L"综合球类馆",      L"",                                 L"室内篮球羽毛球等场馆.");
    hashInsert(L"海棠园",          L"",                                 L"校园中心广场花园.");
    hashInsert(L"创新创业园",      L"",                                 L"学生创业孵化基地.");
    hashInsert(L"学子餐厅",        L"D:\\CampusNav\\photos\\9.jpg",     L"学生食堂之一.");
    hashInsert(L"学苑餐厅",        L"",                                 L"学生食堂之一.");
    hashInsert(L"七至十一公寓",    L"",                                 L"学生宿舍区.");
    hashInsert(L"一至五公寓",      L"",                                 L"学生宿舍区.");
    hashInsert(L"校史馆",          L"",                                 L"展示学校发展历程.");
    hashInsert(L"博学楼",          L"",                                 L"教学楼.");
    hashInsert(L"大学生服务中心",  L"D:\\CampusNav\\photos\\15.jpg",    L"一站式学生服务大厅.");
    hashInsert(L"明学楼",          L"D:\\CampusNav\\photos\\16.jpg",    L"教学楼.");
    hashInsert(L"功学楼",          L"D:\\CampusNav\\photos\\17.jpg",    L"教学楼.");
    hashInsert(L"主楼",            L"D:\\CampusNav\\photos\\18.jpg",    L"学校行政与教学主楼.");
    hashInsert(L"体育场",          L"",                                 L"标准塑胶跑道操场.");
    hashInsert(L"西南门",          L"",                                 L"校园西南出入口.");
    hashInsert(L"东门",            L"",                                 L"校园东侧出入口.");
    hashInsert(L"南校门",          L"",                                 L"校园正门.");
    hashInsert(L"山上公寓",        L"",                                 L"东区学生宿舍.");
    hashInsert(L"研究院",          L"",                                 L"科研与研究生教学楼.");
}

Vertex vexs[MAXV];
int   adj[MAXV][MAXV];
int gDist[MAXV];
int gPrev[MAXV];
int vertexCount = 0;

IMAGE mapBg;
IMAGE photo;

Button btnReset = { 20,  950, 80, 36, L"重新查询" };
Button btnList  = { 110, 950, 80, 36, L"地点列表" };
Button btnInfo  = { 200, 950, 80, 36, L"地点详情" };
Button btnHist  = { 290, 950, 80, 36, L"历史记录" };
Button btnMat   = { 380, 950, 80, 36, L"邻接矩阵" };
Button btnAdd   = { 470, 950, 80, 36, L"插入地点" };
Button btnDel   = { 560, 950, 80, 36, L"删除地点" };
Button btnExit  = { 650, 950, 80, 36, L"退出系统" };

HistoryNode history[MAX_HIST];
int histCount = 0;

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
    Vertex v[REAL_V] = {
        { L"西门",            216, 570 },
        { L"电子工程研究所",  241, 432 },
        { L"探海楼",          442, 360 },
        { L"图书馆",          552, 392 },
        { L"宁学楼",          446, 439 },
        { L"大学生活动中心",  545, 446 },
        { L"综合球类馆",      626, 434 },
        { L"海棠园",          431, 492 },
        { L"创新创业园",      318, 539 },
        { L"学子餐厅",        516, 531 },
        { L"学苑餐厅",        576, 592 },
        { L"七至十一公寓",    443, 563 },
        { L"一至五公寓",      545, 608 },
        { L"校史馆",          643, 634 },
        { L"博学楼",          583, 675 },
        { L"大学生服务中心",  521, 688 },
        { L"明学楼",          522, 704 },
        { L"功学楼",          578, 709 },
        { L"主楼",            670, 705 },
        { L"体育场",          520, 777 },
        { L"西南门",          308, 794 },
        { L"东门",            832, 723 },
        { L"南校门",          670, 886 },
        { L"山上公寓",        760, 479 },
        { L"研究院",          763, 843 },
    };
    for (int i = 0; i < REAL_V; i++) vexs[i] = v[i];

    int cross[][2] = {
        {629,458},{493,588},{494,663},{432,665},{335,762},
        {424,772},{493,727},{553,729},{553,668},{557,645},
        {560,608},{606,665},{606,729},{704,646},{681,597},
        {670,628},{670,668},{615,598},{489,775},{489,824},
        {523,853},{561,821},{667,835},{775,785},{826,807},
        {779,765},{730,728},{712,823},{603,880},
    };
    int ncross = sizeof(cross)/sizeof(cross[0]);
    for (int i = 0; i < ncross; i++) {
        wcscpy(vexs[REAL_V+i].name, L"");
        vexs[REAL_V+i].x = cross[i][0];
        vexs[REAL_V+i].y = cross[i][1];
    }
    vertexCount = REAL_V + ncross;

    for (int i = 0; i < MAXV; i++)
        for (int j = 0; j < MAXV; j++)
            adj[i][j] = (i == j) ? 0 : INF;

    int edges[][2] = {
        {1,2},{2,3},{2,4},{3,5},{4,7},{5,6},
        {5,25},{6,25},{25,23},
        {7,11},{7,8},{8,11},
        {9,11},{9,26},{26,11},{26,35},{35,10},{35,12},
        {35,42},{42,39},{39,40},{40,13},{40,38},{38,36},{38,41},
        {26,27},{27,33},{33,34},{34,35},
        {27,28},{28,30},{30,29},{29,20},
        {27,31},{31,32},{32,37},{37,17},{32,16},{31,43},
        {36,14},{33,14},{36,41},{41,18},
        {15,27},{15,31},
        {43,19},{43,44},{44,45},{45,46},{46,47},{47,53},{53,22},
        {47,52},{52,24},{52,48},{48,49},{49,21},{48,50},{50,51},{51,18},{50,24},
        {0,8},{0,20},{20,8},
        {19,22},
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
    for (int i = 0; i < vertexCount; i++) {
        gDist[i] = INF; visited[i] = 0; gPrev[i] = -1;
    }
    gDist[s] = 0;
    heapPush(0, s);
    while (heapSize > 0) {
        HeapNode node = heapPop();
        int u = node.v;
        if (visited[u]) continue;
        visited[u] = 1;
        for (int i = 0; i < vertexCount; i++) {
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
    for (int i = 0; i < vertexCount; i++) {
        if (vexs[i].name[0] == L'\0') continue;
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
    settextstyle(13,0,L"微软雅黑");
    setbkmode(TRANSPARENT);
    outtextxy(b->x+8,b->y+10,b->text);
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
    printf("===== 邻接矩阵 (%d 个顶点) =====\n", vertexCount);
    for (int i = 0; i < vertexCount; i++) {
        for (int j = 0; j < vertexCount; j++) {
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

void showInfoByName(const wchar_t *name) {
    HashNode *info = hashSearch(name);
    setfillcolor(WHITE); setlinecolor(BLACK);
    rectangle(620,60,880,940); solidrectangle(621,61,879,939);
    settextcolor(BLACK); setbkmode(TRANSPARENT);
    settextstyle(22,0,L"微软雅黑");
    outtextxy(640,80,name);
    if (info && info->photo[0] != L'\0') {
        loadimage(&photo, info->photo, 240, 180);
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
    if (info) outtextxy(640,350,info->desc);
    else      outtextxy(640,350,L"暂无简介");
}

void render(int start, int end, int pathLen, int path[], int hoverBtn,
            int showList, int showHist, int addMode, int delMode) {
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
    for (int i = 0; i < vertexCount; i++) {
        if (vexs[i].name[0] == L'\0') continue;
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
        rectangle(10,50,220,50+18*(REAL_V+1));
        solidrectangle(11,51,219,50+18*(REAL_V+1)-1);
        settextcolor(BLACK); settextstyle(13,0,L"微软雅黑");
        outtextxy(16,55,L"--- 全部地点 (点击查看) ---");
        int idx = 0;
        for (int i = 0; i < vertexCount; i++) {
            if (vexs[i].name[0] == L'\0') continue;
            wchar_t line[64];
            swprintf(line,64,L"%2d. %ls",i,vexs[i].name);
            outtextxy(16,53+16*(idx+1),line);
            idx++;
        }
    }

    if (showHist) {
        setfillcolor(RGB(255,255,255)); setlinecolor(BLACK);
        int h = 60 + histCount*24 + 30;
        rectangle(10,50,300,h);
        solidrectangle(11,51,299,h-1);
        settextcolor(BLACK); settextstyle(14,0,L"微软雅黑");
        outtextxy(16,55,L"--- 历史记录 (点选重看) ---");
        if (histCount == 0) outtextxy(16,85,L"暂无查询记录");
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
            if (path[k] < vertexCount && vexs[path[k]].name[0]) {
                swprintf(line,64,L"%d. %ls",k+1,vexs[path[k]].name);
            } else {
                swprintf(line,64,L"%d. 路口",k+1);
            }
            outtextxy(16,53+20*(k+1),line);
        }
        wchar_t distBuf[64];
        swprintf(distBuf,64,L"总距离: %d 米",gDist[end]*2);
        outtextxy(16,53+20*(pathLen+1),distBuf);
    }

    settextcolor(WHITE); settextstyle(14,0,L"微软雅黑");
    if (addMode)       outtextxy(620,960,L"插入模式: 点地图空白处, 控制台输英文名");
    else if (delMode)  outtextxy(620,960,L"删除模式: 点要删除的地点");
    else if (start==-1) outtextxy(620,960,L"点蓝点选起点, 或点左侧列表看详情");
    else if (end==-1) outtextxy(620,960,L"点终点");

    drawButton(&btnReset,  hoverBtn==0);
    drawButton(&btnList,   hoverBtn==1);
    drawButton(&btnInfo,   hoverBtn==2);
    drawButton(&btnHist,   hoverBtn==3);
    drawButton(&btnMat,    hoverBtn==4);
    drawButton(&btnAdd,    hoverBtn==5);
    drawButton(&btnDel,    hoverBtn==6);
    drawButton(&btnExit,   hoverBtn==7);
}

int main(void) {
    initCampus();
    initHash();
    initgraph(WIN_W, WIN_H);
    loadimage(&mapBg, L"D:\\CampusNav\\map.jpg", WIN_W, WIN_H);

    int start=-1, end=-1;
    int path[MAXV], pathLen=0;
    int hoverBtn=-1, showList=0, showHist=0;
    int addMode=0, delMode=0;
    render(start,end,pathLen,path,hoverBtn,showList,showHist,addMode,delMode);

    MOUSEMSG m;
    while (true) {
        if (_kbhit()) {
            int c = _getch();
            if (c==27) break;
            if (c=='r'||c=='R') {
                start=-1;end=-1;pathLen=0;showHist=0;
                render(start,end,pathLen,path,hoverBtn,showList,showHist,addMode,delMode);
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
                else if (clickButton(&btnAdd,m.x,m.y))   hoverBtn=5;
                else if (clickButton(&btnDel,m.x,m.y))   hoverBtn=6;
                else if (clickButton(&btnExit,m.x,m.y))  hoverBtn=7;
                if (hoverBtn != old)
                    render(start,end,pathLen,path,hoverBtn,showList,showHist,addMode,delMode);
            }
            if (m.uMsg == WM_LBUTTONDOWN) {
                if (clickButton(&btnReset,m.x,m.y)) {
                    start=-1;end=-1;pathLen=0;showHist=0;
                    render(start,end,pathLen,path,hoverBtn,showList,showHist,addMode,delMode);
                    continue;
                }
                if (clickButton(&btnList,m.x,m.y)) {
                    showList = !showList; showHist = 0;
                    render(start,end,pathLen,path,hoverBtn,showList,showHist,addMode,delMode);
                    continue;
                }
                if (clickButton(&btnInfo,m.x,m.y)) {
                    showList = !showList; showHist = 0;
                    render(start,end,pathLen,path,hoverBtn,showList,showHist,addMode,delMode);
                    continue;
                }
                if (clickButton(&btnHist,m.x,m.y)) {
                    showHist = !showHist; showList = 0;
                    render(start,end,pathLen,path,hoverBtn,showList,showHist,addMode,delMode);
                    continue;
                }
                if (clickButton(&btnMat,m.x,m.y)) { printAdj(); continue; }
                if (clickButton(&btnAdd,m.x,m.y)) {
                    addMode = !addMode; delMode = 0;
                    render(start,end,pathLen,path,hoverBtn,showList,showHist,addMode,delMode);
                    continue;
                }
                if (clickButton(&btnDel,m.x,m.y)) {
                    delMode = !delMode; addMode = 0;
                    render(start,end,pathLen,path,hoverBtn,showList,showHist,addMode,delMode);
                    continue;
                }
                if (clickButton(&btnExit,m.x,m.y)) break;

                if (addMode && m.x < 900 && m.y < 940) {
                    int newId = vertexCount;
                    if (newId < MAXV) {
                        char name[32];
                        printf("Enter new place name (english): ");
                        scanf("%s", name);
                        MultiByteToWideChar(CP_ACP, 0, name, -1, vexs[newId].name, 32);
                        vexs[newId].x = m.x;
                        vexs[newId].y = m.y;
                        for (int i = 0; i <= newId; i++) {
                            adj[newId][i] = INF;
                            adj[i][newId] = INF;
                        }
                        adj[newId][newId] = 0;

                        int connected[3] = {-1,-1,-1};
                        for (int k = 0; k < 3; k++) {
                            int best = -1, bestD = INF;
                            for (int i = 0; i < newId; i++) {
                                if (vexs[i].name[0] == L'\0') continue;
                                int already = 0;
                                for (int c = 0; c < k; c++) if (connected[c]==i) already=1;
                                if (already) continue;
                                int d = dist(newId, i);
                                if (d < bestD) { bestD = d; best = i; }
                            }
                            if (best >= 0) {
                                connected[k] = best;
                                adj[newId][best] = adj[best][newId] = bestD;
                            }
                        }
                        vertexCount++;
                        printf("Connected to 3 nearest places.\n");
                    }
                    addMode = 0;
                    start=-1;end=-1;pathLen=0;
                    render(start,end,pathLen,path,hoverBtn,showList,showHist,addMode,delMode);
                    continue;
                }

                if (delMode) {
                    int v = pickVertex(m.x,m.y);
                    if (v >= 0) {
                        printf("Deleted: %d\n", v);
                        vexs[v].name[0] = L'\0';
                        for (int i = 0; i < vertexCount; i++) {
                            adj[v][i] = INF;
                            adj[i][v] = INF;
                        }
                        adj[v][v] = 0;
                    }
                    delMode = 0;
                    start=-1;end=-1;pathLen=0;
                    render(start,end,pathLen,path,hoverBtn,showList,showHist,addMode,delMode);
                    continue;
                }

                if (showList) {
                    int idx = 0;
                    for (int i = 0; i < vertexCount; i++) {
                        if (vexs[i].name[0] == L'\0') continue;
                        int y0 = 53 + 16*(idx+1);
                        if (m.y >= y0 && m.y <= y0+14 && m.x >= 16 && m.x <= 210) {
                            showInfoByName(vexs[i].name);
                            break;
                        }
                        idx++;
                    }
                    continue;
                }

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
                            render(start,end,pathLen,path,hoverBtn,showList,showHist,addMode,delMode);
                            break;
                        }
                    }
                    continue;
                }

                int v = pickVertex(m.x,m.y);
                if (v >= 0) {
                    showList = 0;
                    if (start == -1) start = v;
                    else if (end == -1 && v != start) {
                        end = v; dijkstra(start);
                        pathLen = buildPath(start,end,path);
                        addHistory(start,end,path,pathLen);
                    } else { start = v; end = -1; pathLen = 0; }
                    render(start,end,pathLen,path,hoverBtn,showList,showHist,addMode,delMode);
                }
            }
        }
    }
    closegraph();
    return 0;
}

#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#include<stdio.h>
#include<graphics.h>
#include<easyx.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include<Windows.h>
#include<conio.h>
#include<math.h>
#include<mmsystem.h>
#pragma comment(lib,"Winmm.lib")


#define bullet_num 200			//控制子弹数量上限
#define enemy_num 50			//控制敌机数量上限
#define enemy_bullet_num 50	//控制敌机子弹数量上限
#define prop_num 50				//控制道具数量上限
#define tracing_bullet_num 20	//控制追踪弹数量上限
#define boss_bullet_num 500		//控制BOSS弹幕数量上限
#define BOSS_TIME 60000			//控制BOSS生成时间
#define BOSS_HP 1500			//控制BOSS血量

IMAGE img_enemy, img_heroblack[10], img_map, img_menu1, img_menu2, img_enemy1,
img_shop, img_herowhite[10], img_bullet1black, img_bullet1white,
img_shopblack, img_shopwhite, sheet, img_enemy1black, img_enemy1white,
img_maps[10], img_enemybulletblack[10], img_tracingbullet_black[3], img_tracingbullet_white[3],
img_enemybulletwhite[10], img_bg_shop[8], img_hero_shop[4], img_hp_white,
img_hp_black, img_enemy_hit_black[7], img_enemy_hit_white[7], img_power_small,
img_power_big, img_score, img_fire_max, img_herodot_black, img_herodot_white, img_gameover,
img_boss_black, img_boss_white, img_pause[3], img_end[3], img_plot[4], img_talkframe, img_helpbg;
clock_t start, stop, middle;

typedef struct enemy_data {//敌人数据结构体
	float x;
	float y;
	int type;
	int hp;
	bool live;
	int hit;
}ENEMY;
ENEMY enemy[enemy_num];

typedef struct shop_data {//商店所卖的地图和角色
	IMAGE *map;
	IMAGE *hero;
	int flag;
	int num;
	struct shop_data* next;
}SHOP;


typedef struct bullet_data {//子弹数据结构体
	int x;
	int y;
	bool live;
}BULLET;
BULLET bullet[bullet_num];

typedef struct tracingbullet_data {//追踪弹结构体
	double x;
	double y;
	double vx;
	double vy;
	double k;
	double s;//路程
	bool live;
	int flag;//0为初始状态，1为开始追踪状态，2为追踪中，3为命中
	int dir;//1为左侧，2为右侧
}TRACINGBULLET;
TRACINGBULLET tracingbullet[tracing_bullet_num];

typedef struct enemy_bullet_data {//子弹数据结构体
	float x;
	float y;
	float vx;
	float vy;
	int vtype;
	bool live;
	int grazed;
	struct enemy_bullet_data* next;
}ENEMY_BULLET;
ENEMY_BULLET enemybullet[enemy_bullet_num];

typedef struct hero_data {//自机数据结构体
	double x;
	double y;
	double dotx;
	double doty;
	int hp;
	double vx;
	double vy;
	double power;
	int boom;
	int level;
	int last_level;
	int powerflag;
	IMAGE* img_black;
	IMAGE* img_white;
}HERO;
HERO hero;

typedef struct hero_hp {//血量图片结构体
	int x;
	int y;
	int num;
	struct hero_hp* next;
}HP;

typedef struct prop_data {//道具结构体
	float x;
	float y;
	bool live;
	int state;
	float vy;
	float vx;
	float start_y;
	IMAGE* img;
}PROP;
PROP prop[prop_num];

typedef struct save_data {//数据结构体
	int money;
	int maxscore;
	int score;
	int flag;
	int graze;
}SAVE;
SAVE data = { 0 };

typedef struct boss_data {//BOSS数据结构体
	float x;
	float y;
	double hp;//生命值
	int lives;
	int stage;
	bool live;
	int dir;//移动方向控制 -1为向左 0为初始 1为向右
	float vx;
	float vy;
	IMAGE* img_black;
	IMAGE* img_white;
}BOSS;
BOSS boss;

typedef struct boss_bullet_data {//BOSS子弹结构体
	double x;
	double y;
	double vx;
	double vy;
	double A;
	double angel;
	bool live;
	int vtype;
	int grazed;
}BOSS_BULLET;
BOSS_BULLET bossbullet[boss_bullet_num];
double map_y;//地图背景图片坐标
void ChooseMap();//选择地图
void ImageInit();//图片初始化
void GameInit();//游戏初始化
void GameStart();//游戏开始
void HeroMove();//控制自机移动
void BulletMove();//子弹移动
void GameOver();//游戏结束
void BulletCreate();//创建子弹
bool timer(int ms, int id);//计时器编写
void ShopStart();//打开商店
void BackMenu();//返回菜单
void ClickStart();//点击开始
void EnemyCreate(int *flag);//敌机创建
void EnemyInit();//敌机初始化
void EnemyMove();//敌机移动
int HitCheck();//碰撞检测
void MoneyInit();//金币初始化
void ScoreInit();//分数初始化
void ScoreAdd();//分数增加
void MoneyAdd();//金币增加
void HeroInit();//自机初始化
void EndGame();//游戏彻底结束
void EnemyBulletCreate();//敌机子弹创建
void EnemyBulletMove();//敌机子弹移动
void EnemyBulletMoveWays(int i);//敌方弹道
//void HpShow();//血量初始化
void ShopBgDraw(SHOP* head);//超市背景绘图
SHOP* CreateList();//商店链表创建
void ShopPurchase(SHOP* head);//超市购买
void HelpStart();//游戏说明启动
void HpUp();//生命值增加
HP* HpNodeCreate();//生命值链表创建
void HpShow(HP* head);//生命值显示
void HpMinus(HP* p, HP* head);//生命链表删除节点
void HpAdd(HP* p);//生命链表增加节点
void EnemyHitAnimation();//敌人受击动画
void PropInit();//道具初始化
void PropCreate(int j);//道具生成
void PropMove();//道具移动
void PowerShow();//P点显示
void HeroLevel();//角色等级变化
void PropXyLimit();//限制道具上升距离
void AllPropsGet();//所有道具回归自机
void TracingBulletInit();//追踪弹初始化
void TracingBulletCreate();//追踪弹创建
void TracingBulletMove();//追踪弹移动
void GetMinDistance(int data[]);//获取最近敌机数据
void HittedPropCreate();//自机产p点
void BossInit();//Boss初始化
void BossCreate();//Boss生成
void BossMove();//Boss移动
void IfBossCreate(int* timeflag,int* plotflag);//判断Boss是否应该生成
void BossBulletInit();//Boss弹幕初始化
void BossBulletCreate(double* angelflag);//Boss弹幕生成
void BossBulletMove();//Boss弹幕移动
void BossHpShow();//Boss血量显示
void GameOverBox();//游戏结束提示框
void BossDeath(int* plotflag);//Boss死亡
void GamePlot(int* plotflag,int num);//游戏剧情
void MapMove(int num);//地图移动
void PowerUpMusic();//升级音乐

int main() {
	//生成随机种子
	srand((unsigned)time(NULL));
	//打开一个图形窗口
	HWND hwnd = initgraph(850,800, SHOWCONSOLE);
	ImageInit();
	GameInit();//敌人、地图、自机、读取文件内容
	closegraph();
	return 0;
}

void ImageInit() {//图片初始化
	loadimage(&img_menu1, L"Game\\bg.jpg", 500, 800);
	loadimage(&img_menu2, L"Game\\灵梦背景cut.jpg", 350, 800);
	loadimage(&img_shopblack, L"Game\\shopblack.jpg", 102, 104);
	loadimage(&img_shopwhite, L"Game\\shopwhile.jpg", 102, 104);
	//loadimage(&img_map, L"AircraftWar\\maps\\map1.jpg", 500, 800);
	loadimage(&img_maps[0], L"Game\\bg_shop\\map0.jpg", 500, 800);
	loadimage(&img_maps[1], L"Game\\bg_shop\\bg1_shop.jpg", 500, 800);
	loadimage(&img_maps[2], L"Game\\bg_shop\\bg2_shop.jpg", 500, 800);
	loadimage(&img_maps[3], L"Game\\bg_shop\\bg3_shop.jpg", 500, 800);
	loadimage(&img_maps[4], L"Game\\bg_shop\\bg4_shop.jpg", 500, 800);
	loadimage(&img_maps[5], L"Game\\bg_shop\\bg5_shop.jpg", 500, 800);
	loadimage(&img_maps[6], L"Game\\bg_shop\\bg6_shop.jpg", 500, 800);
	loadimage(&img_maps[7], L"Game\\bg_shop\\bg7_shop.jpg", 500, 800);
	loadimage(&img_maps[8], L"Game\\bg_shop\\bg8_shop.jpg", 500, 800);
	loadimage(&img_heroblack[0], L"Game\\heroes\\hero1black.jpg", 35, 50);
	loadimage(&img_herowhite[0], L"Game\\heroes\\hero1white.jpg", 35, 50);
	loadimage(&img_heroblack[1], L"Game\\heroes\\hero2black.jpg", 35, 50);
	loadimage(&img_herowhite[1], L"Game\\heroes\\hero2white.jpg", 35, 50);
	loadimage(&img_bullet1black, L"Game\\bullet\\redbullet_black.jpg", 20, 20);
	loadimage(&img_bullet1white, L"Game\\bullet\\redbullet_white.jpg", 20, 20);
	loadimage(&img_herodot_black, L"Game\\bullet\\redbullet_black.jpg", 17, 17);
	loadimage(&img_herodot_white, L"Game\\bullet\\redbullet_white.jpg", 17, 17);
	loadimage(&sheet, L"Game\\bg_shop\\sheet.jpg", 500, 800);
	loadimage(&img_enemy1,L"Game\\enemy\\enemy_1.jpg",28,31);
	loadimage(&img_enemy1black, L"Game\\enemy\\enemy1black.jpg", 48, 37);
	loadimage(&img_enemy1white, L"Game\\enemy\\enemy1white.jpg", 48, 37);
	loadimage(&img_enemybulletblack[0], L"Game\\bullet\\bluebullet_black.jpg", 18, 18);
	loadimage(&img_enemybulletwhite[0], L"Game\\bullet\\bluebullet_white.jpg", 18, 18);
	loadimage(&img_enemybulletblack[1], L"Game\\bullet\\yellowbullet_black.jpg", 18, 18);
	loadimage(&img_enemybulletwhite[1], L"Game\\bullet\\yellowbullet_white.jpg", 18, 18);
	loadimage(&img_enemybulletblack[2], L"Game\\bullet\\greenbullet_black.jpg", 18, 18);
	loadimage(&img_enemybulletwhite[2], L"Game\\bullet\\greenbullet_white.jpg", 18, 18);
	loadimage(&img_tracingbullet_white[0], L"Game\\bullet\\tracing1_white.jpg", 25, 25);
	loadimage(&img_tracingbullet_black[0], L"Game\\bullet\\tracing1_black.jpg", 25, 25);
	loadimage(&img_tracingbullet_white[1], L"Game\\bullet\\tracing2_white.jpg", 25, 25);
	loadimage(&img_tracingbullet_black[1], L"Game\\bullet\\tracing2_black.jpg", 25, 25);
	loadimage(&img_tracingbullet_white[1], L"Game\\bullet\\tracing3_white.jpg", 25, 25);
	loadimage(&img_tracingbullet_black[1], L"Game\\bullet\\tracing3_black.jpg", 25, 25);
	loadimage(&img_bg_shop[0], L"Game\\bg_shop\\bg1_shop.jpg", 85, 120);
	loadimage(&img_bg_shop[1], L"Game\\bg_shop\\bg2_shop.jpg", 85, 120);
	loadimage(&img_bg_shop[2], L"Game\\bg_shop\\bg3_shop.jpg", 85, 120);
	loadimage(&img_bg_shop[3], L"Game\\bg_shop\\bg4_shop.jpg", 85, 120);
	loadimage(&img_bg_shop[4], L"Game\\bg_shop\\bg5_shop.jpg", 85, 120);
	loadimage(&img_bg_shop[5], L"Game\\bg_shop\\bg6_shop.jpg", 85, 120);
	loadimage(&img_bg_shop[6], L"Game\\bg_shop\\bg7_shop.jpg", 85, 120);
	loadimage(&img_bg_shop[7], L"Game\\bg_shop\\bg8_shop.jpg", 85, 120);
	loadimage(&img_hp_white, L"Game\\hp_white.jpg", 34, 30);
	loadimage(&img_hp_black, L"Game\\hp_black.jpg", 34, 30);
	loadimage(&img_enemy_hit_black[0], L"Game\\enemy\\enemy1black.jpg", 48, 37);
	loadimage(&img_enemy_hit_black[1], L"Game\\enemy\\enemy1_hit1_black.jpg", 48, 37);
	loadimage(&img_enemy_hit_black[2], L"Game\\enemy\\enemy1_hit2_black.jpg", 48, 37);
	loadimage(&img_enemy_hit_black[3], L"Game\\enemy\\enemy1_hit3_black.jpg", 48, 37);
	loadimage(&img_enemy_hit_black[4], L"Game\\enemy\\enemy1_hit4_black.jpg", 48, 37);
	loadimage(&img_enemy_hit_black[5], L"Game\\enemy\\enemy1_hit5_black.jpg", 48, 37);
	loadimage(&img_enemy_hit_black[6], L"Game\\enemy\\enemy1_hit6_black.jpg", 48, 37);
	loadimage(&img_enemy_hit_white[0], L"Game\\enemy\\enemy1white.jpg", 48, 37);
	loadimage(&img_enemy_hit_white[1], L"Game\\enemy\\enemy1_hit1_white.jpg", 48, 37);
	loadimage(&img_enemy_hit_white[2], L"Game\\enemy\\enemy1_hit2_white.jpg", 48, 37);
	loadimage(&img_enemy_hit_white[3], L"Game\\enemy\\enemy1_hit3_white.jpg", 48, 37);
	loadimage(&img_enemy_hit_white[4], L"Game\\enemy\\enemy1_hit4_white.jpg", 48, 37);
	loadimage(&img_enemy_hit_white[5], L"Game\\enemy\\enemy1_hit5_white.jpg", 48, 37);
	loadimage(&img_enemy_hit_white[6], L"Game\\enemy\\enemy1_hit6_white.jpg", 48, 37);
	loadimage(&img_power_small,L"Game\\道具\\power_small.jpg",15,15);
	loadimage(&img_power_big, L"Game\\道具\\power_big.jpg", 25, 25);
	loadimage(&img_score, L"Game\\道具\\score.jpg", 15, 15);
	loadimage(&img_boss_black , L"Game\\enemy\\boss_black.jpg", 50, 60);
	loadimage(&img_boss_white, L"Game\\enemy\\boss_white.jpg", 50, 60);
	loadimage(&img_pause[0], L"Game\\pause1.jpg", 740, 640);
	loadimage(&img_pause[1], L"Game\\pause2.jpg", 740, 640);
	loadimage(&img_pause[2], L"Game\\pause3.jpg", 740, 640);
	loadimage(&img_end[0], L"Game\\End1.jpg", 740, 640);
	loadimage(&img_end[1], L"Game\\End2.jpg", 740, 640);
	loadimage(&img_end[2], L"Game\\End3.jpg", 740, 640);
	loadimage(&img_plot[0], L"Game\\boss\\琪露诺对话black.jpg", 142, 155);
	loadimage(&img_plot[1], L"Game\\boss\\琪露诺对话white.jpg", 142, 155);
	loadimage(&img_plot[2], L"Game\\boss\\博丽灵梦对话black.jpg", 142, 155);
	loadimage(&img_plot[3], L"Game\\boss\\博丽灵梦对话white.jpg", 142, 155);
	loadimage(&img_talkframe, L"Game\\boss\\talkframe.jpg", 300, 200);
	loadimage(&img_gameover, L"Game\\gameover.jpg", 740, 640);
	loadimage(&img_helpbg, L"Game\\helpbg.jpg", 850, 800);
}
SHOP* shophead = CreateList();

void GameInit() {//初始化
	//关闭所有音乐文件
	mciSendString(L"close music1", NULL, NULL, NULL);
	mciSendString(L"close music2", NULL, NULL, NULL);
	mciSendString(L"close music3", NULL, NULL, NULL);
	mciSendString(L"close music4", NULL, NULL, NULL);
	mciSendString(L"close music5", NULL, NULL, NULL);
	mciSendString(L"close music6", NULL, NULL, NULL);
	setbkmode(TRANSPARENT);//设置背景颜色为透明
	int play = rand() % 3;//随机播放三首主菜单bgm
	if (play == 0) {
		mciSendString(L"open Game\\music\\常世之乡repeat.mp3 alias music1", NULL, NULL, NULL);
	}
	else if (play == 1) {
		mciSendString(L"open Game\\music\\萃梦想repeat.MP3 alias music1", NULL, NULL, NULL);
	}
	else if(play == 2) {
		mciSendString(L"open Game\\music\\永夜抄repeat.MP3 alias music1", NULL, NULL, NULL);
	}
	mciSendString(L"close music2", NULL, NULL, NULL);
	mciSendString(L"play music1 repeat", NULL, NULL, NULL);
	BeginBatchDraw();
	putimage(0, 0, &img_menu1);
	putimage(500, 0, &img_menu2);
	HeroInit();
	EnemyInit();
	BossInit();
	MoneyInit();
	BossBulletInit();
	PropInit();
	setlinestyle(PS_SOLID,5);
	line(500,0,500,800);
	for (int i = 0; i < bullet_num; i++) {
		bullet[i].live = false;
		bullet[i].y = -500;
	}
	for (int i = 0; i < enemy_bullet_num; i++) {
		enemybullet[i].live = false;
		enemybullet[i].y = -500;
	}
	TracingBulletInit();
	EndBatchDraw();
	ClickStart();
}

void ClickStart() {
	MOUSEMSG msg = { 0 };
	while (1) {
		msg = GetMouseMsg();
		if(msg.uMsg == WM_LBUTTONDOWN && timer(500,2)) {//左键点击
			if (msg.x >= 410 && msg.x <= 480 && msg.y >= 340 && msg.y <= 410 && timer(500,13)) {
				ShopStart();//判定进入商城
			}
			else if (msg.x >= 175 && msg.x <= 345 && msg.y >= 510 && msg.y <= 565) {
				GameStart();//判定开始游戏
			}
			else if (msg.uMsg == WM_LBUTTONDOWN && timer(500, 16)) {//左键点击
				if (msg.x >= 180 && msg.x <= 340 && msg.y >= 605 && msg.y <= 655 && timer(500, 13)) {
					HelpStart();//判定进入帮助说明
				}
			}
		}
	}
}


void HelpStart() {//帮助说明
	putimage(0, 0, &img_helpbg);
	while (1) {
		if (GetAsyncKeyState(0x1B)) {
			GameInit();
		}
	}
}

int flags[8] = {0};
SHOP* CreateList() {//商店链表创建(头插法)
	SHOP* head = NULL, * p;
	for (int i = 0; i < 8; i++) {
		p = (SHOP*)malloc(sizeof(SHOP));
		if (p != NULL) {
			p->map = &img_bg_shop[7-i];
			p->num = 8 - i;
			p->next = head;
			head = p;
		}
	}
	return head;
}

void ShopStart() {//打开商店
	MOUSEMSG msg = { 0 };
	settextstyle(30, 0, _T("微软雅黑"), 0, 0, FW_BOLD, 0, 0, 0);
	settextcolor(WHITE);
	setlinecolor(RGB(155, 155, 155));
	setfillcolor(RGB(155, 155, 155));
	fillrectangle(520,200,620,230);
	outtextxy(520,200,_T("金币数量："));
	wchar_t summoney[20];
	_itow_s(data.money, summoney, 10);//转换数据类型以通过outtextxy输出
	fillrectangle(700, 200, 800, 230);
	outtextxy(700, 200, summoney);
	putimage(0, 0, &sheet);
	ShopBgDraw(shophead);
	ShopPurchase(shophead);
	GameInit();
}

void ShopBgDraw(SHOP* head) {//商店绘图
	SHOP* p = head;
	FILE* fp = fopen("Game\\shopbgsave.txt", "r");
	while(p != NULL) {
		if (fp == NULL) {
			perror("fopen");
			return;
		}
		fscanf(fp, "%d", &p->flag);
		flags[p->num-1] = p->flag;
		if (p->flag == 1) {
			if (p->num <= 4) {
				putimage(97 * (p->num - 1) + 60, 84, p->map);
			}
			else {
				putimage(97 * (p->num - 5) + 60, 296, p->map);
			}
		}
		p = p->next;
	}
	fclose(fp);
}

void ShopPurchase(SHOP* head) {//商店购买
	SHOP* p = head;
	MOUSEMSG msg = { 0 };
	while (1) {
		msg = GetMouseMsg();
		if (GetAsyncKeyState(0x1B) && timer(500,49)) {
			GameInit();
		}
		if (msg.uMsg == WM_LBUTTONDOWN) {
			while (p != NULL) {
				if (msg.x >= 67 + 100 * (p->num - 1) && msg.x <= 137 + 100 * (p->num - 1) && msg.y >= 247 && msg.y <= 267 && p->num <= 4) {
					printf("%d", p->num);
					if (data.money >= 600) {
						p->flag = 0;
						data.money -= 600;
					}
					FILE* fp = fopen("Game\\save.txt", "r+");
					if (fp == NULL) {
						perror("fopen");
						return;
					}
					fprintf(fp, "%d", data.money);
					fclose(fp);
					MoneyAdd();
					flags[(p->num) - 1] = p->flag;
					p = head;
					break;
				}
				else if (msg.x >= 67 + 100 * (p->num - 5) && msg.x <= 137 + 100 * (p->num - 5) && msg.y >= 460 && msg.y <= 480 && p->num > 4) {
					printf("%d", p->num);
					if (data.money >= 600) {
						p->flag = 0;
						data.money -= 600;
					}
					FILE* fp = fopen("Game\\save.txt", "r+");
					if (fp == NULL) {
						perror("fopen");
						return;
					}
					fprintf(fp, "%d", data.money);
					fclose(fp);
					MoneyAdd();
					flags[(p->num) - 1] = p->flag;
					p = head;
					break;
				}
				p = p->next;
			}
			FILE* fp = fopen("Game\\shopbgsave.txt", "r+");
			if (fp == NULL) {
				perror("fopen");
				return;
			}
			fprintf(fp,"%d %d %d %d %d %d %d %d", flags[0], flags[1], flags[2], flags[3], flags[4], flags[5], flags[6], flags[7]);
			fclose(fp);
			putimage(0, 0, &sheet);
			ShopBgDraw(head);
		}
	}
}

void GameStart() {//游戏主程序开始
	mciSendString(L"close music1", NULL, NULL, NULL);
	mciSendString(L"open Game\\music\\东方绯想天repeat.MP3 alias music2", NULL, NULL, NULL);
	mciSendString(L"play music2 repeat", NULL, NULL, NULL);
	start = clock();//游戏计时
	ScoreInit(); 
	int flag = 0;
	int bossflag = 0;
	int num;
	while(1){
		num = rand() % 9;
		if (num == 0) {
			break;
		} 
		else if(flags[num - 1] == 0) {
			break;
		}
	}
	HP* hphead = HpNodeCreate();
	int* enemyflag = (int*)malloc(sizeof(int));
	*enemyflag = 1;
	int *timeflag = (int*)malloc(sizeof(int));
	*timeflag = -1;
	/*double* bosscreateflag = (double*)malloc(sizeof(double));
	*bosscreateflag = 1;*/
	double* angelflag = (double*)malloc(sizeof(double));
	*angelflag = 0;
	int* endflag = (int*)malloc(sizeof(int));
	*endflag = 1;
	int* plotflag = (int*)malloc(sizeof(int));
	*plotflag = 0;
	while (1) {
		BackMenu();
		BeginBatchDraw();
		MapMove(num);
		if (flag >= 100) {
			IfBossCreate(timeflag,plotflag);
			if (*timeflag == -1) {
				EnemyCreate(enemyflag);
			}
			else if (*timeflag >= 0) {	
				bossflag++;
				if (bossflag >= 300) {
					if (*timeflag == 1) {
						BossCreate();
						*timeflag = 2;
					}
					if (*timeflag == 2) {
						BossMove();
						BossBulletCreate(angelflag);
						BossBulletMove();
						BossDeath(plotflag);
					}
				}		
			}
			EnemyMove();
			if (timer(3000, 7)) {
				EnemyBulletCreate();
			}
			EnemyBulletMove();
		}
		else {
			flag++;
		}
		if (GetAsyncKeyState(0x5A)) {	//z 0x5A
			BulletCreate();
		}
		BulletMove();
		putimage(500, 0, &img_menu2);
		ScoreAdd();
		MoneyAdd();
		HeroMove();
		HpUp();
		HpShow(hphead);
		if (HitCheck() == 1) {
			GameOverBox();
		}	
		HeroLevel();
		EnemyHitAnimation();
		PropMove();
		PropXyLimit();
		PowerShow();
		AllPropsGet();
		if (*plotflag > 0) {
			GamePlot(plotflag, num);
		}
		EndBatchDraw();
		PowerUpMusic();
	}
	GameOver();
}

void HeroInit() {//自机初始化
	hero.x = 220;
	hero.y = 600;
	hero.dotx = hero.x + 9;
	hero.doty = hero.y + 13;
	hero.hp = 3;
	hero.level = 1;
	hero.last_level = 1;//用于判断power变化后等级提升还是下降
	hero.power = 0;
	hero.vx = 4;
	hero.vy = 4;
	hero.powerflag = 0;
	int flag = 0;
	//原计划利用改变flag设计多位自机角色，最终因时间原因未实现
	hero.img_black = &img_heroblack[flag];
	hero.img_white = &img_herowhite[flag];
}

void HeroLevel() {//自机等级
	if (hero.power >= 0 && hero.power < 1) {
		hero.level = 1;
	}
	else if (hero.power >= 1 && hero.power < 2) {
		hero.level = 2;
	}
	else if (hero.power >= 2 && hero.power < 3) {
		hero.level = 3;
	}
	else if (hero.power >= 3 && hero.power < 4) {
		hero.level = 4;
	}
	else if (hero.power >= 4) {
		hero.level = 5;
	}
}

void PowerUpMusic() {//POWER升级播放音效
	if (hero.level - hero.last_level > 0.99) {
		mciSendString(L"close music5", NULL, NULL, NULL);
		mciSendString(L"open Game\\music\\powerup.wav alias music5", NULL, NULL, NULL);
		mciSendString(L"play music5", NULL, NULL, NULL);
		hero.last_level = hero.level;
	}
	hero.powerflag = 4;
	if (hero.powerflag == 4) {
		hero.powerflag = 5;
	}
}

void HeroMove() {//自机移动
	hero.dotx = hero.x + 9;
	hero.doty = hero.y + 13;
	putimage(hero.x, hero.y, hero.img_black, NOTSRCERASE);
	putimage(hero.x, hero.y, hero.img_white, SRCINVERT);
	putimage(hero.dotx, hero.doty, &img_herodot_black, NOTSRCERASE);
	putimage(hero.dotx, hero.doty, &img_herodot_white, SRCINVERT);
	//检测移动方向按键
	if (GetAsyncKeyState(0x43)) {//C 0x43
		hero.vx = 2;
		hero.vy = 2;
	}
	else{
		hero.vx = 4;
		hero.vy = 4;
	}
	if (GetAsyncKeyState(0x26)) {//↑ 0x26
		if (hero.y >= 50) {
			hero.y -= hero.vy;
		}
	}
	if (GetAsyncKeyState(0x28)) {//↓ 0x28
		if (hero.y <= 740) {
			hero.y += hero.vy;
		}
	}
	if (GetAsyncKeyState(0x25)) {//← 0x25
		if (hero.x >= 5) {
			hero.x -= hero.vx;
		}
	}
	if (GetAsyncKeyState(0x27)) {//→ 0x27
		if (hero.x <= 465) {
			hero.x += hero.vx;
		}
	}
}

bool timer(int ms, int id) {//计时器函数
	static DWORD t[50];
	if (clock() - t[id] >= ms) {
		t[id] = clock();
		return true;
	}
	return false;
}

void BulletCreate() {//自机子弹创建
	for (int i = 0; i < bullet_num; i++) {
		if (!bullet[i].live && timer(150, 0)) {
			bullet[i].x = hero.x + 7;
			bullet[i].y = hero.y - 15;			
			bullet[i].live = true;
			break;
		}
	}
	if (hero.level >= 2) {
		for (int i = 0; i < bullet_num; i++) {
			if (!bullet[i].live && timer(150, 10)) {
				bullet[i].x = hero.x - 15;
				bullet[i].y = hero.y + 5;
				bullet[i].live = true;
				break;
			}
			for (int i = 0; i < bullet_num; i++) {
				if (!bullet[i].live && timer(150, 11)) {
					bullet[i].x = hero.x + 30;
					bullet[i].y = hero.y + 5;
					bullet[i].live = true;
					break;
				}
			}
		}
		if (hero.level >= 3 && timer(500,31)) {
			TracingBulletCreate();
		}
		if (hero.level >= 4 && timer(500,32)) {
			TracingBulletCreate();
		}
	}
	
}

void BulletMove() {//自机子弹移动
	for (int i = 0; i < bullet_num; i++) {
		if (bullet[i].live) {
			bullet[i].y -= 15;
			if (bullet[i].x < 500) {
				putimage(bullet[i].x, bullet[i].y, &img_bullet1black, NOTSRCERASE);
				putimage(bullet[i].x, bullet[i].y, &img_bullet1white, SRCINVERT);
			}
			if (bullet[i].y < -100) {
				bullet[i].live = false;
			}
		}
	}
	TracingBulletMove();
}

void BackMenu() {
	int flag = 0;
	if (GetAsyncKeyState(0x1B)) {//检测是否返回主菜单 ESC 0x1B
		while (1) {//检测到按下ESC，进入暂停界面，使用while定住
			putimage(50, 100, &img_pause[flag]);
			FlushBatchDraw();
			if (GetAsyncKeyState(0x26)) {
				flag = 1;
			}
			else if (GetAsyncKeyState(0x28)) {
				flag = 2;
			}
			if (GetAsyncKeyState(0x5A)) {
				if (flag == 1) {
					start = clock();
					break;
				}
				else if (flag == 2) {
					mciSendString(L"close music2", NULL, NULL, NULL);
					GameOver();
				}
			}
		}
	}
}

void MoneyInit() {//初始化金币
	//打开文件，读入金币
	FILE* fp = fopen("Game\\save.txt","r");
	if(fp == NULL){
		perror("fopen");
		return ;
	}
	fscanf(fp, "%d\n%d", &data.money, &data.maxscore);
	fclose(fp);
}

void ScoreInit() {//初始化分数
	data.score = 0;
}

void ScoreAdd() {
	settextcolor(WHITE);
	setlinecolor(RGB(155, 155, 155));
	setfillcolor(RGB(155, 155, 155));
	fillrectangle(520, 250, 600, 280);
	outtextxy(520, 250, _T("残机数："));
	settextstyle(30, 0, _T("微软雅黑"), 0, 0, FW_BOLD, 0, 0, 0);
	fillrectangle(520, 100, 600, 130);
	outtextxy(520, 100, L"SCORE:");
	fillrectangle(520, 50, 655, 80);
	outtextxy(520, 50, L"MAXSCORE:");
	fillrectangle(520, 370, 610, 400);
	outtextxy(520, 370, L"GRAZE:");
	wchar_t grazebullet[20];
	_itow_s(data.graze,grazebullet,10);//转换数据类型以通过outtextxy输出
	fillrectangle(650, 370, 700, 400);
	outtextxy(650, 370, grazebullet);
	wchar_t maxscore[20];
	_itow_s(data.maxscore, maxscore, 10);//转换数据类型以通过outtextxy输出
	fillrectangle(700, 50, 800, 80);
	outtextxy(700, 50, maxscore);
	//data.score++;
	wchar_t sumscore[20];
	_itow_s(data.score, sumscore, 10);//转换数据类型以通过outtextxy输出
	fillrectangle(700, 100, 800, 130);
	outtextxy(700, 100, sumscore);
}

void MoneyAdd() {//金币更新
	settextstyle(30, 0, _T("微软雅黑"),0,0,FW_BOLD,0,0,0);
	fillrectangle(520, 200, 620, 230);
	outtextxy(520, 200, _T("金币数量："));
	wchar_t summoney[20];
	_itow_s(data.money, summoney, 10);//转换数据类型以通过outtextxy输出
	fillrectangle(700, 200, 800, 230);
	outtextxy(700, 200, summoney);
}


void EnemyInit() {//敌机初始化
	for (int i = 0; i < enemy_num; i++) {
		enemy[i].live = false;
		enemy[i].x = 1000;
		enemy[i].hit = -1;
	}
}

void EnemyCreate(int *flag) {//敌机创建
	if (timer(2500,23)) {//每2.5秒使敌机变换方向生成
		*flag = (*flag == 1) ? 2 : 1;
	}
	for (int i = 0; i < enemy_num; i++) {
		if (enemy[i].x < -50 || enemy[i].x >550 || enemy[i].y > 850) {//回收敌机
			enemy[i].live = false;
			enemy[i].hit = -1;
		}
		if (enemy[i].live == false && timer(300, 3) && enemy[i].hit == -1) {
			enemy[i].live = true;
			enemy[i].type = *flag;
			enemy[i].y = 100;
			enemy[i].hit = 0;
			if (enemy[i].type == 1) {
				enemy[i].x = -20;
				break;
			}
			else if (enemy[i].type == 2) {
				enemy[i].x = 520;
				break;
			}
		}
	}
}


void EnemyMove() {//敌机移动
	for (int i = 0; i < enemy_num; i++) {
		if (enemy[i].live == true) {
			putimage(enemy[i].x, enemy[i].y, &img_enemy_hit_black[0], NOTSRCERASE);
			putimage(enemy[i].x, enemy[i].y, &img_enemy_hit_white[0], SRCINVERT);
			if (enemy[i].type == 1) {
				enemy[i].x += 2;
				if (enemy[i].x > 0) {
					enemy[i].y += 0.1 * sqrt(500 - enemy[i].x);
				}
			}
			else if (enemy[i].type == 2) {
				enemy[i].x -= 2;
				enemy[i].y += 0.1 * sqrt(enemy[i].x);
			}
		}
	}
}

void EnemyHitAnimation() {//敌机受击动画
	for (int i = 0; i < enemy_num; i++) {
		if (enemy[i].live == false && enemy[i].hit == 0) {
			enemy[i].hit = 1;
		}
		if (enemy[i].hit >= 1 && enemy[i].hit <= 6) {
			putimage(enemy[i].x, enemy[i].y, &img_enemy_hit_black[enemy[i].hit], NOTSRCERASE);
			putimage(enemy[i].x, enemy[i].y, &img_enemy_hit_white[enemy[i].hit], SRCINVERT);
			if (timer(10, 25)) {
				enemy[i].hit++;
			}
		}
		if (enemy[i].hit == 7) {
			enemy[i].x -= 500;
			enemy[i].hit = -1;
		}
	}
}

int HitCheck() {//碰撞检测
	for (int i = 0; i < enemy_num; i++) {//敌机与角色碰撞
		if (hero.dotx > enemy[i].x - 17 && hero.dotx < enemy[i].x + 28
			&& hero.doty > enemy[i].y - 17 && hero.doty < enemy[i].y + 31 && timer(5000,4) ) {
			hero.hp--;
			mciSendString(L"close music6", NULL, NULL, NULL);
			mciSendString(L"open Game\\music\\pldead00.wav alias music6", NULL, NULL, NULL);
			mciSendString(L"play music6", NULL, NULL, NULL);
			hero.power -= 1;
			hero.last_level = hero.level;
			if (hero.power <= 0) {
				hero.power = 0;
			}
			if (hero.hp == 0) {
				return 1;
			}
		}
		for(int j = 0; j < bullet_num; j++) {//子弹与敌方碰撞	
			if (bullet[j].x >= enemy[i].x - 20 && bullet[j].x <= enemy[i].x + 28 && bullet[j].y >= enemy[i].y - 20 && bullet[j].y <= enemy[i].y + 31 && bullet[j].y > 0 && bullet[j].y < 800) {
				bullet[j].live = false;
				bullet[j].y = -500;
				enemy[i].live = false;
				data.score += 100;
				data.money++;
				MoneyAdd();
				PropCreate(i);
			}
			//子弹与boss碰撞
			else if (bullet[j].x >= boss.x - 20 && bullet[j].x <= boss.x + 60 && bullet[j].y >= boss.y - 20 && bullet[j].y <= boss.y + 80 && bullet[j].y > 0 && bullet[j].y < 800) {
				boss.hp -= 2;
				bullet[j].live = false;
				bullet[j].y = -500;
			}
		}
		for (int k = 0; k < tracing_bullet_num; k++) {//追踪弹与敌方碰撞
			if (tracingbullet[k].x >= enemy[i].x - 25 && tracingbullet[k].x <= enemy[i].x + 28 
				&& tracingbullet[k].y >= enemy[i].y - 25 && tracingbullet[k].y <= enemy[i].y + 31
				&& tracingbullet[k].y > 0 && tracingbullet[k].y < 800 && tracingbullet[k].x < 530 && tracingbullet[k].x > -30) {
				tracingbullet[k].live = false;
				tracingbullet[k].y = -500;
				enemy[i].live = false;
				data.score += 100;
				data.money++;
				MoneyAdd();
				PropCreate(i);
			}
			else if (tracingbullet[k].x >= boss.x - 25 && tracingbullet[k].x <= boss.x + 75
				&& tracingbullet[k].y >= boss.y - 25 && tracingbullet[k].y <= boss.y + 85
				&& tracingbullet[k].y > 0 && tracingbullet[k].y < 800 && tracingbullet[k].x < 530 && tracingbullet[k].x > -30) {
				tracingbullet[k].live = false;
				tracingbullet[k].y = -500;
				boss.hp -= 5;
			}
		}
		
	}
	for (int k = 0; k < enemy_bullet_num; k++) {//敌方子弹与角色碰撞
		if (hero.dotx < enemybullet[k].x + 18 && hero.dotx > enemybullet[k].x - 17
			&& hero.doty < enemybullet[k].y + 18 && hero.doty > enemybullet[k].y - 17 && timer(5000, 8)) {
			hero.hp--;
			data.graze--;
			data.score -= 1000;
			hero.power -= 1;
			hero.last_level = hero.level;
			mciSendString(L"close music6", NULL, NULL, NULL);//自机受击音效
			mciSendString(L"open Game\\music\\pldead00.wav alias music6", NULL, NULL, NULL);
			mciSendString(L"play music6", NULL, NULL, NULL);
			HittedPropCreate();
			if (hero.power <= 0) {
				hero.power = 0;
			}
			if (hero.hp == 0) {
				return 1;
			}
		}
		else if (enemybullet[k].grazed == 0 && ((enemybullet[k].x >= hero.x - 18 && enemybullet[k].x <= hero.x + 35 &&
			enemybullet[k].y >= hero.y - 18 && enemybullet[k].y <= hero.y - 5) ||
			(enemybullet[k].x >= hero.x - 18 && enemybullet[k].x <= hero.x + 35 &&
				enemybullet[k].y >= hero.y + 30 && enemybullet[k].y <= hero.y + 50) ||
			(enemybullet[k].x >= hero.x - 18 && enemybullet[k].x <= hero.x - 9 &&
				enemybullet[k].y >= hero.y - 5 && enemybullet[k].y <= hero.y + 12) ||
			(enemybullet[k].x >= hero.x + 26 && enemybullet[k].x <= hero.x + 35 &&
				enemybullet[k].y >= hero.y - 5 && enemybullet[k].y <= hero.y + 12))) {//擦弹判定	
			data.graze++;
			enemybullet[k].grazed = 1;
			data.score += 100;
		}
	}
	for (int i = 0; i < boss_bullet_num; i++) {//Boss弹幕与自机碰撞
		if (hero.dotx < bossbullet[i].x + 15 && hero.dotx > bossbullet[i].x - 15
			&& hero.doty < bossbullet[i].y + 15 && hero.doty > bossbullet[i].y - 10 && timer(5000, 43)) {
			hero.hp--;
			data.graze--;
			data.score -= 1000;
			hero.power -= 1;
			hero.last_level = hero.level;
			mciSendString(L"close music6", NULL, NULL, NULL);
			mciSendString(L"open Game\\music\\pldead00.wav alias music6", NULL, NULL, NULL);
			mciSendString(L"play music6", NULL, NULL, NULL);
			HittedPropCreate();
			if (hero.power <= 0) {
				hero.power = 0;
			}
			if (hero.hp == 0) {
				return 1;
			}
		}
		else if (bossbullet[i].grazed == 0 && ((bossbullet[i].x >= hero.x - 18 && bossbullet[i].x <= hero.x + 35 &&
			bossbullet[i].y >= hero.y - 18 && bossbullet[i].y <= hero.y - 5) ||
			(bossbullet[i].x >= hero.x - 18 && bossbullet[i].x <= hero.x + 35 &&
				bossbullet[i].y >= hero.y + 30 && bossbullet[i].y <= hero.y + 50) ||
			(bossbullet[i].x >= hero.x - 18 && bossbullet[i].x <= hero.x - 9 &&
				bossbullet[i].y >= hero.y - 5 && bossbullet[i].y <= hero.y + 12) ||
			(bossbullet[i].x >= hero.x + 26 && bossbullet[i].x <= hero.x + 35 &&
				bossbullet[i].y >= hero.y - 5 && bossbullet[i].y <= hero.y + 12))) {//擦弹判定
			data.graze++;
			bossbullet[i].grazed = 1;
			data.score += 100;
		}
	}
	for (int l = 0; l < prop_num; l++) {//道具进入角色吸收范围
		if (hero.x <= prop[l].x + 25 && hero.x >= prop[l].x - 45 && hero.y <= prop[l].y + 25 && hero.y >= prop[l].y - 55) {
			if (prop[l].img == &img_power_small) {
				hero.power += 0.03;
				hero.last_level = hero.level;
			}
			else if (prop[l].img == &img_power_big) {
				hero.power += 0.2;
				hero.last_level = hero.level;
			}
			else if (prop[l].img == &img_score) {
				data.score += 1000;
			}
			if (hero.power >= 4) {
				hero.power = 4;
			}
			prop[l].live = false;
			prop[l].x -= 500;
		}
	}
	return 0;
}

void GameOver() {
	mciSendString(L"close music1", NULL, NULL, NULL);
	mciSendString(L"close music2", NULL, NULL, NULL);
	mciSendString(L"close music3", NULL, NULL, NULL);
	mciSendString(L"close music4", NULL, NULL, NULL);
	FILE* fp = fopen("Game\\save.txt", "r+");
	if (fp == NULL) {
		perror("fopen");
		return;
	}
	if (data.score > data.maxscore) {
		data.maxscore = data.score;
	}
	fprintf(fp, "%d\n%d", data.money, data.maxscore);
	fclose(fp);
	cleardevice();
	GameInit();
	//FlushBatchDraw();
}

void GameOverBox() {
	int flag = 0;
	while (1) {
		putimage(50, 100, &img_end[flag]);
		FlushBatchDraw();
		if (GetAsyncKeyState(0x26)) {//up 0x26  
			flag = 1;
		}
		else if (GetAsyncKeyState(0x28)) {//down 0x28
			flag = 2;
		}
		if (GetAsyncKeyState(0x5A)) {//Z 0x5A
			if (flag == 1) {
				hero.hp = 3;
				break;
			}
			else if (flag == 2) {
				mciSendString(L"close music2", NULL, NULL, NULL);
				GameOver();
			}
		}
	}
}

void EndGame() {//退出游戏
	exit(1);
}

void EnemyBulletCreate() {//敌机子弹生成
	for (int i = 0; i < enemy_num; i++) {
		if (enemy[i].live) {
			for (int j = 0; j < enemy_bullet_num; j ++) {
				if (enemybullet[j].live == false) {
					enemybullet[j].x = enemy[i].x + 13;
					enemybullet[j].y = enemy[i].y + 10;
					if (enemybullet[j].x < hero.x) {
						enemybullet[j].vx = 1.5;
						enemybullet[j].vy = 1.8 * enemybullet[j].vx;
					}
					else {
						enemybullet[j].vx = -1.5;
						enemybullet[j].vy = 1.8 * enemybullet[j].vx * -1;
					}
					enemybullet[j].grazed = 0;
					enemybullet[j].live = true;
					enemybullet[j].vtype = rand() % 2 + 1;
					break;
				}
			}
		}
	}
}

void EnemyBulletMove() {
	for (int i = 0; i < enemy_bullet_num; i++) {
		if (enemybullet[i].live) {
			putimage(enemybullet[i].x, enemybullet[i].y, &img_enemybulletblack[0], NOTSRCERASE);
			putimage(enemybullet[i].x, enemybullet[i].y, &img_enemybulletwhite[0], SRCINVERT);
			EnemyBulletMoveWays(i);
			if (enemybullet[i].y < -50 || enemybullet[i].y > 850 || enemybullet[i].x < -50 || enemybullet[i].x > 480) {
				enemybullet[i].live = false;
			}
		}
	}
}

void EnemyBulletMoveWays(int i) {
	if (enemybullet[i].vtype == 1) {
		enemybullet[i].y += 3;
	}
	else if (enemybullet[i].vtype == 2) {
		enemybullet[i].x += enemybullet[i].vx;
		enemybullet[i].y += enemybullet[i].vy;
	}
}


void HpUp() {
	if (data.score - data.flag >= 25000) {
		data.flag += 25000;
		if(hero.hp < 10) {
			hero.hp++;
		}
	}
}

HP* HpNodeCreate() {
	HP* p = NULL;
	HP *head = NULL, * end = NULL;
	for (int i = 0; i < hero.hp; i++) {
		p = (HP*)malloc(sizeof(HP));
		if (p != NULL) {
			p->x = 600 + 50 * i;
			p->y = 250 + (i / 4) * 30;
			p->num = i + 1;
			if (head == NULL) {
				head = end = p;
			}
			else {
				end->next = p;
				end = p;
			}
		}
	}
	p->next = NULL;
	return head;
}

void HpShow(HP* head) {
	HP* p = head;
	HP* q = p->next;
	while (q != NULL) {
		putimage(p->x, p->y, &img_hp_black, NOTSRCERASE);
		putimage(p->x, p->y, &img_hp_white, SRCINVERT);
		p = p->next;
		q = p->next;
	}
	putimage(p->x, p->y, &img_hp_black, NOTSRCERASE);
	putimage(p->x, p->y, &img_hp_white, SRCINVERT);
	if (p->num < hero.hp) {
		HpAdd(p);
	}
	else if (p->num > hero.hp) {
		HpMinus(p,head);
	}
	
}

void HpMinus(HP* p,HP*head) {
	HP* q = head;
	while (q->next != p) {
		q = q->next;
	}
	free(p);
	q->next = NULL;
	putimage(500, 0, &img_menu2);
}

void HpAdd(HP* p) {
	HP* q = (HP*)malloc(sizeof(HP));
	q->num = p->num + 1;
	if (q->num <= 5) {
		q->x = 600 + 50 * (q->num - 1);
		q->y = 250;
	}
	if (q->num > 5) {
		q->x = 600 + 50 * (q->num - 6);
		q->y = 280;
	}
	q->next = NULL;
	p->next = q;
}


void PropInit() {
	for (int i = 0; i < prop_num; i++) {
		prop[i].live = false;
		prop[i].vy = 0.5;
		prop[i].vx = 0;
		//prop[i].state = 0;
	}
}

void PropCreate(int j) {
	int flag;
	for (int i = 0; i < prop_num; i++) {
		if (prop[i].x < -50 || prop[i].y > 850) {
			prop[i].live = false;
		}
		if (prop[i].live == false) {
			flag = rand() % 15;
			/*if (flag >= 7) {
				break;
			}*/
			if(flag >= 0 && flag < 15){
				if (flag == 0) {
					prop[i].img = &img_power_big;//大power
				}
				else if (flag >= 1 && flag <= 12) {
					prop[i].img = &img_power_small;//小power
				}
				else if (flag > 12) {
					prop[i].img = &img_score;//score
				}
				prop[i].live = true;
				prop[i].state = 0;
				prop[i].x = enemy[j].x;
				prop[i].y = enemy[j].y;
				prop[i].start_y = prop[i].y;
				prop[i].vy = 0.5;
				break;
			}
		}
	}
}

void PropMove() {
	for (int i = 0; i < prop_num; i++) {
		if (prop[i].live == true) {
			putimage(prop[i].x,prop[i].y,prop[i].img);
			if (prop[i].state == 0) {//0代表上升状态
				prop[i].y -= prop[i].vy;
				//if (timer(10,26)) {
					prop[i].vy -= 0.01;
					prop[i].x += prop[i].vx;
				//}
				if (prop[i].vy <= 0) {
					prop[i].state = 1;
				}
			}
			else if (prop[i].state == 1) {//1代表下降状态
				if (prop[i].vy < 2) {
					prop[i].vy+= 0.2;
				}
				prop[i].vx = 0;
				prop[i].y += prop[i].vy;
			}
			else if (prop[i].state == 2) {//2代表回归状态
				if (prop[i].x < hero.x && prop[i].y < hero.y) {
					prop[i].vy = 10;
					prop[i].x += prop[i].vy;
					prop[i].y += prop[i].vy;
				}
				else if (prop[i].x >= hero.x && prop[i].y < hero.y) {
					prop[i].vy = 10;
					prop[i].x -= prop[i].vy;
					prop[i].y += prop[i].vy;
				}
				else if (prop[i].x < hero.x && prop[i].y >= hero.y) {
					prop[i].vy = 10;
					prop[i].x += prop[i].vy;
					prop[i].y -= prop[i].vy;
				}
				else if (prop[i].x >= hero.x && prop[i].y >= hero.y) {
					prop[i].vy = 10;
					prop[i].x -= prop[i].vy;
					prop[i].y -= prop[i].vy;
				}
			}
		}
	}
}

void PropXyLimit() {//补充限定道具上升范围
	for (int i = 0; i < prop_num; i++) {
		const int y = prop[i].y;
		if (prop[i].state == 0 && prop[i].start_y - prop[i].y >= 50) {
			prop[i].state = 1;
		}
	}
}

void PowerShow() {
	fillrectangle(520, 320, 800, 350);
	outtextxy(520, 320, _T("POWER："));
	outtextxy(700, 320, L"/4.00");
	wchar_t power[20];
	_stprintf(power,L"%.2lf",hero.power);
	outtextxy(650, 320, power);
}

void AllPropsGet() {
	if (hero.y <= 250 && hero.power > 3.99) {
		for (int i = 0; i < prop_num; i++) {
			if (prop[i].x >= 0 && prop[i].x <= 490 && prop[i].y >= 0 && prop[i].y <= 800) {
				prop[i].state = 2;
			}
		}
	}
}

void TracingBulletInit() {
	for (int i = 0; i < tracing_bullet_num; i++) {
		tracingbullet[i].live = false;
		tracingbullet[i].x = -200;
		tracingbullet[i].y = -200;
		tracingbullet[i].vx = 0;
		tracingbullet[i].vy = 0;
		tracingbullet[i].k = 0;
		tracingbullet[i].flag = 0;
		tracingbullet[i].dir = 0;
	}
}

void TracingBulletCreate() {
	for (int i = 0; i < tracing_bullet_num; i++) {//左侧追踪弹
		if (tracingbullet[i].live == false) {
			tracingbullet[i].x = hero.x - 35;
			tracingbullet[i].y = hero.y + 10;
			tracingbullet[i].live = true;
			tracingbullet[i].dir = 1;
			tracingbullet[i].flag = 0;
			tracingbullet[i].vx = 0;
			tracingbullet[i].vy = 0;
			tracingbullet[i].s = 0;
			break;
		}
	}
	for (int i = 0; i < tracing_bullet_num; i++) {//右侧追踪弹
		if (tracingbullet[i].live == false) {
			tracingbullet[i].x = hero.x + 40;
			tracingbullet[i].y = hero.y + 10;
			tracingbullet[i].live = true;
			tracingbullet[i].dir = 2;
			tracingbullet[i].flag = 0;
			tracingbullet[i].vx = 0;
			tracingbullet[i].vy = 0;
			tracingbullet[i].s = 0;
			break;
		}
	}
}

void TracingBulletMove() {
	int data[3];
	for (int i = 0; i < tracing_bullet_num; i++) {
		if (tracingbullet[i].live == true) {
			if (tracingbullet[i].flag == 0) {
				putimage(tracingbullet[i].x, tracingbullet[i].y, &img_tracingbullet_black[0], NOTSRCERASE);
				putimage(tracingbullet[i].x, tracingbullet[i].y, &img_tracingbullet_white[0], SRCINVERT);
				tracingbullet[i].vx += 0.1;
				tracingbullet[i].s += tracingbullet[i].vx;
				tracingbullet[i].y = hero.y + 10;
				if (tracingbullet[i].dir == 1) {
					if (tracingbullet[i].vx < 3) {
						tracingbullet[i].x = hero.x - 35 - tracingbullet[i].s;
					}
					else {
						tracingbullet[i].vx = 0;
						tracingbullet[i].flag = 1;
					}
				}
				else if (tracingbullet[i].dir == 2) {
					if (tracingbullet[i].vx < 3) {
						tracingbullet[i].x = hero.x + 40 + tracingbullet[i].s;
					}
					else {
						tracingbullet[i].vx = 0;
						tracingbullet[i].flag = 1;
					}
				}
			}
			else if (tracingbullet[i].flag == 1) {
				putimage(tracingbullet[i].x, tracingbullet[i].y, &img_tracingbullet_black[0], NOTSRCERASE);
				putimage(tracingbullet[i].x, tracingbullet[i].y, &img_tracingbullet_white[0], SRCINVERT);
				GetMinDistance(data);
				//tracingbullet[i].flag = 2;
				if (tracingbullet[i].vy < 10) {
					tracingbullet[i].vy += 2;
				}
				if (tracingbullet[i].vx < 3) {
					tracingbullet[i].vx += 1.5;
				}
				tracingbullet[i].y -= tracingbullet[i].vy;
				if (boss.live == true) {
					if (tracingbullet[i].x < boss.x) {
						tracingbullet[i].x += tracingbullet[i].vx;
					}
					else if (tracingbullet[i].x > boss.x) {
						tracingbullet[i].x -= tracingbullet[i].vx;
					}
				}
				else {
					if (tracingbullet[i].x < data[0]) {
						tracingbullet[i].x += tracingbullet[i].vx;
					}
					else if (tracingbullet[i].x > data[0]) {
						tracingbullet[i].x -= tracingbullet[i].vx;
					}
				}
				
				//tracingbullet[i].x += tracingbullet[i].vx;
			}
			/*else if (tracingbullet[i].flag == 2) {
				putimage(tracingbullet[i].x, tracingbullet[i].y, &img_tracingbullet_black[0], NOTSRCERASE);
				putimage(tracingbullet[i].x, tracingbullet[i].y, &img_tracingbullet_white[0], SRCINVERT);
				tracingbullet[i].y -= tracingbullet[i].vy;
				tracingbullet[i].x += tracingbullet[i].vx;
			}*/
			if (tracingbullet[i].y < -50 || tracingbullet[i].x > 550 || tracingbullet[i].x < -50) {
				tracingbullet[i].live = false;
				tracingbullet[i].flag = 0;
			}
		}
	}

}

void GetMinDistance(int data[]) {
	for (int i = 0; i < enemy_num; i++) {
		if (enemy[i].live == true && enemy[i].y > 0 && enemy[i].y < 820 && enemy[i].x >= 0 && enemy[i].x < 520) {
			if (enemy[i].y > data[1]) {
				data[0] = enemy[i].x;
				data[1] = enemy[i].y;
				data[2] = i;
			}
		}
	}
}


void HittedPropCreate() {//自机爆p点
	int flag = 0;
	for (int i = 0; i < prop_num; i++) {
		if (prop[i].live == false) {
			prop[i].img = &img_power_big;//大power
			prop[i].live = true;	
			prop[i].vy = 1.5;
			if (flag == 0) {
				prop[i].x = hero.x + 5;
				prop[i].y = hero.y - 70;
				flag = 1;
				prop[i].state = 0;
				prop[i].start_y = prop[i].y;
				continue;
			}
			else if (flag == 1) {
				prop[i].x = hero.x + 45;
				prop[i].y = hero.y - 50;
				prop[i].vx = 0.5;
				flag = 2;
				prop[i].state = 0;
				prop[i].start_y = prop[i].y;
				continue;
			}
			else if (flag == 2) {
				prop[i].x = hero.x - 35;
				prop[i].y = hero.y - 50;
				prop[i].vx = -0.5;
				prop[i].state = 0;
				prop[i].start_y = prop[i].y;
				break;
			}
		}
	}
}


void BossInit() {//Boss初始化
	boss.x = -500;
	boss.y = -500;
	boss.live = false;
	boss.hp = BOSS_HP;
	boss.vx = 0;
	boss.vy = 0;
	boss.dir = 0;
	boss.lives = 3;
	boss.stage = 1;
}

void IfBossCreate(int* timeflag, int* plotflag) {
	stop = clock();
	if (stop - start >= BOSS_TIME) {
		if (*timeflag == -1) {
			*timeflag = 0;
			*plotflag = 1;
		}
		else if (*timeflag == 0) {//只需要生成一次boss
			mciSendString(L"close music2", NULL, NULL, NULL);
			mciSendString(L"open Game\\music\\琪露诺曲repeat.mp3 alias music3", NULL, NULL, NULL);
			mciSendString(L"play music3 repeat", NULL, NULL, NULL);
			*timeflag = 1;
		}
	}
}

void BossCreate() {//Boss生成
	boss.x = 230;
	boss.y = 150;
	boss.live = true;
	boss.img_black = &img_boss_black;
	boss.img_white = &img_boss_white;
}

void BossMove() {//Boss移动
	putimage(boss.x, boss.y, boss.img_black, NOTSRCERASE);
	putimage(boss.x, boss.y, boss.img_white, SRCINVERT);
	BossHpShow();
	if (timer(2000, 36)) {//每2s使BOSS移动状态变化一次
		switch (boss.dir) {
		case 0:
			boss.dir = 1;
			boss.vx = -0.5;
			break;
		case 1:
			boss.dir = 2;
			boss.vx = 0;
			break;
		case 2:
			boss.dir = 3;
			boss.vx = 0.5;
			break;
		case 3:
			boss.dir = 4;
			boss.vx = 0;
			break;
		case 4:
			boss.dir = 5;
			boss.vx = 0.5;
			break;
		case 5:
			boss.dir = 6;
			boss.vx = 0;
			break;
		case 6:
			boss.dir = 7;
			boss.vx = -0.5;
			break;
		case 7:
			boss.dir = 0;
			boss.vx = 0;
			break;
		}
	}
	boss.x += boss.vx;
}

void BossBulletInit() {//BOSS弹幕初始化
	for (int i = 0; i < bossbullet[i].live; i++) {
		bossbullet[i].angel = 0.0;
		bossbullet[i].x = -500;
		bossbullet[i].y = -500;
		bossbullet[i].vx = 0;
		bossbullet[i].vy = 0;
		bossbullet[i].live = false;
		bossbullet[i].vtype = 1;
	}
}

void BossBulletCreate(double* angelflag) {//Boss弹幕生成
	double angleadd1 = 2 * M_PI / 36;//0.17453
	double angleadd2 = 2 * M_PI / 12;
	double angleadd3 = 2 * M_PI / 72;
	//三个变量控制不同阶段子弹发射角度
	int t;
	//t控制BOSS不同阶段攻击速度不同
	if (boss.stage == 1) {
		t = 750;
	}
	else if (boss.stage == 2) {
		t = 500;
	}
	else if (boss.stage == 3) {
		t = 10;
	}
	if (timer(t, 39)) {
		if (boss.stage == 1 || boss.stage == 2) {
			for (int j = 0; j < 36; j++) {
				for (int i = 0; i < boss_bullet_num; i++) {
					if (bossbullet[i].live == false) {
						//bossbullet[i].angel = (*bosscreateflag) * (j * angleadd);//奇奇怪怪的效果，但是好像还不错
						if (boss.stage == 1) {
							bossbullet[i].angel = (*angelflag) + (j * angleadd1);
						}
						else if (boss.stage == 2) {
							bossbullet[i].angel = (*angelflag) + (5 * j * angleadd1);
							*angelflag += 0.2;
						}
						bossbullet[i].A = 6;
						bossbullet[i].vx = bossbullet[i].A * cos(bossbullet[i].angel);
						bossbullet[i].vy = bossbullet[i].A * sin(bossbullet[i].angel);
						bossbullet[i].x = boss.x + 15;
						bossbullet[i].y = boss.y + 15;
						bossbullet[i].live = true;
						bossbullet[i].vtype = 1;
						break;
					}
				}
				if (boss.stage == 2) {
					for (int j = 0; j < 12; j++) {
						for (int i = 0; i < boss_bullet_num; i++) {
							if (bossbullet[i].live == false) {
								bossbullet[i].angel = (j * angleadd2);
								bossbullet[i].A = 3;
								bossbullet[i].vx = bossbullet[i].A * cos(bossbullet[i].angel);
								bossbullet[i].vy = bossbullet[i].A * sin(bossbullet[i].angel);
								bossbullet[i].x = boss.x + 15;
								bossbullet[i].y = boss.y + 15;
								bossbullet[i].live = true;
								bossbullet[i].vtype = 2;
								break;
							}
						}
					}
				}
			}
		}
		if (boss.stage == 3) {
			for (int i = 0; i < boss_bullet_num; i++) {
				if (bossbullet[i].live == false) {
					bossbullet[i].angel = ((*angelflag) * angleadd3);
					bossbullet[i].A = 4;
					bossbullet[i].vx = bossbullet[i].A * cos(bossbullet[i].angel);
					bossbullet[i].vy = bossbullet[i].A * sin(bossbullet[i].angel);
					bossbullet[i].x = boss.x + 15;
					bossbullet[i].y = boss.y + 15;
					bossbullet[i].live = true;
					bossbullet[i].vtype = 2;
					*angelflag += 2;
					break;
				}
			}
		}
	}
}

void BossBulletMove() {//BOOS弹幕移动
	for (int i = 0; i < boss_bullet_num; i++) {
		if (bossbullet[i].x > 500 || bossbullet[i].x < -20 || bossbullet[i].y > 800 || bossbullet[i].y < - 20) {
			bossbullet[i].live = false;
		}
		if (bossbullet[i].live == true) {
			if (boss.stage == 1) {
				putimage(bossbullet[i].x, bossbullet[i].y, &img_enemybulletblack[0], NOTSRCERASE);
				putimage(bossbullet[i].x, bossbullet[i].y, &img_enemybulletwhite[0], SRCINVERT);
			}
			else if (boss.stage == 2) {
				if (bossbullet[i].vtype == 1) {
					putimage(bossbullet[i].x, bossbullet[i].y, &img_enemybulletblack[1], NOTSRCERASE);
					putimage(bossbullet[i].x, bossbullet[i].y, &img_enemybulletwhite[1], SRCINVERT);
				}
				else if (bossbullet[i].vtype == 2) {
					putimage(bossbullet[i].x, bossbullet[i].y, &img_enemybulletblack[2], NOTSRCERASE);
					putimage(bossbullet[i].x, bossbullet[i].y, &img_enemybulletwhite[2], SRCINVERT);
				}
			}
			else if (boss.stage == 3) {
				putimage(bossbullet[i].x, bossbullet[i].y, &img_enemybulletblack[0], NOTSRCERASE);
				putimage(bossbullet[i].x, bossbullet[i].y, &img_enemybulletwhite[0], SRCINVERT);
			}
			bossbullet[i].x += bossbullet[i].vx;
			bossbullet[i].y += bossbullet[i].vy;
			if (bossbullet[i].A > 1.5) {
				bossbullet[i].A -= 0.05;
				bossbullet[i].vx = bossbullet[i].A * cos(bossbullet[i].angel);
				bossbullet[i].vy = bossbullet[i].A * sin(bossbullet[i].angel);
			}
		}	
	}
}

void BossHpShow() {//Boss血条显示
	if (boss.hp > 0) {
		setlinecolor(RGB(155,155,155));
		setlinestyle(PS_SOLID, 5);
		rectangle(95, 5, 450, 15);
		setlinecolor(RED);
		setlinestyle(PS_SOLID, 7);
		line(100,10,100+(boss.hp/BOSS_HP)*350,10);		
	}
}

void BossDeath(int* plotflag) {//Boss死亡
	if (boss.hp <= 0 && boss.lives > 1) {
		boss.hp = BOSS_HP;
		boss.lives--;
		boss.stage++;
		data.score += 10000;
		data.money += 100;
		ScoreAdd();
		MoneyAdd();
		if (boss.stage == 2) {
			*plotflag = 2;
		}
		else if (boss.stage == 3) {
			*plotflag = 3;
		}
	}
	else if (boss.hp <= 0 && boss.lives == 1) {
		boss.lives = 0;
		boss.stage = 4;
		*plotflag = 4;
	}
}

void GamePlot(int* plotflag,int num) {//游戏剧情
	int flag = 0;
	while (1) {
		MapMove(num);
		putimage(0, 70, &img_plot[0], NOTSRCERASE);
		putimage(0, 70, &img_plot[1], SRCINVERT);
		putimage(150, 50, &img_talkframe);
		putimage(348, 600, &img_plot[2], NOTSRCERASE);
		putimage(348, 600, &img_plot[3], SRCINVERT);
		putimage(40, 570, &img_talkframe);
		setbkmode(BLACK);
		settextstyle(30, 0, _T("华文新魏"));
		settextcolor(BLACK);
		if (*plotflag == 1) {
			if (flag == 0) {
				outtextxy(50, 580, _T("诶，这里是哪？"));
				outtextxy(50, 620, _T("难不成我是路痴？"));
			}
			else if (flag == 1) {
				outtextxy(50, 580, _T("诶，这里是哪？"));
				outtextxy(50, 620, _T("难不成我是路痴？"));
				outtextxy(160, 60, _T("如果迷路，"));
				outtextxy(160, 100, _T("定是妖精所为"));
			}
			else if (flag == 2) {
				outtextxy(50, 580, _T("啊啦是吗？"));
				outtextxy(50, 620, _T("那么，带个路吧？"));
				outtextxy(50, 660, _T("这附近有岛对不对？"));
				outtextxy(160, 60, _T("如果迷路，"));
				outtextxy(160, 100, _T("定是妖精所为"));
			}
			else if (flag == 3) {
				outtextxy(50, 580, _T("啊啦是吗？"));
				outtextxy(50, 620, _T("那么，带个路吧？"));
				outtextxy(50, 660, _T("这附近有岛对不对？"));
				outtextxy(160, 60, _T("你啊 可别吓着了喔，"));
				outtextxy(160, 100, _T("在你面前，"));
				outtextxy(160, 140, _T("可是有个强敌呢。"));
			}
			else if (flag == 4) {
				outtextxy(50, 580, _T("靶子？"));
				outtextxy(50, 620, _T("这还真是令人吃惊啊"));
				outtextxy(160, 60, _T("你啊 可别吓着了喔，"));
				outtextxy(160, 100, _T("在你面前，"));
				outtextxy(160, 140, _T("可是有个强敌呢。"));
			}
			else if (flag == 5) {
				outtextxy(50, 580, _T("靶子？"));
				outtextxy(50, 620, _T("这还真是令人吃惊啊"));
				outtextxy(160, 60, _T("开什么玩笑啊～"));
				outtextxy(160, 100, _T("像你这样的人，")); 
				outtextxy(160, 140, _T("就和英吉利牛肉一起"));
				outtextxy(160, 180, _T("冰冻冷藏起来吧！！"));
			}
			else if (flag >= 6) {
				*plotflag = -1;
				flag = 0;
				break;
			}
			if (GetAsyncKeyState(0x58) && timer(500, 43)) {// x 0x58
				flag++;
			}
		}
		else if (*plotflag == 2) {
			if (flag == 0) {
				outtextxy(160, 60, _T("可恶，还没完！"));
				outtextxy(160, 100, _T("让你见识一下"));
				outtextxy(160, 140, _T("我真正的力量"));
			}
			else if (flag == 1) {
				outtextxy(50, 580, _T("唉...真是麻烦"));
				outtextxy(50, 620, _T("放马过来吧！"));
				outtextxy(160, 60, _T("可恶，还没完！"));
				outtextxy(160, 100, _T("让你见识一下"));
				outtextxy(160, 140, _T("我真正的力量"));
			}
			else if (flag >= 2) {
				*plotflag = -1;
				flag = 0;
				break;
			}
			if (GetAsyncKeyState(0x58) && timer(500, 44)) {// x 0x58
				flag++;
			}
		}
		else if (*plotflag == 3) {
			if (flag == 0) {
				outtextxy(160, 60, _T("怎么会这样？"));
				outtextxy(160, 100, _T("我琪露诺"));
				outtextxy(160, 140, _T("可是最强的！"));
			}
			else if (flag >= 1) {
				*plotflag = -1;
				flag = 0;
				break;
			}
			if (GetAsyncKeyState(0x58) && timer(500, 45)) {// x 0x58 
				flag++;
			}
		}
		else if (*plotflag == 4) {
			if (flag == 0) {
				outtextxy(50, 580, _T("啊啊，越来越冷了啊"));
				outtextxy(50, 620, _T("这样会得空调病的啊"));
			}
			else if (flag >= 1) {
				*plotflag = -1;
				flag = 0;
				while (1) {
					putimage(50, 100, &img_gameover);
					FlushBatchDraw();
					if (GetAsyncKeyState(0x5A) && timer(500, 47)) {	//z 0x5A
						GameOver();
					}
				}
				break;
			}
			if (GetAsyncKeyState(0x58) && timer(500, 46)) {// x 0x58
				flag++;
			}
		}
		else if (*plotflag == 5) {
			GameOver();
		}
		FlushBatchDraw();
	}
}	

void MapMove(int num) {
	putimage(0, map_y, &img_maps[num]);
	putimage(0, map_y - 800, &img_maps[num]);
	map_y += 1;//背景移动
	if (map_y == 800) {
		map_y = 0;
	}
	Sleep(10);
}

//23024117 阎思远
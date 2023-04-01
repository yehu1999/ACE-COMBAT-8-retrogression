#include<iostream>
#include<stdio.h>
#include<graphics.h> 
#include<conio.h>
#include<math.h>
#include<time.h>
using namespace std;

//定时器函数
bool timer(int ms, int id)
{
	static DWORD t[10];//定义10个空闲定时器待用，使用id来取用
	if (clock() - t[id] > ms)
	{
		t[id] = clock();
		return true;
	}
	return false;
}
//碰撞检测函数
bool crash(int x, int y, int x1, int y1, int x2, int y2)//检测点(x,y)和矩形是否碰撞，其中(x1,y1)(x2,y2)表示矩形左上角和右下角
{
	if (x > x1 && x<x2 && y>y1 && y < y2)
	{
		return true;
	}
	return false;
}



//枚举组
enum my
{
	WIDTH = 480,
	HEIGHT = 700,
	WIDTH_player = 102,
	HEIGHT_player = 126,
	BULLET_number = 25,
	ENEMY_number = 25,
	ENEMY1, ENEMY2, ENEMY3,
	ENEMY_STEP = 100,
	TOOL_NUM = 3,
	TOOL_cure, TOOL_shield, TOOL_bigbullet,
	TOOL_STEP = 100,
};

//飞机结构
struct plance
{
	int x, y;
	bool live;
	int bigbullet;
	int width;
	int height;
	int hp;
	int shield_hp;
	int type;
	int step;//步幅，用于解决敌人水平方向随机移动摇摆不定问题
	int score;
	int gamelevel;
}player, bullet[BULLET_number], enemy[ENEMY_number], tool[TOOL_NUM], BigBullet;




//导入图片
IMAGE bk;
IMAGE img_palyer[2];
IMAGE img_bullet_player[2];
IMAGE img_enemy[3][2];
IMAGE img_player_ouch[2];
IMAGE img_player_dying[2];
IMAGE img_gameover;
IMAGE img_cure; IMAGE img_shield; IMAGE img_bigbullet;
IMAGE img_BigBullet;

//图片加载函数
void LoadImage()
{
	loadimage(&bk, "pictures//background.png");

	loadimage(&img_palyer[0], "pictures//player1_yan.png");
	loadimage(&img_palyer[1], "pictures//player1_yuan.png");

	loadimage(&img_bullet_player[0], "pictures//bullet_yan.png");
	loadimage(&img_bullet_player[1], "pictures//bullet_player_yuan.png");

	loadimage(&img_enemy[0][0], "pictures//enemy1_yan.png");
	loadimage(&img_enemy[0][1], "pictures//enemy1_yuan.png");
	loadimage(&img_enemy[1][0], "pictures//enemy2_yan.png");
	loadimage(&img_enemy[1][1], "pictures//enemy2_yuan.png");
	loadimage(&img_enemy[2][0], "pictures//enemy3_yan.png");
	loadimage(&img_enemy[2][1], "pictures//enemy3_yuan.png");

	loadimage(&img_player_ouch[0], "pictures//player1_yan.png");
	loadimage(&img_player_ouch[1], "pictures//player1_ouch_yuan.png");
	loadimage(&img_player_dying[0], "pictures//player1_yan.png");
	loadimage(&img_player_dying[1], "pictures//player1_dying_yuan.png");

	loadimage(&img_gameover, "pictures//gameover.png");

	loadimage(&img_cure, "pictures//cure.png");
	loadimage(&img_bigbullet, "pictures//bigbullet.png");
	loadimage(&img_shield, "pictures//shield.png");

	loadimage(&img_BigBullet, "pictures//bigbulletDo.png");
}




//敌人初始化函数
void enemyHp(int i)
{
	int randnum = rand() % 100;
	if (randnum <= 5)
	{
		enemy[i].type = ENEMY3;
		enemy[i].hp = 12;
		enemy[i].width = 169;
		enemy[i].height = 258;
		enemy[i].step = rand() % ENEMY_STEP;
	}
	else if (randnum >= 6 && randnum <= 40)
	{
		enemy[i].type = ENEMY2;
		enemy[i].hp = 6;
		enemy[i].width = 69;
		enemy[i].height = 99;
		enemy[i].step = rand() % ENEMY_STEP;
	}
	else
	{
		enemy[i].type = ENEMY1;
		enemy[i].hp = 2;
		enemy[i].width = 57;
		enemy[i].height = 43;
		enemy[i].step = rand() % ENEMY_STEP;
	}
}

//道具初始化函数
void TOOLHP(int i)
{
	int randnum = rand() % 100;
	if (randnum <= 33)
	{
		tool[i].type = TOOL_cure;
		tool[i].hp = 1;
		tool[i].width = 50;
		tool[i].height = 50;
		tool[i].step = rand() % TOOL_STEP;
	}
	else if (randnum >= 33 && randnum <= 66)
	{
		tool[i].type = TOOL_shield;
		tool[i].hp = 2;
		tool[i].width = 50;
		tool[i].height = 50;
		tool[i].step = rand() % TOOL_STEP;
	}
	else
	{
		tool[i].type = TOOL_bigbullet;
		tool[i].hp = 3;
		tool[i].width = 50;
		tool[i].height = 50;
		tool[i].step = rand() % TOOL_STEP;
	}
}

//初始化数据函数
void gameInit()
{
	//初始化玩家
	LoadImage();
	player.x = WIDTH / 2 - WIDTH_player / 2;
	player.y = HEIGHT - HEIGHT_player;
	player.live = true;
	player.hp = 100;
	player.shield_hp = 0;
	player.bigbullet = 0;
	player.score = 0;
	player.gamelevel = 0;
	//初始化大子弹
	BigBullet.live = false;
	BigBullet.x = 0;
	BigBullet.y = 0;
	//初始化子弹
	for (int i = 0; i < BULLET_number; i++)
	{
		bullet[i].x = 0;
		bullet[i].y = 0;
		bullet[i].live = false;
	}

	//初始化敌人
	for (int i = 0; i < ENEMY_number; i++)
	{
		enemy[i].live = false;
		enemyHp(i);
	}

	//初始化道具
	for (int i = 0; i < TOOL_NUM; i++)
	{
		tool[i].live = false;
		TOOLHP(i);
	}
}




//敌人生成
void creatEnemy()
{
	for (int i = 0; i < ENEMY_number; i++)
	{
		if (!enemy[i].live)
		{
			enemy[i].live = true;
			enemy[i].x = rand() % WIDTH;
			enemy[i].y = -100;
			enemyHp(i);
			break;
		}
	}
}

//子弹生成函数
void creatBullet()
{
	for (int i = 0; i < BULLET_number; i++)
	{
		if (!bullet[i].live)
		{
			bullet[i].x = player.x + WIDTH_player / 2;
			bullet[i].y = player.y;
			bullet[i].live = true;
			break;
		}
	}
}

//大子弹生成函数
void createBigBullet(int i)
{
	if (!BigBullet.live)
	{
		BigBullet.live = true;
		BigBullet.x = player.x + player.width / 2 + 18;
		BigBullet.y = player.y - 1000;
	}
}

//道具生成函数
void creatTools()
{
	for (int i = 0; i < TOOL_NUM; i++)
	{
		if (!tool[i].live)
		{
			tool[i].live = true;
			tool[i].x = rand() % WIDTH;
			tool[i].y = 100 + rand() % (HEIGHT - 100);
			TOOLHP(i);
			break;
		}
	}
}




//绘制函数//游戏出口暂时在这
void gameDraw()
{
	//绘制背景
	if (player.live == true)
	{
		putimage(0, 0, &bk);
	}

	//gameover
	if (player.live == false)
	{
		putimage(0, 0, &img_gameover);
		outtextxy(WIDTH / 2, HEIGHT / 2 - 100, "按return结束");
		if (GetAsyncKeyState(VK_RETURN))
		{
			closegraph();//游戏出口
		}
	}

	//绘制玩家
	if (player.hp >= 70)
	{
		putimage(player.x, player.y, &img_palyer[0], NOTSRCERASE);
		putimage(player.x, player.y, &img_palyer[1], SRCINVERT);
	}
	if (player.hp < 70 && player.hp >= 40)
	{
		putimage(player.x, player.y, &img_player_ouch[0], NOTSRCERASE);
		putimage(player.x, player.y, &img_player_ouch[1], SRCINVERT);
	}
	if (player.hp > 0 && player.hp < 40)
	{
		putimage(player.x, player.y, &img_player_dying[0], NOTSRCERASE);
		putimage(player.x, player.y, &img_player_dying[1], SRCINVERT);
	}

	//绘制UI
	//score
	int score = player.score;
	char str[10];
	sprintf(str, "%d", score);
	setbkmode(TRANSPARENT);
	settextcolor(WHITE);
	settextstyle(50, 0, "楷体");
	outtextxy(15, 0, str);
	//hp
	int hp = player.hp;
	char strhp[10];
	sprintf(strhp, "%d", hp);
	setbkmode(TRANSPARENT);
	settextcolor(RED);
	settextstyle(25, 0, "楷体");
	outtextxy(WIDTH - 45, 0, strhp);
	//shield_hp
	int shihp = player.shield_hp;
	char strshihp[10];
	sprintf(strshihp, "%d", player.shield_hp);
	setbkmode(TRANSPARENT);
	settextcolor(LIGHTGRAY);
	settextstyle(25, 0, "楷体");
	outtextxy(WIDTH - 45, 25, strshihp);
	//BigBullet
	int Bhp = player.bigbullet;
	char strBhp[10];
	sprintf(strBhp, "%d", Bhp);
	setbkmode(TRANSPARENT);
	settextcolor(YELLOW);
	settextstyle(25, 0, "楷体");
	outtextxy(WIDTH - 45, 50, strBhp);
	//gamelevel
	int gamelevel = player.gamelevel;
	char strgamelevel[10];
	sprintf(strgamelevel, "%d", gamelevel);
	setbkmode(TRANSPARENT);
	settextcolor(BLACK);
	settextstyle(35, 0, "楷体");
	outtextxy(WIDTH / 2 - 2, 0, strgamelevel);

	//绘制子弹
	for (int i = 0; i < BULLET_number; i++)
	{
		if (bullet[i].live)
		{
			putimage(bullet[i].x, bullet[i].y, &img_bullet_player[0], NOTSRCERASE);
			putimage(bullet[i].x, bullet[i].y, &img_bullet_player[1], SRCINVERT);
		}
	}
	//绘制敌人
	for (int i = 0; i < ENEMY_number; i++)
	{
		if (enemy[i].live)
		{
			if (enemy[i].type == ENEMY1)
			{
				putimage(enemy[i].x, enemy[i].y, &img_enemy[0][0], NOTSRCERASE);
				putimage(enemy[i].x, enemy[i].y, &img_enemy[0][1], SRCINVERT);
			}
			else if (enemy[i].type == ENEMY2)
			{
				putimage(enemy[i].x, enemy[i].y, &img_enemy[1][0], NOTSRCERASE);
				putimage(enemy[i].x, enemy[i].y, &img_enemy[1][1], SRCINVERT);
			}
			else if (enemy[i].type == ENEMY3)
			{
				putimage(enemy[i].x, enemy[i].y, &img_enemy[2][0], NOTSRCERASE);
				putimage(enemy[i].x, enemy[i].y, &img_enemy[2][1], SRCINVERT);
			}
		}
	}
	//绘制大子弹
	if (BigBullet.live)
		putimage(BigBullet.x, BigBullet.y, &img_BigBullet);
	if (BigBullet.live && timer(500, 5))
		BigBullet.live = false;

	//绘制道具
	for (int i = 0; i < TOOL_NUM; i++)
	{
		if (tool[i].live)
		{
			if (tool[i].type == TOOL_cure)
			{
				putimage(tool[i].x, tool[i].y, &img_cure);

			}
			else if (tool[i].type == TOOL_shield)
			{
				putimage(tool[i].x, tool[i].y, &img_shield);

			}
			else if (tool[i].type == TOOL_bigbullet)
			{
				putimage(tool[i].x, tool[i].y, &img_bigbullet);
			}
		}
	}
}



//医疗包效果
void cure(int i)
{
	if (player.hp + 50 <= 100)
		player.hp += 50;
	if (player.hp + 50 > 100)
		player.hp = 100;
}
//护盾效果
void shield(int hurt)
{
	if (player.shield_hp > 0)
		if (player.shield_hp - hurt >= 0)
			player.shield_hp -= hurt;
		else if (player.shield_hp - hurt < 0)
			player.shield_hp = 0;
	if (player.shield_hp == 0)
		player.hp -= hurt;
}
//大子弹效果
void bigbullet(int i)
{
	if (i == 0)
	{
		createBigBullet(1);

		for (int i = 0; i < ENEMY_number; i++)
		{
			if (!enemy[i].live)
			{
				continue;
			}
			if (enemy[i].live && crash(enemy[i].x + enemy[i].width / 2, enemy[i].y + enemy[i].height / 2, BigBullet.x, BigBullet.y, BigBullet.x + 70, BigBullet.y + 1000))
				enemy[i].hp -= 100;
		}
		player.bigbullet -= 1;
		player.score += 1;
	}
}




//敌人移动函数
void EnemyMove(int speed)
{

	for (int i = 0; i < ENEMY_number; i++)
	{
		if (enemy[i].live)
		{
			if (enemy[i].y < HEIGHT)
			{
				enemy[i].y += speed;
			}
			else
			{
				enemy[i].live = false;
			}
			if (enemy[i].x > 0 && enemy[i].x < WIDTH - enemy[i].width)
			{
				if (enemy[i].step < ENEMY_STEP / 2)
					enemy[i].x += 1;
				else
					enemy[i].x -= 1;
			}
			else if (enemy[i].x <= 0)
			{
				enemy[i].x += 1;
				enemy[i].step + 1;
			}
			else if (enemy[i].x >= WIDTH - enemy[i].width)
			{
				enemy[i].x -= 1;
				enemy[i].step + 1;
			}

			if (enemy[i].step < ENEMY_STEP)
				enemy[i].step++;
			if (enemy[i].step == ENEMY_STEP)
				enemy[i].step = 0;
		}
	}
}

//子弹移动函数
void BulletMove(int speed)
{
	for (int i = 0; i < BULLET_number; i++)
	{
		if (bullet[i].live)
		{

			bullet[i].y -= speed;
			if (bullet[i].y < 0)
			{
				bullet[i].live = false;
			}
		}
	}
}

//角色移动/攻击函数
void playerMove(int speed)
{
#if 0
	//检测按键是否按下
	if (_kbhit())
	{
		//1_getch()阻塞函数(<conio.h>为非标准库，移植性差）（百度）：getch()是编程中所用的函数，这个函数是一个不回显函数，当用户按下某个字符时，函数自动读取，无需按回车，有的C语言命令行程序会用到此函数做游戏，但是这个函数并非标准函数，要注意移植性！
		char key = _getch();
		switch (key)
		{
		case 'W':
		case 'w':
			player.y -= speed;
			break;
		case 'S':
		case 's':
			player.y += speed;
			break;
		case 'A':
		case 'a':
			player.x -= speed;
			break;
		case 'D':
		case 'd':
			player.x += speed;
			break;
		}
	}

#elif 1
	//2使用windows函数获取键盘输入(非阻塞函数）(检测字母按键必须用大写）
	if (player.live == true)
	{
		if (GetAsyncKeyState(VK_UP) || GetAsyncKeyState('W'))
		{
			if (player.y > 0)//边界限制判断和刷新率限制
			{
				player.y -= speed;
			}
		}
		if (GetAsyncKeyState(VK_DOWN) || GetAsyncKeyState('S'))
		{
			if (player.y < HEIGHT - HEIGHT_player / 2)//边界限制判断
			{
				player.y += speed;
			}
		}
		if (GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState('A'))
		{
			if (player.x > 0 - WIDTH_player / 2)//边界限制判断
			{
				player.x -= speed;
			}
		}
		if (GetAsyncKeyState(VK_RIGHT) || GetAsyncKeyState('D'))
		{
			if (player.x < WIDTH - WIDTH_player / 2)//边界限制判断
			{
				player.x += speed;
			}
		}
	}

#endif
	if (player.live == true && GetAsyncKeyState(VK_SPACE) && timer(150, 0))
	{
		creatBullet();
	}
	if (player.live == true && GetAsyncKeyState(VK_NUMPAD1) && timer(500, 0)&&player.bigbullet > 0)
		bigbullet(0);
}



//角色碰撞//吃包括道具
void PlayerKO()
{
	//敌人
	for (int i = 0; i < ENEMY_number; i++)
	{
		//碰撞点
		if (player.live == true && enemy[i].live == true)
		{   //机头（修正）
			if (crash(player.x + WIDTH_player / 2, player.y + 3, enemy[i].x + 5, enemy[i].y + 5, enemy[i].x + enemy[i].width - 5, enemy[i].y + enemy[i].height - 5))
			{
				enemy[i].live = false;
				if (enemy[i].type == ENEMY3)
					shield(50);
				if (enemy[i].type == ENEMY2)
					shield(30);
				if (enemy[i].type == ENEMY1)
					shield(10);
			}
			//左下角（修正）
			else if (crash(player.x + 3, player.y + HEIGHT_player - 30, enemy[i].x + 5, enemy[i].y + 5, enemy[i].x + enemy[i].width - 5, enemy[i].y + enemy[i].height - 5))
			{
				enemy[i].live = false;
				if (enemy[i].type == ENEMY3)
					shield(40);
				if (enemy[i].type == ENEMY2)
					shield(20);
				if (enemy[i].type == ENEMY1)
					shield(5);
			}
			//右下角（修正）
			else if (crash(player.x + WIDTH_player - 3, player.y + HEIGHT_player - 30, enemy[i].x + 5, enemy[i].y + 5, enemy[i].x + enemy[i].width - 5, enemy[i].y + enemy[i].height - 5))
			{
				enemy[i].live = false;
				if (enemy[i].type == ENEMY3)
					shield(40);
				if (enemy[i].type == ENEMY2)
					shield(20);
				if (enemy[i].type == ENEMY1)
					shield(5);
			}
			//左腰
			else if (crash(player.x + 30, player.y + 54, enemy[i].x + 5, enemy[i].y + 5, enemy[i].x + enemy[i].width - 5, enemy[i].y + enemy[i].height - 5))
			{
				enemy[i].live = false;
				if (enemy[i].type == ENEMY3)
					shield(40);
				if (enemy[i].type == ENEMY2)
					shield(20);
				if (enemy[i].type == ENEMY1)
					shield(5);
			}
			//右腰
			else if (crash(player.x + WIDTH_player - 30, player.y + 54, enemy[i].x + 5, enemy[i].y + 5, enemy[i].x + enemy[i].width - 5, enemy[i].y + enemy[i].height - 5))
			{
				enemy[i].live = false;
				if (enemy[i].type == ENEMY3)
					shield(40);
				if (enemy[i].type == ENEMY2)
					shield(20);
				if (enemy[i].type == ENEMY1)
					shield(5);
			}
			//屁股
			else if (crash(player.x + WIDTH_player / 2, player.y + HEIGHT_player - 30, enemy[i].x + 5, enemy[i].y + 5, enemy[i].x + enemy[i].width - 5, enemy[i].y + enemy[i].height - 5))
			{
				enemy[i].live = false;
				if (enemy[i].type == ENEMY3)
					shield(50);
				if (enemy[i].type == ENEMY2)
					shield(30);
				if (enemy[i].type == ENEMY1)
					shield(10);
			}
		}
		//死亡
		if (player.hp <= 0)
		{
			player.live = false;
		}
		if (player.live == false)
		{
			player.x = 100000;
			player.y = 100000;
		}
	}

	//道具
	for (int i = 0; i < TOOL_NUM; i++)
	{
		//碰撞点
		if (player.live == true && tool[i].live == true)
		{   //机头（修正）
			if (crash(player.x + WIDTH_player / 2, player.y + 3, tool[i].x + 5, tool[i].y + 5, tool[i].x + tool[i].width - 5, tool[i].y + tool[i].height - 5))
			{
				tool[i].live = false;
				if (tool[i].type == TOOL_cure)
					cure(i);
				if (tool[i].type == TOOL_shield)
					player.shield_hp = 100;
				if (tool[i].type == TOOL_bigbullet)
					player.bigbullet += 3;
			}
			//左下角（修正）
			else if (crash(player.x + 3, player.y + HEIGHT_player - 30, tool[i].x + 5, tool[i].y + 5, tool[i].x + tool[i].width - 5, tool[i].y + tool[i].height - 5))
			{
				tool[i].live = false;
				if (tool[i].type == TOOL_cure)
					cure(i);
				if (tool[i].type == TOOL_shield)
					player.shield_hp = 100;
				if (tool[i].type == TOOL_bigbullet)
					player.bigbullet += 3;
			}
			//右下角（修正）
			else if (crash(player.x + WIDTH_player - 3, player.y + HEIGHT_player - 30, tool[i].x + 5, tool[i].y + 5, tool[i].x + tool[i].width - 5, tool[i].y + tool[i].height - 5))
			{
				tool[i].live = false;
				if (tool[i].type == TOOL_cure)
					cure(i);
				if (tool[i].type == TOOL_shield)
					player.shield_hp = 100;
				if (tool[i].type == TOOL_bigbullet)
					player.bigbullet += 3;
			}
			//左腰
			else if (crash(player.x + 30, player.y + 54, tool[i].x + 5, tool[i].y + 5, tool[i].x + tool[i].width - 5, tool[i].y + tool[i].height - 5))
			{
				tool[i].live = false;
				if (tool[i].type == TOOL_cure)
					cure(i);
				if (tool[i].type == TOOL_shield)
					player.shield_hp = 100;
				if (tool[i].type == TOOL_bigbullet)
					player.bigbullet += 3;
			}
			//右腰
			else if (crash(player.x + WIDTH_player - 30, player.y + 54, tool[i].x + 5, tool[i].y + 5, tool[i].x + tool[i].width - 5, tool[i].y + tool[i].height - 5))
			{
				tool[i].live = false;
				if (tool[i].type == TOOL_cure)
					cure(i);
				if (tool[i].type == TOOL_shield)
					player.shield_hp = 100;
				if (tool[i].type == TOOL_bigbullet)
					player.bigbullet += 3;
			}
			//屁股
			else if (crash(player.x + WIDTH_player / 2, player.y + HEIGHT_player - 30, tool[i].x + 5, tool[i].y + 5, tool[i].x + tool[i].width - 5, tool[i].y + tool[i].height - 5))
			{
				tool[i].live = false;
				if (tool[i].type == TOOL_cure)
					cure(i);
				if (tool[i].type == TOOL_shield)
					player.shield_hp = 100;
				if (tool[i].type == TOOL_bigbullet)
					player.bigbullet += 3;
			}
		}
	}
}




//find板块（粘贴到这）
void find()
{

}




//敌人击毁
void enemyko()
{
	for (int i = 0; i < ENEMY_number; i++)
	{
		if (!enemy[i].live)
		{
			continue;
		}
		for (int k = 0; k < BULLET_number; k++)
		{
			if (!bullet[k].live)
			{
				continue;
			}
			if (bullet[k].x > enemy[i].x && bullet[k].x < enemy[i].x + enemy[i].width
				&& bullet[k].y>enemy[i].y && bullet[k].y < enemy[i].y + enemy[i].height)
			{
				enemy[i].hp -= 1;
				bullet[k].live = false;
				player.score += 1;
			}
		}
		if (enemy[i].hp <= 0)
		{
			enemy[i].live = false;
		}
	}
}




//分数级与游戏难度
void gamelevel()
{
	if (player.live == true)
	{
		if (player.score <= 60)
		{
			player.gamelevel = 0;
			if (timer(3000, 1))
			{
				creatEnemy();
			}
			EnemyMove(1);
		}
		else if (player.score > 60 && player.score <= 120)
		{
			player.gamelevel = 1;
			if (timer(2000, 1))
			{
				creatEnemy();
			}
			EnemyMove(1);
		}
		else if (player.score > 120 && player.score <= 240)
		{
			player.gamelevel = 2;
			if (player.score == 125 || player.score == 225)
			{
				creatTools();
			}
			if (timer(800, 1))
			{
				creatEnemy();
			}
			EnemyMove(2);
		}
		else if (player.score > 240 && player.score <= 480)
		{
			player.gamelevel = 3;
			if (player.score == 250 || player.score == 300 || player.score == 400)
			{
				creatTools();
			}
			if (timer(500, 1))
			{
				creatEnemy();
			}
			EnemyMove(3);
		}
		else if (player.score > 480 && player.score <= 999)
		{
			player.gamelevel = 4;
			if (player.score == 450 || player.score == 500 || player.score == 600 || player.score == 666 || player.score == 700 || player.score == 800 || player.score == 900 || player.score == 999)
			{
				creatTools();
			}
			if (timer(200, 1))
			{
				creatEnemy();
			}
			EnemyMove(4);
		}
	}
}




//数据可视化函数(DEBUG)
void showdata()
{
	cout << player.x << "," << player.y << "  ";
	cout << player.hp << "  " << player.score <<"  "<< player.gamelevel << endl;
	for (int i = 0; i < ENEMY_number; i++)
	{
		//printf("ENEMY%d(%d,%d),HP=%d\n", i, enemy[i].x, enemy[i].y, enemy[i].hp);
		;
	}
	printf("%d", BigBullet.live);
}




//游戏主体
int main()
{
	initgraph(WIDTH, HEIGHT);//打包时删去showconsole，关闭控制台
	gameInit();
	//双缓冲绘图(B-F-E)(不然画面会一卡一卡的）
	BeginBatchDraw();
	while (1)
	{
		gameDraw();
		FlushBatchDraw();
		enemyko();
		gamelevel();
		playerMove(10);
		BulletMove(45);
		PlayerKO();
		showdata();
		Sleep(10);//控制刷新频率		
	}
	EndBatchDraw();

	return 0;
}



//游戏：ACE COMBAT 8 _ retrogression
//作者：Ajga
//2022.11.21（1.0.1）
//2022.12.11（1.0.2）
//2022.12.15（1.0.3）
//2022.12.21（1.0.4）
//2023.02.15（1.0.5）
//版本：1.0.5.1
//2023.2.16

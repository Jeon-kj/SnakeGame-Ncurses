#include<ncurses.h>
#include<iostream>
#include<locale.h>
#include<vector>
#include <unistd.h>
#include <string>
#include <cstring>
#include <cstdlib>
#include <time.h>

using namespace std;

#define map_rows 23
#define map_cols 23
#define Wall 1
#define IWall 2
#define SHead 3
#define SBody 4
#define GItem 5
#define PItem 6
#define Gate 7

string choices[] = { 
			"Game Start",
			"Game Manual",
			"Producers",
			"Exit",
		  };
int snake_length;
int max_snake_length = 20;
int get_GItem;
int get_PItem;
int get_Gate;
int map[map_rows][map_cols]={0};
int ms_snake_length;
int ms_GItem;
int ms_PItem;
int ms_Gate;
int pass_gate = 0;

int tick = 0;		//뱀의 움직임과 아이템 생성에 영향을 미치는 틱
int Gtick = 0;		//게이트 생성에 영향을 미치는 틱

bool game_running = true;
bool menu_running = true;
int stage = 1;

WINDOW *GameMap;
WINDOW *GameScore;
WINDOW *GameMission;
WINDOW *GameEnd;
WINDOW *MenuWin;
WINDOW *SubMenu;

class Snake {		//뱀의 길이가 늘어날 때 필요한 클래스.
public:
	int body_no;
	Snake *frontBody;
	Snake *backBody;
	pair<int, int> snake_rc;
	char dir = 'w'; // wasd (머리의 방향)

	void init(pair<int, int>rc, Snake *front = nullptr, Snake *back = nullptr){
		snake_length ++;
		body_no = snake_length;
		frontBody = front;
		backBody = back;
		snake_rc = make_pair(rc.first, rc.second);
		if(front != nullptr)
			dir = front->dir;
	}

	void move(int x, int y){
		snake_rc = make_pair(x, y);
	}
};

class GateC {
public:
	pair<int, int> gate_rc;
	string wall_dir = "wasd";

	void init(int row, int col){
		gate_rc = make_pair(row, col);
	}
};

vector<GateC> gateVT;
Snake& MakeSnake(Snake *s);
vector<Snake> snakeVT;
void SetMap();
void VTmatchMap();
void ShowGame();
void ShowMap();
void ShowScore();
void ShowMission();
void MoveSnake();
void MakeItem();
void SetItem();
void GameOver();
void GameClear();
void MissionComplete();
void MakeGate();
void SetGate();
void GetGate();
int menu();
void print_menu(int thing);
void run();
void GameInit();
void munual();
void producers();

Snake& MakeSnake(Snake *s = nullptr){		//뱀의 길이가 늘어날 때 마다 호출.
	Snake *fb = s;		//생성될 스네이크 몸의 프론트 바디 주소.

	Snake *snake = new Snake;
	
	if(fb == nullptr) snake->init(make_pair(map_rows/2, map_cols/2), nullptr);
	else if(fb->dir == 'w') snake->init(make_pair(fb->snake_rc.first+1, fb->snake_rc.second), fb);
	else if(fb->dir == 'd') snake->init(make_pair(fb->snake_rc.first, fb->snake_rc.second-1), fb);
	else if(fb->dir == 's') snake->init(make_pair(fb->snake_rc.first-1, fb->snake_rc.second), fb);
	else if(fb->dir == 'a') snake->init(make_pair(fb->snake_rc.first, fb->snake_rc.second+1), fb);

	if(fb != nullptr) fb->backBody = snake;
	return *snake;
}

void SetMap(){		// 처음에 뱀과 맵을 구현.
	clear();
    GameInit();
	for(int i=0; i<map_rows; i++)
		for(int j=0; j<map_cols; j++)
			map[i][j]=0;
	snakeVT.clear();
	gateVT.clear();
	
	switch (stage)
	{
	case 1:
	snake_length = 0;
	get_GItem = 0;
	get_PItem = 0;
	get_Gate = 0;

	ms_snake_length = 5;
	ms_GItem = 5;
	ms_PItem = 3;
	ms_Gate = 3;

	for(int i=0; i<map_rows; i++){
		for(int j=0; j<map_cols; j++){
			if(i <= 1) map[i][j]++;
			if(i >= map_rows-2) map[i][j]++;
			if(j <= 1) map[i][j]++;
			if(j >= map_cols-2) map[i][j]++;
			if(i == map_rows/2 && j == map_cols/2) {
				snakeVT.push_back(MakeSnake());
				snakeVT.push_back(MakeSnake(&snakeVT[snake_length-1]));
				snakeVT.push_back(MakeSnake(&snakeVT[snake_length-1]));
			}
		}
	}
	break;

	case 2:
	snake_length = 0;
	get_GItem = 0;
	get_PItem = 0;
	get_Gate = 0;
	tick = 0;
	Gtick = 0;

	ms_snake_length = 10;
	ms_GItem = 8;
	ms_PItem = 5;
	ms_Gate = 5;

	for(int i=0; i<map_rows; i++){
		for(int j=0; j<map_cols; j++){
			if(i <= 1) map[i][j]++;
			if(i >= map_rows-2) map[i][j]++;
			if(j <= 1) map[i][j]++;
			if(j >= map_cols-2) map[i][j]++;
			if(i > 1 && i < map_rows-2 && (i == j || i == map_cols-j) && !(i > map_rows/2-3 && i < map_rows/2+3)) map[i][j]++;
			if(i == map_rows/2 && j == map_cols/2) {
				snakeVT.push_back(MakeSnake());
				snakeVT.push_back(MakeSnake(&snakeVT[snake_length-1]));
				snakeVT.push_back(MakeSnake(&snakeVT[snake_length-1]));
			}
		}
	}
	break;
	case 3:
	snake_length = 0;
	get_GItem = 0;
	get_PItem = 0;
	get_Gate = 0;
	tick = 0;
	Gtick = 0;

	ms_snake_length = 14;
	ms_GItem = 11;
	ms_PItem = 7;
	ms_Gate = 7;

    for(int i=0; i<map_rows; i++){
		for(int j=0; j<map_cols; j++){
			if(i <= 1) map[i][j]++;
			if(i >= map_rows-2) map[i][j]++;
			if(j <= 1) map[i][j]++;
			if(j >= map_cols-2) map[i][j]++;

			if(i > 1 && i < map_rows-2 && (j != 9 && j != 10 && j != 11 && j != 12 && i % 4 == 1) && !(i > map_rows/2-1 && i < map_rows/2+1)) map[i][j]++;
			if(i == map_rows/2 && j == map_cols/2) {
				snakeVT.push_back(MakeSnake());
				snakeVT.push_back(MakeSnake(&snakeVT[snake_length-1]));
				snakeVT.push_back(MakeSnake(&snakeVT[snake_length-1]));
			}
		}
	}
	break;

	case 4:
	snake_length = 0;
	get_GItem = 0;
	get_PItem = 0;
	get_Gate = 0;
	tick = 0;
	Gtick = 0;

	ms_snake_length = 18;
	ms_GItem = 14;
	ms_PItem = 9;
	ms_Gate = 9;
	for(int i=0; i<map_rows; i++){
		for(int j=0; j<map_cols; j++){
			if(i <= 1) map[i][j]++;
			if(i >= map_rows-2) map[i][j]++;
			if(j <= 1) map[i][j]++;
			if(j >= map_cols-2) map[i][j]++;

			if(i > 2 && i < map_rows-2 &&  j%3 == 0 && i%4 != 0) map[i][j]++;
			if(i == map_rows/2 && j == map_cols/2) {
				snakeVT.push_back(MakeSnake());
				snakeVT.push_back(MakeSnake(&snakeVT[snake_length-1]));
				snakeVT.push_back(MakeSnake(&snakeVT[snake_length-1]));
			}
	    }
	}
	break;

	case 5:
	GameClear();
	}
}

void VTmatchMap(){		//뱀을 벡터로 저장해뒀는데, 그걸 맵 배열에 동기화하는 코드. 
	Snake snake = snakeVT.front();
	if(map[snake.snake_rc.first][snake.snake_rc.second] == PItem){
		snakeVT.pop_back();
		snake_length--;
		get_PItem++;
		if(pass_gate != 0)
			pass_gate--;
		
		if(snake_length < 3) GameOver();
	}
	else if(map[snake.snake_rc.first][snake.snake_rc.second] == GItem){
		if(snake_length != max_snake_length)		// 현재 길이가 최대 길이라면 생성하지 않음. 먹은 수만 늘어남.
			snakeVT.push_back(MakeSnake(&snakeVT[snake_length-1]));
		get_GItem++;
		if(pass_gate != 0)
			pass_gate++;
	}
	else if(map[snake.snake_rc.first][snake.snake_rc.second] == Wall){
		GameOver();
	}
	else if(map[snake.snake_rc.first][snake.snake_rc.second] == Gate){
		GetGate();
		pass_gate = snake_length;
	}
	else if(map[snake.snake_rc.first][snake.snake_rc.second] == SBody){
		GameOver(); // 이거 왜 안 되지?? 이거 안 돼서 저기 아래, 12 행 아래 처리해줌.
		//GameOver();
		//if(snakeVT.back().snake_rc != make_pair(snake.snake_rc.first, snake.snake_rc.second))
		// Game Over But if SBody == tail, Keep Going
	}

	for(int i = 0; i<snake_length; i++){
		snake = snakeVT[i];
		if(snake.body_no == 1){
			map[snake.snake_rc.first][snake.snake_rc.second] = SHead;
		}
		else{
			if(map[snake.snake_rc.first][snake.snake_rc.second] == SBody) GameOver(); //여기에.
			else map[snake.snake_rc.first][snake.snake_rc.second] = SBody;
		}
	}
}

void ShowGame(){
	clear();
    
    resize_term(map_rows+3, map_cols+25);
    box(stdscr, 0, 0);

	init_pair(1, COLOR_BLUE, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_GREEN);
	init_pair(3, COLOR_WHITE, COLOR_BLACK);
	init_pair(4, COLOR_GREEN, COLOR_BLACK);
	init_pair(5, COLOR_RED, COLOR_BLACK);
	bkgd(COLOR_PAIR(1));
	VTmatchMap();
	

	refresh();
	ShowMap();
	ShowScore();
	ShowMission();

	getch();	
}

void ShowMap(){
	GameMap = newwin(map_rows, map_cols, 2, 2);
	wbkgd(GameMap, COLOR_PAIR(1));

	for(int i=1; i<map_rows-1; i++){
        for(int j=1; j<map_cols-1; j++){
			if(map[i][j] == IWall || map[i][j] == Wall){
				wattron(GameMap, COLOR_PAIR(2));
				mvwprintw(GameMap,i,j,"X");
				wattroff(GameMap, COLOR_PAIR(2));
			}
			else if(map[i][j] == SHead){
				wattron(GameMap,COLOR_PAIR(3));
				mvwprintw(GameMap,i,j,"@");
				wattroff(GameMap,COLOR_PAIR(3));
			}
			else if(map[i][j] == SBody){
				wattron(GameMap,COLOR_PAIR(3));
				mvwprintw(GameMap,i,j,"*");
				wattroff(GameMap,COLOR_PAIR(3));
			}
            else if(map[i][j] == GItem){
                wattron(GameMap,COLOR_PAIR(3));
				mvwprintw(GameMap,i,j,"G");
				wattroff(GameMap,COLOR_PAIR(3));
            }
            else if(map[i][j] == PItem){
                wattron(GameMap,COLOR_PAIR(3));
				mvwprintw(GameMap,i,j,"P");
				wattroff(GameMap,COLOR_PAIR(3));
            }
			else if(map[i][j] == Gate){
				wattron(GameMap,COLOR_PAIR(3));
				mvwprintw(GameMap,i,j,"O");
				wattroff(GameMap,COLOR_PAIR(3));
			}
        }
    }
	wrefresh(GameMap);
}

void ShowScore(){
	GameScore = newwin(8, 20, 3, map_cols+3);
    box(GameScore, 0, 0);
	
	wattron(GameMap,COLOR_PAIR(1));
	mvwprintw(GameScore, 1, 3, "Score Board");
	mvwprintw(GameScore, 2, 3, "B: %d / %d", snake_length, max_snake_length);
	mvwprintw(GameScore, 3, 3, "+: %d", get_GItem);
	mvwprintw(GameScore, 4, 3, "-: %d", get_PItem);
	mvwprintw(GameScore, 5, 3, "G: %d", get_Gate);
	mvwprintw(GameScore, 6, 3, "time: %d", tick/2);
	wattroff(GameMap,COLOR_PAIR(1));

	wrefresh(GameScore);
}

void ShowMission(){
	GameMission = newwin(7, 20, 11, map_cols+3);
	box(GameMission, 0, 0);

	mvwprintw(GameMission, 1, 3, "Mission");
	if(snake_length >= ms_snake_length){
		wattron(GameMission,COLOR_PAIR(4));
		mvwprintw(GameMission, 2, 3, "B: %d (v)", ms_snake_length);
		wattroff(GameMission,COLOR_PAIR(4));
	}
	else 
		mvwprintw(GameMission, 2, 3, "B: %d ( )", ms_snake_length);

	if(get_GItem >= ms_GItem){
		wattron(GameMission,COLOR_PAIR(4));
		mvwprintw(GameMission, 3, 3, "+: %d (v)", ms_GItem);
		wattroff(GameMission,COLOR_PAIR(4));
	}
	else
		mvwprintw(GameMission, 3, 3, "+: %d ( )", ms_GItem);
	
	if(get_PItem >= ms_PItem){
		wattron(GameMission,COLOR_PAIR(4));
		mvwprintw(GameMission, 4, 3, "-: %d (v)", ms_PItem);
		wattroff(GameMission,COLOR_PAIR(4));
	}
	else
		mvwprintw(GameMission, 4, 3, "-: %d ( )", ms_PItem);
	
	if(get_Gate >= ms_Gate){
		wattron(GameMission,COLOR_PAIR(4));
		mvwprintw(GameMission, 5, 3, "G: %d (v)", ms_Gate);
		wattroff(GameMission,COLOR_PAIR(4));
	}
	else 
		mvwprintw(GameMission, 5, 3, "G: %d ( )", ms_Gate);
		
	wrefresh(GameMission);

	if(snake_length >= ms_snake_length && get_GItem >= ms_GItem && get_PItem >= ms_PItem && get_Gate >= ms_Gate){
		stage++;
		MissionComplete();
		SetMap();
	}
}

void MoveSnake(){
	timeout(0);
	int bft = clock();
	int aft = clock();
	char ch=getch();

	while(aft - bft < 500000){
		aft = clock();
		if(ch != 'w' && ch != 'a' && ch != 's' && ch != 'd')
			ch = getch();
	}
	
	if(ch != 'w' && ch != 'a' && ch != 's' && ch != 'd'){
		ch = snakeVT.front().dir;
	}

	Snake *snake = &snakeVT.front();
	if(snake->dir == 'w' && ch == 's') GameOver();
	else if(snake->dir == 'd' && ch == 'a') GameOver();
	else if(snake->dir == 'a' && ch == 'd') GameOver();
	else if(snake->dir == 's' && ch == 'w') GameOver();
	else{
		char before_dir = snake->dir;
		char tmp_dir = snake->dir;

		pair<int, int> before_rc = make_pair(snake->snake_rc.first, snake->snake_rc.second);
		pair<int, int> tmp = make_pair(snake->snake_rc.first, snake->snake_rc.second);

		snake->dir = ch;

		for(int i=0; i<snake_length; i++){
			map[snake->snake_rc.first][snake->snake_rc.second] = 0;
			if(i>0){
				tmp = make_pair(snake->snake_rc.first, snake->snake_rc.second);
				snake->move(before_rc.first, before_rc.second);

				tmp_dir = snake->dir;
				snake->dir = before_dir;
			}
			else{
				switch(snake->dir){
					case 'w':
						snake->move(snake->snake_rc.first-1, snake->snake_rc.second);
						break;

					case 'd':
						snake->move(snake->snake_rc.first, snake->snake_rc.second+1);
						break;
					
					case 's':
						snake->move(snake->snake_rc.first+1, snake->snake_rc.second);
						break;

					case 'a':
						snake->move(snake->snake_rc.first, snake->snake_rc.second-1);
						break;
				}
			}

			before_dir = tmp_dir;
			before_rc = tmp;
			snake = &snakeVT[i+1];
		}
	}	
}

void MakeItem(){
    for(int r=0; r<map_rows; r++)
        for(int c=0; c<map_cols; c++)
            if(map[r][c]==GItem || map[r][c]==PItem)
                map[r][c]=0;
            
    for(int i = 0; i<3; i++)
        SetItem();
}

void SetItem(){
	while(1){
		int row, col;

		srand(clock());
		row = rand() % (map_rows-4) + 2;
		srand(clock());
		col = rand() % (map_cols-4) + 2;
		srand(clock());
		int item_kind = rand() % 2 + 5;



		if(map[row][col]==0){
			map[row][col] = item_kind;
			break;
		}
	}
}

void GameOver(){
	clear();
	game_running = false;
	int sizeR = (int)(((map_rows+3)*2/3)-((map_rows+3)*1/3));
	int sizeC = (int)(((map_cols+25)*2/3)-((map_cols+25)*1/3));
	GameEnd = newwin(sizeR, sizeC, (int)((map_rows+3)*1/3), (int)((map_cols+25)*1/3));
	box(GameEnd, 0, 0);
	
	wattron(GameEnd,COLOR_PAIR(5));
	mvwprintw(GameEnd, sizeR/2, sizeC/2-4, "GameOver");
	wattroff(GameEnd,COLOR_PAIR(5));
    mvprintw((int)((map_rows+3)*2/3), (int)((map_cols+25)*1/2-7), "Press any key.");

	refresh();
	wrefresh(GameEnd);
	wgetch(GameEnd);
}

void GameClear(){
	clear();
	game_running = false;
	int sizeR = (int)(((map_rows+3)*2/3)-((map_rows+3)*1/3));
	int sizeC = (int)(((map_cols+25)*2/3)-((map_cols+25)*1/3));
	GameEnd = newwin(sizeR, sizeC, (int)((map_rows+3)*1/3), (int)((map_cols+25)*1/3));
	box(GameEnd, 0, 0);
	
	wattron(GameEnd,COLOR_PAIR(4));
	mvwprintw(GameEnd, sizeR/2, sizeC/2-4, "GameClear");
	wattroff(GameEnd,COLOR_PAIR(4));
    mvprintw((int)((map_rows+3)*2/3), (int)((map_cols+25)*1/2-7), "Press any key.");

	refresh();
	wrefresh(GameEnd);
    wgetch(GameEnd);
}

void MissionComplete(){
	clear();
	int sizeR = (int)(((map_rows+3)*2/3)-((map_rows+3)*1/3));
	int sizeC = (int)(((map_cols+25)*2/3)-((map_cols+25)*1/3));
	GameEnd = newwin(sizeR, sizeC, (int)((map_rows+3)*1/3), (int)((map_cols+25)*1/3));
	box(GameEnd, 0, 0);
	
	wattron(GameEnd,COLOR_PAIR(4));
	mvwprintw(GameEnd, sizeR/2-1, sizeC/2-7, "MissionComplete");
	mvwprintw(GameEnd, sizeR/2+1, sizeC/2-7, "stage %d clear!", stage-1);
	mvwprintw(GameEnd, sizeR/2+2, sizeC/2-7, "run time : %d", tick/2);
	wattroff(GameEnd,COLOR_PAIR(4));
    mvprintw((int)((map_rows+3)*2/3), (int)((map_cols+25)*1/2-7), "Press any key.");

	refresh();
	wrefresh(GameEnd);
    wgetch(GameEnd);
}

void MakeGate(){
	while(!gateVT.empty()){
		map[gateVT.back().gate_rc.first][gateVT.back().gate_rc.second] = Wall;
		gateVT.pop_back();
	}
			
    for(int i = 0; i<2; i++)
        SetGate();
}

void SetGate(){
	while(1){
		int row, col;
		GateC *gate = new GateC;
		
		srand(clock());
		row = rand() % (map_rows-2) + 1;
		srand(clock());
		col = rand() % (map_cols-2) + 1;
		srand(clock());

		if(map[row][col]==Wall){
			gate->init(row, col);

			gateVT.push_back(*gate);
			map[row][col] = Gate;
			delete gate;
			break;
		}
	}
}

void GetGate(){
	get_Gate++;
	GateC *out_gate;
	if(snakeVT.front().snake_rc == gateVT.front().gate_rc) out_gate = &gateVT.back();
	else if(snakeVT.front().snake_rc == gateVT.back().gate_rc) out_gate = &gateVT.front();

	out_gate->wall_dir = "wasd";
	int row = out_gate->gate_rc.first;
	int col = out_gate->gate_rc.second;
	int pos;
	if(map[row-1][col]==Wall || map[row-1][col]==IWall || map[row-1][col]==Gate){
		pos = out_gate->wall_dir.find('w');
		out_gate->wall_dir.erase(pos, 1);
	}
	if(map[row][col-1]==Wall || map[row][col-1]==IWall || map[row][col-1]==Gate){
		pos = out_gate->wall_dir.find('a');
		out_gate->wall_dir.erase(pos, 1);
	}
	if(map[row+1][col]==Wall || map[row+1][col]==IWall || map[row+1][col]==Gate){
		pos = out_gate->wall_dir.find('s');
		out_gate->wall_dir.erase(pos, 1);
	}
	if(map[row][col+1]==Wall || map[row][col+1]==IWall || map[row][col+1]==Gate){
		pos = out_gate->wall_dir.find('d');
		out_gate->wall_dir.erase(pos, 1);
	}

	switch(snakeVT.front().dir){
		case 'w':
			if(out_gate->wall_dir.find('w') != -1){
				snakeVT.front().move(out_gate->gate_rc.first-1, out_gate->gate_rc.second);
				snakeVT.front().dir = 'w';
			}
			else if(out_gate->wall_dir.find('d') != -1){
				snakeVT.front().move(out_gate->gate_rc.first, out_gate->gate_rc.second+1);
				snakeVT.front().dir = 'd';
			}
			else if(out_gate->wall_dir.find('a') != -1){
				snakeVT.front().move(out_gate->gate_rc.first, out_gate->gate_rc.second-1);
				snakeVT.front().dir = 'a';
			}
			else{
				snakeVT.front().move(out_gate->gate_rc.first+1, out_gate->gate_rc.second);
				snakeVT.front().dir = 's';
			}
			break;

		case 'a':
			if(out_gate->wall_dir.find('a') != -1){
				snakeVT.front().move(out_gate->gate_rc.first, out_gate->gate_rc.second-1);
				snakeVT.front().dir = 'a';
			}
			else if(out_gate->wall_dir.find('w') != -1){
				snakeVT.front().move(out_gate->gate_rc.first-1, out_gate->gate_rc.second);
				snakeVT.front().dir = 'w';
			}
			else if(out_gate->wall_dir.find('s') != -1){
				snakeVT.front().move(out_gate->gate_rc.first+1, out_gate->gate_rc.second);
				snakeVT.front().dir = 's';
			}
			else{
				snakeVT.front().move(out_gate->gate_rc.first, out_gate->gate_rc.second+1);
				snakeVT.front().dir = 'd';
			}
			break;

		case 's':
			if(out_gate->wall_dir.find('s') != -1){
				snakeVT.front().move(out_gate->gate_rc.first+1, out_gate->gate_rc.second);
				snakeVT.front().dir = 's';
			}
			else if(out_gate->wall_dir.find('a') != -1){
				snakeVT.front().move(out_gate->gate_rc.first, out_gate->gate_rc.second-1);
				snakeVT.front().dir = 'a';
			}
			else if(out_gate->wall_dir.find('d') != -1){
				snakeVT.front().move(out_gate->gate_rc.first, out_gate->gate_rc.second+1);
				snakeVT.front().dir = 'd';
			}
			else{
				snakeVT.front().move(out_gate->gate_rc.first-1, out_gate->gate_rc.second);
				snakeVT.front().dir = 'w';
			}
			break;

		case 'd':
			if(out_gate->wall_dir.find('d') != -1){
				snakeVT.front().move(out_gate->gate_rc.first, out_gate->gate_rc.second+1);
				snakeVT.front().dir = 'd';
			}
			else if(out_gate->wall_dir.find('s') != -1){
				snakeVT.front().move(out_gate->gate_rc.first+1, out_gate->gate_rc.second);
				snakeVT.front().dir = 's';
			}
			else if(out_gate->wall_dir.find('w') != -1){
				snakeVT.front().move(out_gate->gate_rc.first-1, out_gate->gate_rc.second);
				snakeVT.front().dir = 'w';
			}
			else{
				snakeVT.front().move(out_gate->gate_rc.first, out_gate->gate_rc.second-1);
				snakeVT.front().dir = 'a';
			}
			break;
	}
}

int menu(){
	int thing = 0;
    bool choice_running = true;
    int choice;
    int c;

	initscr();
	clear();
	noecho();
	cbreak();
		
	MenuWin = newwin(6, 20, 1, 1);
	keypad(MenuWin, TRUE);
	refresh();
	print_menu(thing);
	while(choice_running){
        c = wgetch(MenuWin);
        switch(c){
            case (int)'w':
            case (int)'a':
            if(thing == 0) thing = 3;
            else thing -= 1;
            break;

            case (int)'s':
            case (int)'d':
            if(thing == 3) thing = 0;
            else thing += 1;
            break;

            case 10:
            choice = thing;
            choice_running = false;
            break;

            default :
            mvprintw(0, 0, "You can only use \'w\', \'a\', \'s\', \'d\' and \'Enter\'");
            refresh();
            break;
        }
        print_menu(thing);
    }
	//clrtoeol();
	refresh();
	endwin();

    return choice;
}

void print_menu(int thing)
{
	int r = 1;
    int c = 1;

    char ch[100];
    

	box(MenuWin, 0, 0);
	for(int i=0; i<sizeof(choices)/sizeof(*choices); i++){
        strcpy(ch,choices[i].c_str());
        if(i == thing){
            wattron(MenuWin, A_REVERSE); 
			mvwprintw(MenuWin, r, c, "%s", ch);
			wattroff(MenuWin, A_REVERSE);
        }
        else 
            mvwprintw(MenuWin, r, c, "%s", ch);
        r++;
    }
	wrefresh(MenuWin);
}

void run(){	
    stage = 1;
	initscr();
	start_color();
	resize_term(map_rows+3, map_cols+25);
    noecho();
  	curs_set(0);

	SetMap();
	ShowGame();	

	while(game_running){
		//뱀이 10번 움직이면 아이템 생성 (0.5초에 1번 움직이니까 5초 주기)
		if(tick % 10 == 0)		
			MakeItem();

		//뱀이 게이트를 지나지 않을 때, 생성. 		
		if(Gtick % 20 == 0 && pass_gate == 0 && tick > 20)
			MakeGate();

		MoveSnake();
		ShowGame();
		
		//뱀이 움직일 때마다 게이트 이용해야하는 몸의 길이는 줄어들음.
		if(pass_gate != 0)		
			pass_gate--;

		//게이트가 바뀔 때가 됐는데 뱀이 게이트를 지날때는 Gtick이 오르지 않는다.
		//뱀이 다 지나가면 바로 게이트가 바뀌도록. (생성 조건 -> 게임 시작 10초 이후)
		if((Gtick+1) % 20 != 0 || pass_gate == 0 && tick > 20)		
			Gtick++;						

		tick++;
	}
	endwin();
}

void GameInit(){
    tick = 0;
    Gtick = 0;

    game_running = true;
}

void munual(){    
    SubMenu = newwin(15, 80, 1, 1);

	box(SubMenu, 0, 0);
	mvwprintw(SubMenu, 1, 1, "Snake Game");
	mvwprintw(SubMenu, 3, 1, "The goal of the game is to complete stage 4.");
	mvwprintw(SubMenu, 4, 1, "Each stage has a mission to complete.");
    mvwprintw(SubMenu, 5, 1, "Check the mission and clear the game!");
    mvwprintw(SubMenu, 7, 1, "Use the 'w', 'a', 's', and 'd' keys to move the snake.");

    mvwprintw(SubMenu, 9, 1, "[ X ]");
    mvwprintw(SubMenu, 9, 7, "is Wall. Game over when it touches");

    mvwprintw(SubMenu, 10, 1, "[ G ]");
    mvwprintw(SubMenu, 10, 7, "is Growth Item. Eating this increases the length of the snake.");

    mvwprintw(SubMenu, 11, 1, "[ P ]");
    mvwprintw(SubMenu, 11, 7, "is Poison Item. Eating this decreases the length of the snake.");
    mvwprintw(SubMenu, 12, 7, "Game over if snake length is less than 3.");
    
    mvwprintw(SubMenu, 13, 1, "[ O ]");
    mvwprintw(SubMenu, 13, 7, "is Gate. You can use this to move to another gate.");

    mvprintw(16, 2, "Press any key.");
	refresh();
	wrefresh(SubMenu);
    wgetch(SubMenu);
}

void producers(){
    SubMenu = newwin(9, 50, 1, 1);

	box(SubMenu, 0, 0);
	mvwprintw(SubMenu, 1, 1, "Snake Game Producers");
	mvwprintw(SubMenu, 3, 1, "Kookmin univ. 20203129 Jeon Gyeongjin");
    mvwprintw(SubMenu, 4, 1, "jeonkj0413@naver.com");
	mvwprintw(SubMenu, 6, 1, "Kookmin univ. 20203114 Lee Minwoo");
    mvwprintw(SubMenu, 7, 1, "lmw6996@naver.com");

    mvprintw(10, 2, "Press any key.");
	refresh();
	wrefresh(SubMenu);
    wgetch(SubMenu);
}

int main(){
    while(menu_running){
        int choice = menu();
        switch (choice)
        {
        case 0: //GameStart
            run();
            break;
        case 1: //Game Manual
            munual();
            break;
        case 2: //Producers
            producers();
            break;
        case 3: //Exit
            endwin();
            menu_running = false;
            break;
        }
    }

	return 0;
}

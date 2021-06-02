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

bool game_running = true;
bool game_state;
int stage = 1;

WINDOW *GameMap;
WINDOW *GameScore;
WINDOW *GameMission;
WINDOW *GameEnd;

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
void run();

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

	if(i > 1 && i < map_rows-2 && (j!=9&&j!=10&&j!=11&&j!=12&&i%4==1) && !(i > map_rows/2-1 && i < map_rows/2+1)) map[i][j]++;
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

		if(i > 1 && i < map_rows-2 &&  j%3==0 && i%4!=0) map[i][j]++;
		if(i == map_rows/2 && j == map_cols/2) {
			snakeVT.push_back(MakeSnake());
			snakeVT.push_back(MakeSnake(&snakeVT[snake_length-1]));
			snakeVT.push_back(MakeSnake(&snakeVT[snake_length-1]));
		 }
	    }
	}
	break;

	}
		
}

void VTmatchMap(){		//뱀을 벡터로 저장해뒀는데, 그걸 맵 배열에 동기화하는 코드. 

	Snake snake = snakeVT.front();
	if(map[snake.snake_rc.first][snake.snake_rc.second] == PItem){
		snakeVT.pop_back();
		snake_length--;
		get_PItem++;
		
		if(snake_length < 3) GameOver(); //Game Over
	}
	else if(map[snake.snake_rc.first][snake.snake_rc.second] == GItem){
		snakeVT.push_back(MakeSnake(&snakeVT[snake_length-1]));
		get_GItem++;
	}
	else if(map[snake.snake_rc.first][snake.snake_rc.second] == Wall){
		GameOver(); // Game Over
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
	border('*','*','*','*','*','*','*','*');
	start_color();

	init_pair(1, COLOR_BLUE, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_GREEN);
	init_pair(3, COLOR_WHITE, COLOR_BLACK);
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
        }
    }
	wrefresh(GameMap);
}

void ShowScore(){
	GameScore = newwin(7, 20, 3, map_cols+3);
	wborder(GameScore, '*','*','*','*','*','*','*','*');
	
	wattron(GameMap,COLOR_PAIR(1));
	mvwprintw(GameScore, 1, 3, "Score Board");
	mvwprintw(GameScore, 2, 3, "B: %d / %d", snake_length, max_snake_length);
	mvwprintw(GameScore, 3, 3, "+: %d", get_GItem);
	mvwprintw(GameScore, 4, 3, "-: %d", get_PItem);
	mvwprintw(GameScore, 5, 3, "G: %d", get_Gate);
	wattroff(GameMap,COLOR_PAIR(1));

	wrefresh(GameScore);
}

void ShowMission(){
	GameMission = newwin(7, 20, 10, map_cols+3);
	wborder(GameMission, '*','*','*','*','*','*','*','*');

	wattron(GameMission,COLOR_PAIR(1));
	mvwprintw(GameMission, 1, 3, "Mission");
	if(snake_length >= ms_snake_length)
		mvwprintw(GameMission, 2, 3, "B: %d (v)", ms_snake_length);
	else 
		mvwprintw(GameMission, 2, 3, "B: %d ( )", ms_snake_length);

	if(get_GItem >= ms_GItem)
		mvwprintw(GameMission, 3, 3, "+: %d (v)", ms_GItem);
	else 
		mvwprintw(GameMission, 3, 3, "+: %d ( )", ms_GItem);
	
	if(get_PItem >= ms_PItem)
		mvwprintw(GameMission, 4, 3, "-: %d (v)", ms_PItem);
	else 
		mvwprintw(GameMission, 4, 3, "-: %d ( )", ms_PItem);
	
	if(get_Gate >= ms_Gate)
		mvwprintw(GameMission, 5, 3, "G: %d (v)", ms_Gate);
	else 
		mvwprintw(GameMission, 5, 3, "G: %d ( )", ms_Gate);
	
	if(snake_length >= ms_snake_length && get_GItem >= ms_GItem && get_PItem >= ms_PItem && get_Gate >= ms_Gate){
		stage++;
		SetMap();
	}
	wattroff(GameMission,COLOR_PAIR(1));
	wrefresh(GameMission);
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
	border('*','*','*','*','*','*','*','*');
	game_running = false;
	int sizeR = (int)(((map_rows+3)*2/3)-((map_rows+3)*1/3));
	int sizeC = (int)(((map_cols+25)*2/3)-((map_cols+25)*1/3));
	GameEnd = newwin(sizeR, sizeC, (int)((map_rows+3)*1/3), (int)((map_cols+25)*1/3));
	wborder(GameEnd, '*','*','*','*','*','*','*','*');
	
	wattron(GameEnd,COLOR_PAIR(1));
	mvwprintw(GameEnd, sizeR/2, sizeC/2-4, "GameOver");
	wattroff(GameEnd,COLOR_PAIR(1));

	refresh();
	wrefresh(GameEnd);

	sleep(100);
	endwin();
}

void run(){
    int tick = 0;
	
	initscr();
	
	resize_term(map_rows+3, map_cols+25);
	SetMap();
	ShowGame();

	noecho();
  	curs_set(0);

	while(game_running){
		if(tick % 10 == 0){
			MakeItem();
		}
		MoveSnake();
		ShowGame();
		tick++;
	}
	endwin();
}

int main(){
	run();
	return 0;
}

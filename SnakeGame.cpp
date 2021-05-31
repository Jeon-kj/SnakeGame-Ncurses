#include<ncurses.h>
#include<iostream>
#include<locale.h>
#include<vector>
//#include<conio.h>
#include <unistd.h>
#include <string>
using namespace std;

#define map_rows 25
#define map_cols 100
#define Wall 1
#define IWall 2
#define SHead 3
#define SBody 4

int map[map_rows][map_cols]={0};
int snake_length = 0;
WINDOW *GameMap;
WINDOW *GameScore;

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
	}

	void move(int x, int y){
		snake_rc = make_pair(x, y);
	}
};

Snake& MakeSnake(Snake *s);
void SetMap();
void VTmatchMap();
void MoveSnake();
vector<Snake> snakeVT;

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
}

void VTmatchMap(){		//뱀을 벡터로 저장해뒀는데, 그걸 맵 배열에 동기화하는 코드. 
	for(int i = 0; i<snake_length; i++){
		Snake snake = snakeVT[i];
		if(snake.body_no == 1){
			map[snake.snake_rc.first][snake.snake_rc.second] = SHead;
		}
		else{
			map[snake.snake_rc.first][snake.snake_rc.second] = SBody;
		}
	}
}

void ShowMap(){
	clear();
	border('*','*','*','*','*','*','*','*');
	start_color();

	init_pair(1, COLOR_BLUE, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_GREEN);
	init_pair(3, COLOR_WHITE, COLOR_BLACK);
	bkgd(COLOR_PAIR(1));

	GameMap = newwin(map_rows, map_cols, 3, 3);
	wbkgd(GameMap, COLOR_PAIR(1));

	VTmatchMap();
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
        }
    }
	
	refresh();
	wrefresh(GameMap);
	getch();
	
}	

void MoveSnake(){
	char ch = getch();
	if(ch != 'w' && ch != 'a' && ch != 's' && ch != 'd'){
		ch = snakeVT.front().dir;
	}
	timeout(200);

	Snake *snake = &snakeVT.front();
	if(snake->dir == 'w' && ch == 's') ;// Game Over
	else if(snake->dir == 'd' && ch == 'a') ;//Game Over
	else if(snake->dir == 'a' && ch == 'd') ;//Game Over
	else if(snake->dir == 's' && ch == 'w') ;//Game Over
	else{
		snake->dir = ch;

		pair<int, int> before_rc = make_pair(snake->snake_rc.first, snake->snake_rc.second);
		pair<int, int> tmp = make_pair(snake->snake_rc.first, snake->snake_rc.second);

		for(int i=0; i<snake_length; i++){
			map[snake->snake_rc.first][snake->snake_rc.second] = 0;
			if(i>0){
				tmp = make_pair(snake->snake_rc.first, snake->snake_rc.second);
				snake->move(before_rc.first, before_rc.second);
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
			before_rc = tmp;
			snake = &snakeVT[i+1];
		}
	}	
}

void run(){
	initscr();
	resize_term(map_rows+50, map_cols+50);

	SetMap();
	ShowMap();

	noecho();
  	curs_set(0);    //화면에 보이는 커서 설정, 0 : 커서 안보이게

	while(1){
		MoveSnake();
		ShowMap();
	}
	endwin();
}

int main(){
	run();

	return 0;
}

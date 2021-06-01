#include<ncurses.h>
#include<iostream>
#include<locale.h>
#include<vector>
//#include<conio.h>
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

int score = 0;
int map[map_rows][map_cols]={0};
int snake_length = 0;
bool game_state = true;
int stage = 2;

string error = "";
int errorcheck = 0;
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
		if(front != nullptr)
			dir = front->dir;
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
void MakeItem();
void SetItem();
void GameOver();

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
		break;
	case 4:
		break;

	}
		
}

void VTmatchMap(){		//뱀을 벡터로 저장해뒀는데, 그걸 맵 배열에 동기화하는 코드. 

	Snake snake = snakeVT.front();
	if(map[snake.snake_rc.first][snake.snake_rc.second] == PItem){
		snakeVT.pop_back();
		snake_length--;
		score--;
		
		if(snake_length < 3) GameOver(); //Game Over
	}
	else if(map[snake.snake_rc.first][snake.snake_rc.second] == GItem){
		snakeVT.push_back(MakeSnake(&snakeVT[snake_length-1]));
		score++;
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

	GameScore = newwin(30, 30, 3, map_cols+3);
	string tmp = to_string(score);
    char const *score_char = tmp.c_str();


	wattron(GameMap,COLOR_PAIR(1));
	mvwprintw(GameScore,1,1,"Score : ");
	mvwprintw(GameScore,1,9,score_char);
	wattroff(GameMap,COLOR_PAIR(1));

	refresh();
	wrefresh(GameMap);
	wrefresh(GameScore);
	getch();
	
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
	game_state = false;
	endwin();
}

void run(){
    int tick = 0;
	
	initscr();
	
	resize_term(map_rows+50, map_cols+50);
	SetMap();
	ShowMap();

	noecho();
  	curs_set(0);

	while(game_state){
		if(tick % 30 == 0){
			MakeItem();
		}
		MoveSnake();
		ShowMap();
		tick++;
	}
	endwin();
}

int main(){
	run();
	cout << error;
	return 0;
}

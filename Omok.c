#include <stdio.h>
#include <ncursesw/curses.h>
#include <locale.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include "Omok.h"
#include "server.c"
#define TIME 350000

int condition=0;
int x, y;
int ex, ey;
int S, port;
int MSC, ESC;
int win, lose;
char map_mirror[15][29];

main(){	
	int key, q = 1, q2 = 1, q3 = 1;
	char temp, checkt;
	int socket;
	pthread_t t1;
	void *flickering(void *);
	start();
	socket=setSocket();
	set_up();
	pthread_create(&t1, NULL, flickering, NULL);
	while(1){ //continue game
		resetMap();
		if(MSC == 11){
			attron(COLOR_PAIR(3));
			mvaddstr(10,30,"opponet turn   ");
			attron(COLOR_PAIR(1));
			refresh();
			receiveXY(&ex, &ey, socket);	
			map_mirror[ey][ex] = ESC;
			print(ex,ey,ESC);
		}//receive first if stone color white
		q = 1;
		while(q){ //one game
			fflush(stdin);
			refresh();
			q2 = 1;q3 = 1;
			x=14;y=7;
			attron(COLOR_PAIR(3));
			clearbottom();
			mvaddstr(10,30,"my turn        ");
			attron(COLOR_PAIR(1));
			print(x,y,MSC);//set game
			while(q2){ //put stone
				condition=1;
				refresh();
				key = getch();
				print(x,y,MSC);
				switch(key){
				case KEY_UP:
					if(y-1>=0){
						condition = 0;
						print(x,y, map_mirror[y][x]);
						y--;
						print(x,y,MSC);
						condition = 1;
					}
					break;
				case KEY_DOWN:
					if(y+1<=14){
						condition = 0;
						print(x,y, map_mirror[y][x]);
						y++;
						print(x,y,MSC);
						condition = 1;
					}
					break;
				case KEY_LEFT:
					if(x-2>=0){
						condition = 0;
						print(x,y, map_mirror[y][x]);
						x-=2;
						print(x,y,MSC);
						condition = 1;
					}
					break;
				case KEY_RIGHT:
					if(x+2<=29){
						condition = 0;
						print(x,y, map_mirror[y][x]);
						x+=2;
						print(x,y,MSC);
						condition = 1;
					}
					break;
				case '\n':case KEY_ENTER:
					//my turn
					key = put(x,y,MSC);
					if(!key){
						attron(COLOR_PAIR(3));
						mvaddstr(15,0,"you cannot put the stone .       ");
						attron(COLOR_PAIR(1));
						move(y,x);
						break;
					}
					if(key > 2 && key < 6){
						printbottom(key);
						move(y,x);
						break;
					} //lose
					sendXY(x,y,socket);
					if(key == 2){
						attron(COLOR_PAIR(3));
						mvaddstr(15,0,"You Win.                         ");
						attron(COLOR_PAIR(1));
						win++;
						print_stat();
						q = 0;
					} //win
					else if(key == 6){
						attron(COLOR_PAIR(3));
						mvaddstr(15,0,"Draw.                            ");
						attron(COLOR_PAIR(1));
						q = 0;
					} //draw
					refresh();
					if(!q){
						attron(COLOR_PAIR(3));
						mvaddstr(16,0,"Play again?(Y/N)                 ");
						refresh();
						while(q3){
							key = getch();
							switch(key){
							case 'Y':case 'y':
								sendXY(1,1,socket);
								mvaddstr(15,0,"wait a game.                     ");
								refresh();
								receiveXY(&x,&y,socket);
								if(x&&y){
									temp = ESC;
									ESC = MSC;
									MSC = temp;
									q3 = 0;
								}
								else if(S){
									close(socket);
									clear();
									mvaddstr(0,0,"Waiting gamer                    ");
									win = 0; lose = 0;
									refresh();
									socket = acceptt();
									MSC = 10;
									q3 = 0;
								}
								else{
									clear();
									mvaddstr(0,0,"opponent logout                  ");
									mvaddstr(1,0,"push any key                     ");
									refresh();
									getch();
									clear();
									endwin();
									close(socket);
									pthread_join(t1, NULL);
									exit(1);
								}
								break;
							case 'N':case 'n':
								sendXY(0,0,socket);
								close(socket);
								clear();
								endwin();
								close(socket);
								pthread_join(t1, NULL);
								exit(1);
							}
						}
						q2 = 0;
						break;
					}
					//enemy turn
					attron(COLOR_PAIR(3));
					mvaddstr(10,30,"opponet tur   n");
					clearbottom();
					attron(COLOR_PAIR(1));
					refresh();
					receiveXY(&ex, &ey, socket);
					key = put(ex,ey,ESC);
					map_mirror[ey][ex]=ESC;
					print(ex,ey, map_mirror[ey][ex]);
					if(key == 6){
						attron(COLOR_PAIR(3));
						mvaddstr(15,0,"Draw.                            ");
						attron(COLOR_PAIR(1));
						print_stat();
						q = 0;
					} //draw
					else if(key == 2){
						attron(COLOR_PAIR(3));
						mvaddstr(15,0,"you lose.                        ");
						attron(COLOR_PAIR(1));
						lose++;
						print_stat();
						q = 0;
					} //win
					refresh();
					if(!q){
						attron(COLOR_PAIR(3));
						mvaddstr(16,0,"play agane?(Y/N)                     ");
						refresh();
						while(q3){
							key = getch();
							switch(key){
							case 'Y':case 'y':
								sendXY(1,1,socket);
								mvaddstr(15,0,"waiting for reply                ");
								refresh();
								receiveXY(&x,&y,socket);
								if(x&&y){
									temp = ESC;
									ESC = MSC;
									MSC = temp;
									q3 = 0;
								}
								else if(S){
									close(socket);
									clear();
									mvaddstr(0,0,"waiting for new challenger.      ");
									refresh();
									win = 0; lose = 0;
									socket = acceptt();
									MSC = 10;
									q3 = 0;
								}
								else{
									mvaddstr(0,0,"opponent quit                    ");
									mvaddstr(1,0,"push any key.                    ");
									refresh();
									getch();
									clear();
									endwin();
									close(socket);
									exit(1);
								}
								break;
							case 'N':case 'n':
								sendXY(0,0,socket);
								close(socket);
								clear();
								endwin();
								close(socket);
								exit(1);
							}
						}
						q2 = 0;
						break;
					}
					q2 = 0;
					break;
				}
			}
		}
	}
	endwin();
}

void printbottom(int a){
	switch(a){
	case 3:
		attron(COLOR_PAIR(3));
		mvaddstr(15,0,"you cannot put a sotne.(more than 66!!)   ");
		break;
	case 4:
		attron(COLOR_PAIR(3));
		mvaddstr(15,0,"you cannot put a stone(33!!)              ");
		break;
	case 5:
		attron(COLOR_PAIR(3));
		mvaddstr(15,0,"you cannot put a stone(44!!)              ");
		break;
	}
}
void clearbottom(){
	mvaddstr(15,0,"                                             ");
	mvaddstr(16,0,"                                             ");
}
void *flickering(void * a){
	while(1){
		while(condition){
			attron(COLOR_PAIR(1));
			print(x,y,MSC);
			usleep(TIME);
			attron(COLOR_PAIR(1));
			print(x,y,map_mirror[y][x]);
			usleep(TIME);
			if(!condition){
				attron(COLOR_PAIR(1));
				print(x,y, map_mirror[y][x]);
			}
		}
	}
}
void resetMap(){
	int i,j;
	clear();
	attron(COLOR_PAIR(1));
	for(i=0; i<15; i++)
		for(j=0; j<29; j++)
			map_mirror[i][j] = map[i][j];
	for(i=0; i<15; i++){
		for(j=0; j<29; j++){
			move(i,j);
			switch(map[i][j]){
			case 0:
				addstr("+");
				break;
			case 1:
				addstr("-");
				break;
			case 2:
				addstr("+");
				break;
			case 3:
				addstr("+");
				break;
			case 4:
				addstr("+");
				break;
			case 5:
				addstr("+");
				break;
			case 6:
				addstr("+");
				break;
			case 7:
				addstr("+");
				break;
			case 8:
				addstr("+");
				break;
			case 9:
				addstr("+");
				break;
			}
		}
	}
	print_stat();
}
void print(int xx, int yy, char a){
	move(yy,xx);
	switch(a){
	case 0:
		addstr("+");
		break;
	case 1:
		addstr("+");
		break;
	case 2:
		addstr("+");
		break;
	case 3:
		addstr("+");
		break;
	case 4:
		addstr("+");
		break;
	case 5:
		addstr("+");
		break;
	case 6:
		addstr("+");
		break;
	case 7:
		addstr("+");
		break;
	case 8:
		addstr("+");
		break;
	case 9:
		addstr("+");
		break;
	case 10:
		addstr("O");
		break;
	case 11:
		//attron(COLOR_PAIR(2));
		addstr("X");
		//attron(COLOR_PAIR(1));
		break;
	}
	refresh();
}
void print_stat(){
	int i,j,k;
	attron(COLOR_PAIR(3));
	for(i=0; i<6; i+=5){
		for(j=0; j<5; j++){
			for(k=0; k<9; k++){
				move(j+i,k+30);
				switch(stat[j][k]){
				case 0:
					addstr("+");
					break;
				case 1:
					addstr("+");
					break;
				case 2:
					addstr("+");
					break;
				case 4:
					addstr("+");
					break;
				case 5:
					addstr("+");
					break;
				case 7:
					addstr("+");
					break;
				}
			}
		}
	}
	mvprintw(1,34,"   me   ");
	mvprintw(2,31,"win :%2d", win);
	mvprintw(3,31,"lose:%2d", lose);
	mvprintw(6,31,"opponent");
	mvprintw(7,31,"win :%2d", lose);
	mvprintw(8,31,"lose:%2d", win);
	attron(COLOR_PAIR(1));
	refresh();
}
int isVictory( char MSCc){
	int dx[] ={1, 0, 1,-1};
	int dy[] = {0, 1, 1,1};
	int i,x1,y1,d;
	int tx;
	int ty;
	const int NOD = 4;


	for(d=0; d<NOD; d++){
		for(x1=0; x1<29; x1+=2){
			for(y1=0; y1<15; y1++){
				for(i=0 ; i<5; i++){
					tx = x1 + dx[d] * i*2;
					ty =  y1 + dy[d] * i;
					if(  map_mirror[ty][tx] != MSCc ) 
						break;
					if( ty>15||tx>31) 
						break;
				}
				if( i == 5 ) 
					return 1; //** 게임이 끝남!
			}
		}
	}
	return 0; //** 게임이 끝나지 않음
}
int six(int xx, int yy, char MSCc) {
	int dx[] = { 1, 0, 1,-1 };
	int dy[] = { 0, 1, 1,1 };
	int i, x, y, d;
	int tx;
	int ty;
	int temp;
	const int NOD = 4;
	temp = map_mirror[yy][xx];
	map_mirror[yy][xx] = MSCc;

	if (MSCc == 10) {
		for (d = 0; d < NOD; d++) {
			for (x = 0; x < 29; x += 2) {
				for (y = 0; y < 15; y++) {
					for (i = 0; i < 10; i++) {
						tx = x + dx[d] * i * 2;
						ty = y + dy[d] * i;
						if (map_mirror[ty][tx] != MSCc)
							break;
						if (ty > 14 || tx > 29)
							break;
						if (i == 5) {
							map_mirror[yy][xx] = temp;
							return 1; //**금수 : 육목검출.
						}
					}
				}
			}
		}
	}
	map_mirror[yy][xx] = temp;
	return 0; //** 게임이 끝나지 않음
}
int threethree(int xx, int yy, char MSCc)
{
	int t, c, i, j, k, three, four, cc;
	char tempMap;

	three = 0, four = 0;

	for (j = 0; j < 4; j++){
		t = 0, cc = 0;
		for (k = j; k < 8; k = k + 4){
			c = 0;
			for (i = 1; i < 5; i++){
				if (k == 0){ //북쪽
					if (yy - i < 0){
						i--;
						if (map_mirror[yy - i][xx] != 10 && map_mirror[yy - i][xx] != 11){
							i++;
							break;
						}
						i++;
						cc++;
						break;
					}
					tempMap = map_mirror[yy - i][xx];
				}
				else if (k == 1){ //북동쪽
					if (yy - i < 0 || xx + i * 2 > 28){
						i--;
						if (map_mirror[yy - i][xx + i * 2] != 10 && map_mirror[yy - i][xx + i * 2] != 11){
							i++;
							break;
						}
						i++;
						cc++;
						break;
					}
					tempMap = map_mirror[yy - i][xx + i * 2];
				}
				else if (k == 2){ //동쪽
					if (xx + i * 2 > 28){
						i--;
						if (map_mirror[yy][xx + i * 2] != 10 && map_mirror[yy][xx + i * 2] != 11){
							i++;
							break;
						}
						i++;
						cc++;
						break;
					}
					tempMap = map_mirror[yy][xx + i * 2];
				}
				else if (k == 3){ //남동쪽
					if (yy + i > 14 || xx + i * 2 > 28){
						i--;
						if (map_mirror[yy + i][xx + i * 2] != 10 && map_mirror[yy + i][xx + i * 2] != 11){
							i++;
							break;
						}
						i++;
						cc++;
						break;
					}
					tempMap = map_mirror[yy + i][xx + i * 2];
				}
				else if (k == 4){ //남쪽
					if (yy + i > 14){
						i--;
						if (map_mirror[yy + i][xx] != 10 && map_mirror[yy + i][xx] != 11){
							i++;
							break;
						}
						i++;
						cc++;
						break;
					}
					tempMap = map_mirror[yy + i][xx];
				}
				else if (k == 5){ //남서쪽
					if (yy + i > 14 || xx - i * 2 < 0){
						i--;
						if (map_mirror[yy + i][xx - i * 2] != 10 && map_mirror[yy + i][xx - i * 2] != 11){
							i++;
							break;
						}
						i++;
						cc++;
						break;
					}
					tempMap = map_mirror[yy + i][xx - i * 2];
				}
				else if (k == 6){ //서쪽
					if (xx - i * 2 < 0){
						i--;
						if (map_mirror[yy][xx - i * 2] != 10 && map_mirror[yy][xx - i * 2] != 11){
							i++;
							break;
						}
						i++;
						cc++;
						break;
					}
					tempMap = map_mirror[yy][xx - i * 2];
				}
				else if (k == 7){ //북서쪽
					if (yy - i < 0 || xx - i * 2 < 0){
						i--;
						if (map_mirror[yy - i][xx - i * 2] != 10 && map_mirror[yy - i][xx - i * 2] != 11){
							i++;
							break;
						}
						i++;
						cc++;
						break;
					}
					tempMap = map_mirror[yy - i][xx - i * 2];
				}

				if (MSCc == tempMap){
					t++;
				}
				else if (tempMap == 10 || tempMap == 11){
					cc++;
					break;
				}
				else{
					if (c == 1)
						break;

					if (i == 1){
						t = t + 10;
					}
					c = 1;
				}
			}
		}

		if (t == 2 || t == 12){
			if (cc == 0)
				three++;
		}
		if (t == 3 || t == 13 || t == 23){
			if (cc != 2)
				four++;
		}
	}

	if (three >= 2)
		return 1;
	if (four >= 2)
		return 2;	

	return 0; //** 게임이 끝나지 않음
}
int isDraw() {
	int  x1, y1;
	for (x1 = 0; x1 < 29; x1 += 2) {
		for (y1 = 0; y1 < 15; y1++) {
			if (map_mirror[y1][x1] == 10 || map_mirror[y1][x1] == 11)
				return 0;
		}
	}
	return 1; //draw.
}
int put(int xx,int yy,char MSCc){
	int End, ban, i, j;
	char temp;
	if( map_mirror[yy][xx]==10 || map_mirror[yy][xx] == 11)	//stone exist at xy
		return 0;

	ban=six(xx,yy,MSCc);
	if(ban)
		return 3; //if black, lose

	if (MSCc == 10){
		temp = map_mirror[yy][xx];
		map_mirror[yy][xx] = MSCc;
		for (i = 0; i < 29; i = i + 2){
			for (j = 0; j < 15; j++){
				if (MSCc == map_mirror[j][i]){
					ban = threethree(i, j, MSCc);
					if (ban == 1){
						map_mirror[yy][xx] = temp;
						return 4;//33 rule
					}
					if (ban == 2){
						map_mirror[yy][xx] = temp;
						return 5;//44 rule
					}
				}
			}
		}
	}

	map_mirror[yy][xx] = MSCc;
	//pthread_mutex_lock(&Locker);
	condition = 0;
	print(xx,yy, map_mirror[yy][xx]);
	End = isVictory(MSCc);

	if(End)
		return 2; //victory or lose
	End = isDraw();
	if (End)
		return 6; //draw

	return 1; //continue game
}
void set_up(){
	int i,j,k;
	clear();
	noecho();
	start_color();

	init_color(3, 880,716,368);
	init_pair(1, COLOR_BLACK, 3);
	init_pair(2, COLOR_WHITE, 3);
	init_pair(3, COLOR_WHITE, COLOR_BLACK);

	resetMap();
	refresh();
}
int setSocket(){
	int q=1, i;
	char ipaddr[20];
	char temp[10];
	while(1){
		clear();
		mvaddstr(0,0,"1. server");
		mvaddstr(1,0,"2. client");
		i = getch();
		switch(i){
		case '1':
			clear();
			echo();
			mvaddstr(0,0,"Port: ");
			refresh();
			getstr(temp);
			port = atoi(temp);
			clear();
			mvaddstr(0,0,"wait challenger");
			refresh();
			if((q = open_server(port)) == -1){
				mvaddstr(1,0,"Dont open server.");
			}
			clear();
			set_up();
			refresh();
			S = 1;
			MSC = 10;
			ESC = 11;
			noecho();
			print_stat();
			return q;
		case '2':
			clear();
			echo();
			mvaddstr(0,0,"IP address: ");
			refresh();
			getstr(ipaddr);
			mvaddstr(1,0,"Port: ");
			refresh();
			getstr(temp);
			port = atoi(temp);
			if((q = connect_server(port, ipaddr)) == -1){
				clear();
				mvaddstr(0,0,"cannot access that server.    ");
				mvaddstr(1,0,"please check the server info. ");
				refresh();
				getch();
				noecho();
				break;
			}
			S = 0;
			MSC=11;
			ESC=10;
			set_up();
			noecho();
			return q;
		}
	}
}
void start(){
	int q=1;
	int i;
	setlocale(LC_CTYPE, "ko_KR.utf-8");
	initscr();
	keypad(stdscr, TRUE);
	noecho();

	while(q){
		clear();
		mvaddstr(4,39,"OMOK");
		mvaddstr(12,35,"1. S T A R T  ");
		mvaddstr(13,35,"2. HOW To PLAY");
		mvaddstr(14,35,"3. Q  U  I  T ");
		refresh();
		i = getch();
		switch(i){
		case '1':
			q = 0;
			refresh();
			break;
		case '2':
			clear();
			move(0,0);
			printw("1. To win you should put five stones in a row\n");
			printw("2. If you are black stone, you cannot put 33, 44, more 66.\n");
			printw("3. If you are white stone, you apply the same rule.       \n");
			getch();
			break;		
		case '3':
			endwin();
			exit(1);
		}
	}
}


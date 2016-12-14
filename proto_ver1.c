#include	"omok.h"

void set_cr_noecho_mode();
void set_up();
void wrap_up();
int bounce_or_lose(struct stone *sp);
int set_ticker(int);


int turn = 1; // 1이면 흑돌 2이면 백돌
int c;
struct	stone	p1;
struct	stone	p2;
int pan[BOTTOM+1][REDGE+1] = {0,};


int main()
{	
	//바둑판을 0으로 초기화함.
	//int i,j;
	
	set_up();//기본세팅
	while( c = getch()){
		if( c == 'q' || c == 'Q')//종료
			break;
		if(turn == 1){//1p차례인 경우
		move(0, 10);
		addstr("1P의 차례입니다");
		refresh();
			if(c == LEFT)           
			{
				p1.x_pos--;
				//clear();
			}
			else if(c == RIGHT)
			{
				p1.x_pos++;
				//clear();
			}
			else if(c == DOWN)
			{
				p1.y_pos++;
				//clear();
			}
			else if(c == UP) 
			{
				p1.y_pos--;
				//clear();
			}
			else if(c == SPACE)//space를 입력하면 pan이라는 배열에 위치가 저장됨
							//배열에 저장된 숫자가 1이면1p위치 2면 2p위치
			{	//기존에 바둑놀이 놓여있는상태라면 저장을 하지 않고 다시 위치를 받아야함.
				if(pan[p1.x_pos][p1.y_pos] == 1 || pan[p1.x_pos][p1.y_pos] == 2)
					continue;
				pan[p1.x_pos][p1.y_pos] = 1;
				turn = 2;
			}
			bounce_or_lose(&p1);
                }
		else if(turn == 2){
			move(0,10);
			addstr("2P의 차례입니다");
            if(c == LEFT)
			{           
				p2.x_pos--;
				//clear();
			}
			else if(c == RIGHT)
			{
				p2.x_pos++;
				//clear();
			}
            else if(c == DOWN)
			{	
				p2.y_pos++;
				//clear();
			}
            else if(c == UP)
			{
				p2.y_pos--;
				//clear();
			}
            else if(c == SPACE)
			{
				//기존에 바둑놀이 놓여있는상태라면 저장을 하지 않고 다시 위치를 받아야함.
				if(pan[p2.x_pos][p2.y_pos] == 1 || pan[p2.x_pos][p2.y_pos] == 2)
					continue;
				pan[p2.x_pos][p2.y_pos] = 2;
				turn = 1;
			}
				bounce_or_lose(&p2);
			}
        	clear();
	}
	wrap_up();
	/*for(i=TOP;i<=BOTTOM;i++){
		for(j=LEDGE;j<=REDGE;j++){
			printf("%d", pan[i][j]);
		}
		printf("\n");
	}*/
		
	return 0;
}
void set_cr_noecho_mode()
{
	struct	termios	ttystate;

	tcgetattr(0,&ttystate);
	ttystate.c_lflag	&= ~ICANON;
	ttystate.c_lflag	&= ~ECHO;
	ttystate.c_cc[VMIN]	= 1;
	tcsetattr(0, TCSANOW, &ttystate);
}

void set_up()
{
	int q = 1;
	int i;
	void stone_move(int);

	p1.y_pos = Y_INIT;
	p1.x_pos = X_INIT;
	p2.y_pos = Y_INIT;
        p2.x_pos = X_INIT;

	p1.symbol = Wstone;//1p가 백돌
	p2.symbol = Bstone;//2p가 흑돌
	initscr();
	set_cr_noecho_mode;
	
	while(q){
		clear();
		mvaddstr(4,39, "오목");
		mvaddstr(12,35,"1. 시    작");
		mvaddstr(13,35,"2. 게임방법");
		mvaddstr(14,35,"3. 종	 료");
		refresh();
		i = getch();

		switch(i){
		case '1' :
			q = 0;
			refresh();
			break;
		case '2' :
			clear();
			move(0,0);
			printw("1. 5개의 돌을 연속해서 놓으면 승리합니다.\n");
			printw("2. 흑돌인 경우 33, 44, 육목이상 위치에 돌을 놓을 수 없습니다.\n");
			printw("3. 백돌인 경우 33, 44, 육목이상 위치에 돌을 놓을 수 없습니다.\n");
			getch();
			break;
		case '3' :
			endwin();
			exit(1);
		}
	}
	
	signal(SIGINT, SIG_IGN);
	//선을 결정하는 부분 지금은 급한것이 아니라 주석처리함
	
	//addstr("please select who first(1 : white, 2: black)");
	//turn = getch();
	clear();	
	if(turn == 1)
		mvaddch(p1.y_pos, p1.x_pos, p1.symbol);
	else if(turn == 2)
		mvaddch(p2.y_pos, p2.y_pos, p2.symbol);
	refresh();

	signal(SIGALRM, stone_move);
	set_ticker(1000 / TICKS_PER_SEC);//나중에 시간경과를 보여주는 부분을 추가하면 좋을듯
}

void wrap_up()
{
	int i,j;
	for(i=0;i<BOTTOM;i++){
		for(j=0;j<REDGE;j++){
			if(pan[i][j] == 1)
				printf("O");
			else if(pan[i][j] == 2)
				printf("X");
			else
				printf("╉");
		}
		printf("\n");
	}
	while(1)
		pause();
	set_ticker(0);
	endwin();
}

void stone_move(int signum)
{
	int y_cur, x_cur, moved;
	
	//signal(SIGALRM, SIG_IGN);
	if(turn == 1) //1p차례일때 돌 움직이는 것을 구현
	{
		y_cur = p1.y_pos;
		x_cur = p1.x_pos;
		moved = 1;
		if(moved) {
       		mvaddch(y_cur, x_cur, BLANK);
            mvaddch(y_cur, x_cur, BLANK);
            mvaddch(p1.y_pos, p1.x_pos, p1.symbol);
            refresh();
			moved=0;
			y_cur = p1.y_pos;
			x_cur = p1.x_pos;
		}
	
	}
	else //2p차례일 때 움직임
	{
		y_cur = p2.y_pos;
		x_cur = p2.x_pos;
		moved = 1;
		if(moved) {
			mvaddch(y_cur, x_cur, BLANK);
			mvaddch(y_cur, x_cur, BLANK);
			mvaddch(p2.y_pos, p2.x_pos, p2.symbol);
			refresh();
			moved=0;
			y_cur = p2.y_pos;
			x_cur = p2.x_pos;
		}
	}
	
	signal(SIGALRM, stone_move);
}

int bounce_or_lose(struct stone *sp)
{
	int	return_val = 0;
	
	if(sp->y_pos < TOP){
		sp->y_pos = BOTTOM+1;
		return_val = 1;
	} else if(sp->y_pos > BOTTOM) {
		sp->y_pos = TOP-1;
		return_val = 1;
	}
	if(sp->x_pos < LEDGE) {
		sp->x_pos = REDGE-1;
		return_val = 1;
	} else if(sp->x_pos > REDGE) {
		sp->x_pos = LEDGE+1;
		return_val = 1;
	}
	return return_val;
}


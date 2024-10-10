/*
Makefile
BIN = \
    mod

all: $(BIN)

clean:
    rm -f $(BIN) *.o

mod: \
        mod.c
    gcc -o $@ $^ -lcurses
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<curses.h>
#include<signal.h>
#include<unistd.h>
#include<sys/select.h>

#define drawWidth 60
#define drawHeight 20
#define inputWidth 15
#define inputHeight 15

int data[8] = {0};
int idx[8] = {0};
char messageArea[15] = {'\0'};

WINDOW *draw,*input;
WINDOW *fdraw,*finput;
fd_set mask;

void win(int sig) {
    endwin();
    puts("Nice work!");
    system("cat ./flag*");
    exit(0);
}

void die(){
    endwin();
    exit(1);
}

void init(){
    initscr();
    draw = newwin(drawHeight,drawWidth,1,1);
    input = newwin(inputHeight,inputWidth,6,drawWidth+4);
    fdraw = newwin(drawHeight+2,drawWidth+2,0,0);
    finput = newwin(inputHeight+2,inputWidth+2,5,drawWidth+3);

    box(fdraw,'|','-');
    box(finput,'|','-');
    scrollok(draw,TRUE);
    scrollok(input,TRUE);
    wmove(draw,0,0);
    wmove(input,0,0);
    cbreak();
    noecho();
    char initMessage[11][11] = {
        "[>] xa1 = \0",
        "[ ] ya1 = \0",
        "[ ] xa2 = \0",
        "[ ] ya2 = \0",
        "[ ] xb1 = \0",
        "[ ] yb1 = \0",
        "[ ] xb2 = \0",
        "[ ] yb2 = \0",
        "          \0",
        "j : down  \0",
        "k : up    \0",
    };
    for(int i = 0;i < 11;i++){
        for(int j = 0;j < 10;j++) {
            wmove(input,i,j);
            waddch(input,(chtype)initMessage[i][j]);
        }
    }
    wmove(input,0,10);
    wrefresh(fdraw);
    wrefresh(finput);
    wrefresh(draw);
    wrefresh(input);
}

void rectangle(int x1,int y1,int x2,int y2){
    y1 = drawHeight -y1 -1;
    y2 = drawHeight -y2 -1;
    wmove(draw,y2,x1);
    waddch(draw,'+');
    for(int i = 0;i < x2-x1-1;i++){
        waddch(draw,'-');
    }
    if(x1 != x2)waddch(draw,'+');
    for(int i=0;i < y1-y2-1;i++){
        wmove(draw,y2+i+1,x1);
        waddch(draw,'|');
        if(x1 != x2){
            wmove(draw,y2+i+1,x2);
            waddch(draw,'|');
        }
    }
    wmove(draw,y1,x1);
    waddch(draw,'+');
    for(int i = 0;i < x2-x1-1;i++){
        waddch(draw,'-');
    }
    if(x1 != x2)waddch(draw,'+');
}

void clean(){
    for(int i = 0;i < drawHeight;i++){
        wmove(draw,i,0);
        for(int j = 0;j < drawWidth;j++){
            waddch(draw,' ');
        }
    }
}

int min(int a, int b){
    return (a < b) ? a : b;
}
    
int max(int a, int b){
    return (a > b) ? a : b;
}


void loop(){
    char currentLow = 0;
    while(1){
        fd_set readfd;
        FD_ZERO(&readfd);
        FD_SET(STDIN_FILENO, &readfd);
        int res = select(STDIN_FILENO + 1, &readfd,NULL,NULL,NULL);
        if(FD_ISSET(STDIN_FILENO,&readfd)){
            int c = getchar();
            if(c == 'j') {
                if(currentLow == 7)continue;
                wmove(input,currentLow,1);
                waddch(input,' ');
                currentLow++;
                wmove(input,currentLow,1);
                waddch(input,'>');
                wmove(input,currentLow,idx[currentLow]+10);
            }
            else if(c == 'k') {
                wmove(input,currentLow,1);
                waddch(input,' ');
                currentLow--;
                wmove(input,currentLow,1);
                waddch(input,'>');
                wmove(input,currentLow,idx[currentLow]+10);
            }
            else if(c == 'q') {
                die();
                break;
            }
            else{
                if(c >= '0' && c <= '9'){
                    if(data[currentLow] * 10 >= 50 && currentLow % 2 == 0)continue;
                    if(data[currentLow] * 10 >= 15 && currentLow % 2 == 1)continue;
                    idx[currentLow]++;
                    data[currentLow] = data[currentLow] * 10 + c - '0';
                    waddch(input,(char)c);

                }
                if(c == '\b' || c == 0x10 || c == 0x7f){
                    if(idx[currentLow] == 0)continue;
                    idx[currentLow]--;
                    data[currentLow] = (data[currentLow] - data[currentLow] % 10) / 10;
                    wmove(input,currentLow,idx[currentLow]+10);
                    waddch(input,' ');
                    wmove(input,currentLow,idx[currentLow]+10);
                }
                clean();
                int flag = 0;
                if(data[2] != 0 && data[3] != 0){
                    rectangle(data[0],data[1],data[2],data[3]);
                    flag = 1;
                }
                if(data[6] != 0 && data[7] != 0){
                    rectangle(data[4],data[5],data[6],data[7]);
                    if(flag == 1){
                        int diffWidth = min(data[2],data[6]) - max(data[0],data[4]);
                        int diffHeight = min(data[3],data[7]) - max(data[1],data[5]);
                        int area = (diffWidth > 0 && diffHeight > 0) ? diffWidth * diffHeight : 0;
                        strcpy(messageArea,"Duplicated: ");
                        sprintf(&messageArea[6],"%4d",area);
                        for(int i = 0;messageArea[i] != '\0';i++){
                            wmove(input,12,i);
                            waddch(input,(chtype)messageArea[i]);
                        }
                        wmove(input,currentLow,idx[currentLow]+10);
                    }
                }
                wrefresh(draw);
            }
            wrefresh(input);
        }
    }
}

int main(void) {
    signal(SIGSEGV,win);
    signal(SIGINT,die);
    setbuf(stdout, NULL);
    init();
    loop();
}
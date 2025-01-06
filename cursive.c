#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <ctype.h>
#include <sys/ioctl.h>

#define CTRL_KEY(k) ((k) & 0x1f) //get character value 

struct editorConfig {
    int screenrows;
    int screencols;
    struct termios old_raw;
};

struct editorConfig E;

void die(const char *s){
    write(STDOUT_FILENO, "\x1b[2J",4);
    write(STDOUT_FILENO, "\x1b[H",3);
   
    perror(s);
    exit(1);     
}

void disableRawMode(){
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.old_raw) == -1) die("tcsetattr");
}

  
void enableRawMode(){
    /**
     * this function makes a new instance of the termios struct to
     * store the current config of the terminal to it.
     * We get the old terminal config and upon exit reset to the old config
     * 
     * Then it gets the termial config 
     * and stores in it in the raw struct we made. Then we bitwise NOT the echo and 
     * and to the c_lfag. This has the effect of not echoing types characters to the
     * terminal screen. Then we set these terminal settings (tcsaflush clears the buffers
     * before changes come into effect
     */

    if (tcgetattr(STDIN_FILENO, &E.old_raw) == -1) die("tcgetattr");
    atexit(disableRawMode);  //register clean up early
    struct termios raw = E.old_raw;
    raw.c_iflag &= ~(BRKINT | INPCK | ISTRIP | IXON | ICRNL); //disable software control flow
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | ISIG| IEXTEN); 
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

char editorReadKey(){
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO, &c,1)) != 1){
        if (nread == -1 && errno != EAGAIN) die("read");
    }    
    return c;
}

int getWindowSize(int *rows, int *cols){
    struct winsize ws;

    if (ioctl(STDOUT_FILENO,TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0){
        return -1;
    } else {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;

    }
}

void editorProcessKeypress(){
    char c = editorReadKey();
    switch (c){
        case CTRL_KEY('q'):
            exit(0);
            break;
    }
 }

void editorDrawRows(){
    int y;
    for (y=0; y<E.screenrows;y++){
        write(STDOUT_FILENO, "-\r\n",3);
    }
    if (y < E.screenrows -1){
        write(STDOUT_FILENO, "\r\n",2);
    }
}


void editorRefreshScreen(){
    write(STDOUT_FILENO, "\x1b[2J",4);
    write(STDOUT_FILENO, "\x1b[H",3);
    
    editorDrawRows();
    write(STDOUT_FILENO, "\x1b[H",3);
}

void initEditor(){
    if (getWindowSize(&E.screenrows, &E.screencols) == -1) die("getWindowSize");
}

int main(){
    enableRawMode();
    initEditor();
    while (1){
        editorRefreshScreen();
        editorProcessKeypress();
     }
    
}


#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <ctype.h>

struct termios old_raw;

void die(const char *s){
    perror(s);
    exit(1);     
}

void disableRawMode(){
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &old_raw) == -1) die("tcsetattr");
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

    if (tcgetattr(STDIN_FILENO, &old_raw) == -1) die("tcgetattr");
    atexit(disableRawMode);  //register clean up early
    struct termios raw = old_raw;
    raw.c_iflag &= ~(BRKINT | INPCK | ISTRIP | IXON | ICRNL); //disable software control flow
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | ISIG| IEXTEN); 
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

int main(){
    enableRawMode();
    while (1){

        char c = '\0';
        if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) die("read");
        if (iscntrl(c)){
            printf("%d\r\n",c);
        }
        else {
            printf("this is the char %d ('%c')\r\n",c,c);
        }
        if (c == 'q') break;
         return 0;
     }
    
}


#include <stdio.h>
#include <unistd.h>
#include <termios.h>

void enableRawMode(){
    /**
     * this function makes a new instance of the termios struct to
     * store the current config of the terminal to it. Then it gets the termial config 
     * and stores in it in the raw struct we made. Then we bitwise NOT the echo and 
     * and to the c_lfag. This has the effect of not echoing types characters to the
     * terminal screen. Then we set these terminal settings (tcsaflush clears the buffers
     * before changes come into effect
     */
    struct termios raw; 
    tcgetattr(STDIN_FILENO, &raw); 
    raw.c_lflag &= ~(ECHO); 
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main(){
    enableRawMode();
    char c;
    while (read(STDIN_FILENO, &c, 1) ==1 && c != 'q');
    return 0;
}


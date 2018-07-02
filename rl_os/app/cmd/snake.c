#include <stdio.h>
#include <termios.h>
#include <fcntl.h> 
#include <unistd.h>

/* platform */
#if 0
int mygetch() {
        char ch;
        int error;
        static struct termios Otty, Ntty;

        int oldf; 

        fflush(stdout);
        tcgetattr(0, &Otty);
        Ntty = Otty;

        Ntty.c_iflag  =  0;     /* input mode       */
        Ntty.c_oflag  =  0;     /* output mode      */
        Ntty.c_lflag &= ~ICANON;    /* line settings    */

#if 1
        /* disable echoing the char as it is typed */
        Ntty.c_lflag &= ~ECHO;  /* disable echo     */
#else
        /* enable echoing the char as it is typed */
        Ntty.c_lflag |=  ECHO;  /* enable echo      */
#endif

        Ntty.c_cc[VMIN]  = CMIN;    /* minimum chars to wait for */
        Ntty.c_cc[VTIME] = CTIME;   /* minimum wait time    */

#if 0
        /*
         *     * use this to flush the input buffer before blocking for new input
         *         */
        #define FLAG TCSAFLUSH
#else
        /*
         *     * use this to return a char from the current input buffer, or block if
         *         * no input is waiting.
         *             */
        #define FLAG TCSANOW

#endif
        oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
        if ((error = tcsetattr(0, FLAG, &Ntty)) == 0) {
                error  = read(0, &ch, 1 );        /* get char from stdin */
                error += tcsetattr(0, FLAG, &Otty);   /* restore old settings */
                fcntl(STDIN_FILENO, F_SETFL, oldf);
        }

            return (error == 1 ? (int) ch : -1 );
}
#else
int mygetch() {
  int c;
  read(0, &c, 1);
  return c;
}

#endif

/* logic */


#define DIRECTION_LEFT  0
#define DIRECTION_DOWN  1
#define DIRECTION_RIGHT 2
#define DIRECTION_UP    3

unsigned char self[20];
unsigned char current_direction = 4; /* L D R U */
unsigned char current_length = 6; /* len * 2 */

unsigned char fb[8];

unsigned char need_enlarge = 0;

unsigned char food_x = 0;
unsigned char food_y = 0;

unsigned char current_random = 0;



unsigned char random() {
    current_random += 73;
    return current_random;
}

void generate_food() {
    unsigned char iterator;
    unsigned char check_x;
    unsigned char check_y;
    
    printf("gen\n");
regenerate:
    iterator = 0;
    food_x = random() & 0x07;//0b00000111;
    food_y = random() & 0x07;//0b00000111;
    while(1) {
        if(iterator >= current_length)
            break;        
        check_x = self[iterator];
        iterator++;
        check_y = self[iterator];
        iterator++;


        printf("food %d %d chec %d %d it %d\n", food_x, food_y, check_x, check_y, iterator);

        if(check_x == food_x) {
            if(check_y == food_y) { 
                goto regenerate;
            }
        }
    }

}

void move() {
    unsigned char current_head_x = self[0];
    unsigned char current_head_y = self[1];
    unsigned char iterator;
    unsigned char iterator2;
    unsigned char current_tail_x;
    unsigned char current_tail_y;



    if(current_direction == DIRECTION_LEFT) {
        current_head_x -= 1;
    } else if(current_direction == DIRECTION_RIGHT) {
        current_head_x += 1;
    } else if(current_direction == DIRECTION_UP) {
        current_head_y -= 1;
    } else if(current_direction == DIRECTION_DOWN) {
        current_head_y += 1;
    } else {
        return;
    }

        if(current_head_x > 7)
            goto fail;
        if(current_head_y > 7) 
            goto fail;


recheck:


        printf("ch %d %d f %d %d\n", current_head_x, current_head_y, food_x, food_y);
    if(current_head_x == food_x) {
        if(current_head_y == food_y) {
            need_enlarge = 1;
            generate_food();
            random();
            goto recheck;
        }
    }

    iterator = 2;
    while(1) {
        unsigned char check_x;
        unsigned char check_y;
        if(iterator >= current_length)
            break;
        
        check_x = self[iterator];
        iterator++;
        check_y = self[iterator];
        iterator++;

        if(current_head_x == check_x) {
            if(current_head_y == check_y) {
                goto fail;
            }
        }


    }


    if(need_enlarge) {
        need_enlarge = 0;
        current_length += 2;
    }

    
    iterator = current_length - 2; /* from */
    iterator2 = current_length; /* to */
    while(1) {
        if(iterator > current_length)
            break;

        self[iterator2] = self[iterator];
        iterator--;
        iterator2--;

        self[iterator2] = self[iterator];
        iterator--;
        iterator2--;

    }

    self[0] = current_head_x;
    self[1] = current_head_y;

    return;

fail:
                printf("Fail!\n");
                while(1) {}


}

void check_control() {
    int c;
    c = mygetch();
    if(c == 'a' && current_direction != DIRECTION_RIGHT)
        current_direction = DIRECTION_LEFT;
    if(c == 'd' && current_direction != DIRECTION_LEFT)
        current_direction = DIRECTION_RIGHT;
    if(c == 'w' && current_direction != DIRECTION_DOWN)
        current_direction = DIRECTION_UP;
    if(c == 's' && current_direction != DIRECTION_UP)
        current_direction = DIRECTION_DOWN;

}

void update_fb() {
    unsigned char iterator = 0;
    fb[0] = 0;
    fb[1] = 0;
    fb[2] = 0;
    fb[3] = 0;
    fb[4] = 0;
    fb[5] = 0;
    fb[6] = 0;
    fb[7] = 0;

    while(1) {
        unsigned char x;
        unsigned char y;
        unsigned char c_self;
        x = self[iterator];
        iterator++;
        y = self[iterator];
        iterator++;

        fb[y] = fb[y] | (1 << x);
        if(iterator >= current_length)
            break;
    }


}

void draw_food() {
    fb[food_y] = fb[food_y] | (1 << food_x);
}

void undraw_food() {
    fb[food_y] = fb[food_y] & (~(1 << food_x));
}


void draw() {
    unsigned char c = 0;
    unsigned char r = 0;
    printf("%c[1J%c[H", 0x1b, 0x1b);
    printf("Direction: %d\n##########\n#", current_direction);
    for(r = 0; r < 8; r++) {
        for(c = 0; c < 8; c++) {
            printf("%s", (fb[r] & (1<<c))?"@":".");
        }
        printf("#\n#");
    }
    printf("#########\n");

}

void delay() {
    int it = 0;
    while(it < 1000) {
        it++;
    }
}


int main() {
    self[0] = 0x04;//0b00000100;
    self[1] = 0x04;//0b00000100;

    self[2] = 0x03;//0b00000011;
    self[3] = 0x04;//0b00000100;

    self[4] = 0x02;//0b00000010;
    self[5] = 0x04;//0b00000100;
   /* 
    self[6] = 0b00000001;
    self[7] = 0b00000100;
    
    self[8] = 0b00000000;
    self[9] = 0b00000100;
    
    self[10] = 0b00000000;
    self[11] = 0b00000101;

    self[12] = 0b00000000;
    self[13] = 0b00000110;
*/

    //generate_food();
    while(1) {
        check_control();
        move();
        update_fb();

        draw_food();
        draw();
        delay();
        undraw_food();
        draw();
        delay();
    }

    return 0;
}

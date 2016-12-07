#include <stdio.h>
#include <termios.h>
#include <fcntl.h> 
#include <unistd.h>

/* platform */
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
unsigned char iterator = 0;
unsigned char iterator2;
unsigned char check_x;
unsigned char check_y;
unsigned char new_head_x;
unsigned char new_head_y;

unsigned char current_tail_x;
unsigned char current_tail_y;

unsigned char tmp_x;
unsigned char tmp_y;

unsigned char random() {
//ldam [current_random]         ; 2
//ldbi 73                       ; 2
//add a                         ; 1
//sta [current_random]         ; 2
//ret                           ; 1


    current_random += 73;
    return current_random;
}

unsigned char is_valid(unsigned char x, unsigned char y) {
//sta [tmp_x]                ; 2
//stbm [tmp_y]                ; 2
//ldbi 7                      ; 2
//jg ret_0                    ; 2
//ldam [tmp_y]                ; 2
//jg ret_0                    ; 2
//ldai 0                      ; 2
//sta [iterator]             ; 2
//loop:                       ; 0
//ldam [iterator]             ; 2
//ldbm [current_length]       ; 2
//jge ret_1                   ; 2
//ldbi self                   ; 2
//add a                       ; 2
//indir a                     ; 1
//sta [check_x]              ; 2
//ldam [iterator]               ; 2
//ldbi 1                        ; 2
//add a                         ; 1
//ldbi self                     ; 2
//add a                         ; 1
//indir a                       ; 1
//sta [check_y]                 ; 2
//ldam [iterator]               ; 2
//ldbi 2                        ; 2
//add a                         ; 1
//sta [iterator]                ; 2
//ldam [check_x]                ; 2
//ldbm [tmp_x]                  ; 2
//jne loop                      ; 2
//ldam [check_y]                ; 2
//ldbm [tmp_y]                  ; 2
//jne loop                      ; 2
//jmp ret_0                     ; 2
//ret_0:                        ; 0
//ldai 0                        ; 2
//ret                           ; 1
//ret_1:                        ; 0
//ldai 1                        ; 2
//ret                           ; 1
    if(x > 7)
        return 0;
    if(y > 7)
        return 0;


    iterator = 0;
    while(1) {
        if(iterator >= current_length)
            break;        
        check_x = self[iterator];
        iterator++;
        check_y = self[iterator];
        iterator++;

        if(check_x == x) {
            if(check_y == y) { 
                return 0;
            }
        }
    }
    return 1;

}


void generate_food() {
    
regenerate:
//regenerate:                   ; 0
//ldbi 0b00000111               ; 2
//call random                   ; 2
//and  a                        ; 1
//sta [food_x]                 ; 2
//call random                   ; 2
//and a                         ; 1
//sta [food_y]                 ; 2
//ldam [food_x]                 ; 2
//call is_valid                 ; 2
//ldbi 0                        ; 2
//eq regenerate                 ; 2
//ret                           ; 1

    food_x = random() & 0b00000111;
    food_y = random() & 0b00000111;

    if(!is_valid(food_x, food_y))
        goto regenerate;

}

void move() {
//ldam [self]                   ; 2
//sta [new_head_x]             ; 2
//ldam [self+1]                 ; 2
//sta [new_head_y]             ; 2
//ldam [curent_direction]       ; 2
//ldbi DIRECTION_LEFT           ; 2
//jne L1                        ; 2
//ldam [new_head_x]             ; 2
//ldbi 1                        ; 2
//sub a                         ; 1
//sta [new_head_x]             ; 2
//jmp L10                       ; 2
//L1:                           ; 0
//ldam [curent_direction]       ; 2
//ldbi DIRECTION_RIGHT          ; 2
//jne L2                        ; 2
//ldam [new_head_x]             ; 2
//ldbi 1                        ; 2
//add a                         ; 1
//sta [new_head_x]             ; 2
//jmp L10                       ; 2
//L2:                           ; 0
//ldam [curent_direction]       ; 2
//ldbi DIRECTION_UP             ; 2
//jne L3                        ; 2
//ldam [new_head_y]             ; 2
//ldbi 1                        ; 2
//sub a                         ; 1
//sta [new_head_y]             ; 2
//jmp L10                       ; 2
//L3:                           ; 0
//ldam [curent_direction]       ; 2
//ldbi DIRECTION_DOWN           ; 2
//jne L4                        ; 2
//ldam [new_head_y]             ; 2
//ldbi 1                        ; 2
//add a                         ; 1
//sta [new_head_y]             ; 2
//jmp L10                       ; 2
//L4:                           ; 0
//ret                           ; 1
//L10:                   recheck; 0
//ldam [food_x]                 ; 2
//ldbm [new_head_x]             ; 2
//jne  L5                       ; 2
//ldam [food_y]                 ; 2
//ldbm [new_head_y]             ; 2
//jne  L5                       ; 2
//ldai 1                        ; 2
//sta [need_enlarge]           ; 2
//call [generate_food]          ; 2
//call [random]                 ; 2
//jmp  L10                      ; 2
//L5:                           ; 0
//ldam [new_head_x]             ; 2
//ldbm [new_head_y]             ; 2
//call is_valid                 ; 2
//ldbi 0                        ; 2
//jeq fail                      ; 2
//ldam [need_enlarge]           ; 2
//jeq L6                        ; 2
//stbm [need_enlarge]           ; 2
//ldam [current_length]         ; 2
//ldbi 2                        ; 2
//add a                         ; 1
//sta [current_length]         ; 2
//L6:                           ; 0
    new_head_x = self[0];
    new_head_y = self[1];

    if(current_direction == DIRECTION_LEFT) {
        new_head_x -= 1;
    } else if(current_direction == DIRECTION_RIGHT) {
        new_head_x += 1;
    } else if(current_direction == DIRECTION_UP) {
        new_head_y -= 1;
    } else if(current_direction == DIRECTION_DOWN) {
        new_head_y += 1;
    } else {
        return;
    }


recheck:


        printf("ch %d %d f %d %d\n", new_head_x, new_head_y, food_x, food_y);
    if(new_head_x == food_x) {
        if(new_head_y == food_y) {
            need_enlarge = 1;
            generate_food();
            random();
            goto recheck;
        }
    }
    
    if(!is_valid(new_head_x, new_head_y)) {
        goto fail;
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

    self[0] = new_head_x;
    self[1] = new_head_y;

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
    volatile unsigned long long it = 0;
    while(it < 100000000) {
        it++;
    }
}


int main() {
    self[0] = 0b00000100;
    self[1] = 0b00000100;

    self[2] = 0b00000011;
    self[3] = 0b00000100;

    self[4] = 0b00000010;
    self[5] = 0b00000100;
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

    generate_food();
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

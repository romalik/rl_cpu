#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

char path[64*4];
void do_pwd(char * prefix) {
  getcwd(path, 64*4);
  printf("-------->>>>>>>>>>>> %s : %s\n", prefix, path);
}

int main(int argc, char **argv) {
  int i = 0;
  do_pwd("1 Main");
  do_pwd("2 Main");
  do_pwd("3 Main");
  if(i = fork()) {
    do_pwd("1 Parent");
    do_pwd("2 Parent");
    do_pwd("3 Parent");
  } else {
    do_pwd("1 Child");
    do_pwd("2 Child");
    do_pwd("3 Child");

  }


  return 0;
}

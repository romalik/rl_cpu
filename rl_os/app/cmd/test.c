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
  unsigned int * a;
  unsigned int * b;
  //  do_pwd("1 Main");
//  do_pwd("2 Main");
//  do_pwd("3 Main");
  *a = 10;
  if(i = fork()) {
      printf("Parent:\n");
    while(1) {printf("Parent a = %d\n", *a);}
  } else {
      printf("Child:\n");
      *a = 20;
      while(1) {printf("Child a = %d\n", *a);}
  }


  return 0;
}

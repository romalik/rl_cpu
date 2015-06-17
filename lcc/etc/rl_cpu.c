/* hostfile for rl_cpu */                                                                       
                                                                                                             
#include <string.h>                                                                                          
                                                                                                             
static char rcsid[] = "";                                                                                    
                                                                                                             
#ifndef LCCDIR                                                                                               
#define LCCDIR "/usr/local/rl_cpu/bin/"                                                                             
#endif                                                                                                       

#ifndef LCCLIBDIR                                                                                               
#define LCCLIBDIR "/usr/local/rl_cpu/lib/"                                                                             
#endif                                                                                                       
                                                                                                             
char *suffixes[] = { ".c", ".i", ".s", ".o", ".out", 0 };                                                    
char inputs[256] = "";                                                                                       
char *cpp[] = { LCCDIR "cpp",                                                                                
   "-D__STDC__=1",                                                                                           
   "$1", "$2", "$3", 0 };                                                                                    
char *include[] = { "-I" LCCDIR "include", 0 };                                                              
char *com[] = { LCCDIR "rcc", "-target=xbytecode",                                                             
   "$1", "$2", "$3", 0 };                                                                                    
char *as[] = { LCCDIR "asm_bc", "$1", "$2", "$3", 0 };     
char *ld[] = { LCCDIR "link", "-o", "$3", LCCLIBDIR "progbeg.o", LCCLIBDIR "rt.o", "$1", "$2", 0};                                             
                                                                                                             
extern char *concat(char *, char *);                                                                         
                                                                                                             
int option(char *arg) {                                                                                      
   if (strncmp(arg, "-lccdir=", 8) == 0) {                                                                   
       cpp[0] = concat(&arg[8], "/cpp");                                                                     
       include[0] = concat("-I", concat(&arg[8], "/include"));                                               
       com[0] = concat(&arg[8], "/rcc");                                                                     
   }                                                                                                         
   else if (strncmp(arg, "-ld=", 4) == 0)                                                                    
       ld[0] = &arg[4];                                                                                      
   else                                                                                                      
       return 0;                                                                                             
   return 1;                                                                                                 
}                                                         


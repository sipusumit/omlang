#include<unistd.h>
#include<stdlib.h>

void print(char *s){
  write(STDOUT_FILENO, s,sizeof(s));
};

char* input(){
  char *c = (char *)calloc(100, sizeof(char));
  read(STDIN_FILENO, c, 99);
  int i = 0;
  while(c[i] != '\0'){
    i++;
  }
  if(c[i-2] == '\n'){
    c[i-2] = '\0';
  }
  return c;
}
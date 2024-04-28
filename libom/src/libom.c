#include<unistd.h>
#include<stdlib.h>

void cosmicSpit(char *s){
  write(STDOUT_FILENO, s, strlen(s));
};

char* galacticReceive(){
  char *c = (char *)calloc(256, sizeof(char));
  read(STDIN_FILENO, c, 255);
  int i = 0;
  while(c[i] != '\0'){
    i++;
  }
  if(c[i-2] == '\n'){
    c[i-2] = '\0';
  }
  return c;
}
#include <stdio.h>
#include <unistd.h>
 
int index;
 
int main(int argc, char *argv[])
{
    int res;
 
    index = 0;
    while ( index < 20000 ) {
        res = index * 2;
        printf("%d\n", res);
        sleep(1);
        index ++;
    }
 
    return 0;
}

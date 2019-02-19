//https://stackoverflow.com/questions/11451618/how-do-you-read-the-mouse-button-state-from-dev-input-mice

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv)
{
    int fd, fd2,bytes;
    unsigned char data[3];

    const char *pDevice = "/dev/input/mice";
    const char *pChardev_leds = "/dev/chardev_leds";
    // Open Mouse
    fd = open(pDevice, O_RDWR);
    if(fd == -1){
        printf("ERROR Opening %s\n", pDevice);
        return -1;
    }


    if( (fd2=open(pChardev_leds, O_WRONLY | O_APPEND))==-1 ){
    	printf("ERROR Opening %s\n", pChardev_leds);
        return -1;	
    }

    int left, middle, right;
    signed char x, y;
    char* a[3];

    bzero(a,sizeof(a));
    
    while(1){
        // Read Mouse     
        bytes = read(fd, data, sizeof(data));

        if(bytes > 0){

            left = data[0] & 0x1;
            right = data[0] & 0x2;
            middle = data[0] & 0x4;

            x = data[1];
            y = data[2];
            printf("x=%d, y=%d, left=%d, middle=%d, right=%d\n", x, y, left, middle, right);

            if(left != 0)
            	strcat(a,"3");

            if(right != 0)
            	strcat(a,"1");

            if(middle != 0)
            	strcat(a,"2");

            printf("Cadena a escribir: %s \n",a);

            write(fd2,a,strlen(a));

            strcpy(a,"");

            sleep(1);

            write(fd2,a,strlen(a));
            
        }   
    }


    return 0; 
}
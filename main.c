/***********************************************************/

/* By Milton Maldonado Jr.
 * Baseado no post de Michael Griffin
 * https://control.com/forums/threads/rs232-serial-port-c-linux-disable-rts.25961/post-136318
 */

#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>
#include <string.h>
#include <sys/select.h>
#include <stdlib.h>
#include <getopt.h>

#define VERSION "1.3"

void send_hexfile(char *filename,int fd);

////////////////////////////////////////////////////////////////////////////////
int wait_char(int fd){

    fd_set rfds;
    struct timeval tv;
    int retval;

    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);

    tv.tv_sec = 0;
    tv.tv_usec = 1000;

    retval = select(1+fd, &rfds, NULL, NULL, &tv);
    /* Don't rely on the value of tv now! */

    if (retval == -1){
        perror("select()");
        return 0;
    }
    else if (retval)
        return 1;//printf("Data is available now.\n");
        /* FD_ISSET(fd, &rfds) will be true. */
    else
        return 0;//printf("No data within five seconds.\n");
}

////////////////////////////////////////////////////////////////////////////////
void send_hexfile(char *filename,int fd){

    char bufhex[200];
    char bufrsp[100];
    char bufrx[100];
    int pbufrsp = 0;

    FILE *f = fopen(filename,"r");

    if (!f){

        perror("Hex file error\n");
        return;
    }

    int retry = 0;

    while (!feof(f)){

        bufhex[0] = 0;
        fgets(bufhex,sizeof(bufhex),f);
        strcat (bufhex,"\r");

repeat:
        printf("===%s",bufhex);

        if (!strncmp(bufhex,":00000001FF",11))
            goto endfile;

        write(fd,bufhex,strlen(bufhex));

        int count_ms = 2500;
        while (count_ms){

            if (wait_char(fd)){

                int nb = read(fd,bufrx,sizeof(bufrx));

                for (int i = 0; i < nb; i++){
                    char c = bufrx[i];
                    //putchar(c);
                    if ((c == '\n') || (c == '\r')){

                        pbufrsp = 0;

                        if (!strncmp(bufrsp,"Verify OK.",10)) {
                            printf("OK\n");
                            retry = 0;
                            bufrsp[0] = 0;
                            goto line_ok;
                        }

                        if (!strncmp(bufrsp,"VERIFY ERROR",12)){
                            bufrsp[0] = 0;
                            printf("Error\n====== RETRY ======\n");
                            goto repeat;
                        }

                    }
                    else{

                        if (pbufrsp < sizeof(bufrsp))
                            bufrsp[pbufrsp++] = c;
                    }
                }
            }
            else
                usleep(1000);
            --count_ms;
        }

        if (!count_ms){

            retry++;
            if (retry < 6){
                printf("TIMEOUT!\nRetry.\n");
                goto repeat;
            }

            printf("TIMEOUT!\nAborting...\n");
            exit(-1);
        }

line_ok: {}
        usleep(50000);
    }
endfile:
    fclose(f);
}

////////////////////////////////////////////////////////////////////////////////
void read_eeprom(char *filename,int fd, int memsize){

#define READSIZE 32
    char buf[32];
    int i;
    FILE *f = NULL;

    if (filename[0]){

        f = fopen(filename,"w");

        if (!f){

            perror("Hex file error\n");
            return;
        }
    }

//    printf("Memsize:%04x\n",memsize);
    for (i = 0; i < memsize; i += READSIZE){

        sprintf(buf,"<%04x%02x",i,READSIZE);

        //printf("**%s**\n",buf);
        write(fd,buf,strlen(buf));

        int count_ms = 10;
        while (count_ms){
            if (wait_char(fd)){

                int nb = read(fd,buf,sizeof(buf));
                buf[nb] = 0;
                if (f)
                    fprintf(f,"%s",buf);
                else
                    printf("%s",buf);
            }
            usleep(10000);
            --count_ms;
        }
    }
    if (f){
        fprintf(f,"%s\n",":00000001FF");
        fclose(f);
    }
    else
        printf(":00000001FF\n");
}

////////////////////////////////////////////////////////////////////////////////
typedef enum {

    COD_PRM_INFILE=101,
    COD_PRM_OUTFILE=102,
    COD_PRM_DEVICE=103,
    COD_CMD_VERSION=1,
    COD_CMD_READ=2,
    COD_CMD_HELP=3,
} cod_option;

struct option longopts[] = {
    {
        "f",                    //const char *name;
        required_argument,      //int         has_arg;
        0,                      //int        *flag;
        COD_PRM_INFILE,         //int         val;
    },
    {
        "o",                    //const char *name;
        required_argument,      //int         has_arg;
        0,                      //int        *flag;
        COD_PRM_OUTFILE,        //int         val;
    },
    {
        "d",                    //const char *name;
        required_argument,      //int         has_arg;
        0,                      //int        *flag;
        COD_PRM_DEVICE,         //int         val;
    },
    {
        "v",                    //const char *name;
        0,                      //int         has_arg;
        0,                      //int        *flag;
        COD_CMD_VERSION,        //int         val;
    },
    {
        "r",                    //const char *name;
        required_argument,      //int         has_arg;
        0,                      //int        *flag;
        COD_CMD_READ,           //int         val;
    },
    {
        "h",                    //const char *name;
        0,                      //int         has_arg;
        0,                      //int        *flag;
        COD_CMD_HELP,           //int         val;
    },
    {
        "help",                 //const char *name;
        0,                      //int         has_arg;
        0,                      //int        *flag;
        COD_CMD_HELP,           //int         val;
    },
    {
        0,                      //const char *name;
        0,                      //int         has_arg;
        0,                      //int        *flag;
        0,                      //int         val;
    }
};

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[]) {

    int res;
    int longindex;
    char infile[200]="";
    char outfile[200]="";
    char indevice[200]="";
    int cmd = 0;
    char readopt[10]="";
    int willread = 0;
    int willwrite= 0;
    int memsize;

    int fd;

    for (;;){

        res = getopt_long_only(argc, argv, "", longopts, &longindex);

        if ((res == -1) || (res == 63))
            break;

        //printf("res:%d longindex:%d\n",res,longindex);
        int val = longopts[longindex].val;

        if ((cmd)&&(val<100)){

            perror("Use only one option!\n");
            exit(-1);
        }

        if (val < 100)
            cmd = val;

        switch (val){

        case COD_PRM_INFILE:
            willwrite = 1;
            if (optarg)
                strncpy(infile,optarg,sizeof(infile)-1);
            break;

        case COD_PRM_OUTFILE:
            if (optarg)
                strncpy(outfile,optarg,sizeof(outfile)-1);
            break;

        case COD_PRM_DEVICE:
            if (optarg)
                strncpy(indevice,optarg,sizeof(indevice)-1);
            break;

        case COD_CMD_READ:
            if (optarg)
                strncpy(readopt,optarg,sizeof(readopt)-1);
            willread = 1;
            break;

        case COD_CMD_VERSION:
            printf("Version:%s\n",VERSION);
            exit(0);
        }
    }

    if (willread && willwrite)
        cmd = COD_CMD_HELP;

    if (!indevice[0])
        cmd = COD_CMD_HELP;

    if (!willread && !infile[0])
        cmd = COD_CMD_HELP;

    if (willread){
        if (!strcmp(readopt,"2k"))
            memsize = 2048;
        else
        if (!strcmp(readopt,"8k"))
            memsize = 8192;
        else
            cmd = COD_CMD_HELP;
    }

    if (cmd == COD_CMD_HELP){

        printf("eeprom_sender by ARMCoder\n");
        printf("Usage:\n");
        printf(" eeprom_sender -v\n");
        printf(" eeprom_sender -f <filename> -d <serial_device>\n");
        printf(" eeprom_sender -r {2k|8k} [-o <filename>] -d <serial_device>\n");
        exit(0);
    }

    ////////////////////////////////////////////////////////////////////////////
    fd = open(indevice, O_RDWR); // Open the serial port.
    if (fd < 0){

        perror("Port not available\n");
        return -1;
    }
    //test_ios(fd);
    struct termios termios_p;

    //int res = tcgetattr(fd, &termios_p);

    cfmakeraw(&termios_p);
    cfsetispeed(&termios_p, B19200);
    cfsetospeed(&termios_p, B19200);

    termios_p.c_cflag |= CRTSCTS;

    res = tcsetattr(fd, TCSANOW, &termios_p);
    if (res < 0){
        perror ("Port control error\n");
        return -1;
    }

    if (willwrite)
        send_hexfile(infile,fd);
    else
        read_eeprom(outfile,fd,memsize);

    close(fd);

    return 0;
}

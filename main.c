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

int main(int argc, char *argv[]) {

    int fd, sercmd, serstat;

    fd = open("/dev/ttyUSB0", O_RDONLY); // Open the serial port.

    /////////// RTS

    sercmd = TIOCM_RTS;

    printf("Setting the RTS pin.\n");
    ioctl(fd, TIOCMBIS, &sercmd); // Set the RTS pin.

    // Read the RTS pin status.
    ioctl(fd, TIOCMGET, &serstat);
    if (serstat & TIOCM_RTS)
        printf("RTS pin is set.\n");
    else
        printf("RTS pin is reset.\n");

    getchar(); // Wait for the return key before continuing.

    printf("Resetting the RTS pin.\n");
    ioctl(fd, TIOCMBIC, &sercmd); // Reset the RTS pin.

    // Read the RTS pin status.
    ioctl(fd, TIOCMGET, &serstat);
    if (serstat & TIOCM_RTS)
        printf("RTS pin is set.\n");
    else
        printf("RTS pin is reset.\n");

    getchar(); // Wait for the return key before continuing.

    /////////// DTR

    sercmd = TIOCM_DTR;

    printf("Setting the DTR pin.\n");
    ioctl(fd, TIOCMBIS, &sercmd); // Set the DTR pin.

    // Read the DTR pin status.
    ioctl(fd, TIOCMGET, &serstat);
    if (serstat & TIOCM_DTR)
        printf("DTR pin is set.\n");
    else
        printf("DTR pin is reset.\n");

    getchar(); // Wait for the return key before continuing.

    printf("Resetting the DTR pin.\n");
    ioctl(fd, TIOCMBIC, &sercmd); // Reset the DTR pin.

    // Read the DTR pin status.
    ioctl(fd, TIOCMGET, &serstat);
    if (serstat & TIOCM_DTR)
        printf("DTR pin is set.\n");
    else
        printf("DTR pin is reset.\n");

    for (;;){

        ioctl(fd, TIOCMGET, &serstat);
        printf("STATUS:%08x\n",serstat);
        if (getchar() == 27) break;
    }


    close(fd);

    return 0;
}

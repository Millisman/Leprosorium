#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

int ckps = 1;
int cams = 0;
int trig = 0;
int phase = 0;



void tick_process(void)
{

    printf("%d", ckps);

    switch (trig) {
        case 6: ckps = 0; break;
        case 7: ckps = 0; break;
        case 8: ckps = 1; break;
        case 9: ckps = 1; break;
        default: if (ckps == 1) ckps = 0; else ckps = 1; }

    if ((trig == 24) && (phase == 0))
        {
            printf("X");
        }


    trig++;
    if (trig > 119) {
        trig = 0;
        if (phase == 1) phase = 0; else phase = 1;
        printf("\n\n\n");
    };
}


int main (void)
{
    for(;;) {
        usleep(800);
        tick_process();
    };
}

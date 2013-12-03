/* diesel_rts.c */
#include <stdio.h>

void myputchar(ch)
    int ch;
{
    putc(ch, stdout);
    fflush(stdout);
}

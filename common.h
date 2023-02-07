#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/X.h>
#include <X11/extensions/XInput2.h>


#define MAX_MONITORS 8


typedef struct mon_info
{
   int x, y, w, h;
} mon_info_t;


void map_init(Display *dpy);
int map(int *x, int *y);


extern int wrap_x_;
extern int wrap_y_;



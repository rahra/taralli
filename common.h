#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/X.h>
#include <X11/extensions/XInput2.h>


#define MAX_MONITORS 8



typedef struct mon_extents
{
   int min, max;
} mon_extents_t;


typedef struct mon_info
{
   mon_extents_t me[2];
} mon_info_t;


int map_init(Display *dpy);
int map(int *x, int *y);


enum {DIMX, DIMY};

extern int wrap_x_;
extern int wrap_y_;



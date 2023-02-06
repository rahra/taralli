#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

#include "common.h"

#define MAX_MONITORS 8


typedef struct mon_info
{
   int x, y, w, h;
} mon_info_t;


static int mi_cnt_ = 0;
static mon_info_t mi_[MAX_MONITORS];


static inline int min(int a, int b)
{
   return a < b ? a : b;
}


static inline int max(int a, int b)
{
   return a > b ? a : b;
}


static int get_maxx_at_y(int y)
{
   int x = 0;

   for (int i = 0; i < mi_cnt_; i++)
      if (y >= mi_[i].y && y < mi_[i].y + mi_[i].h)
         x = max(x, mi_[i].x + mi_[i].w);

   return x - 1;
}


static int get_minx_at_y(int y)
{
   int x = INT_MAX;

   for (int i = 0; i < mi_cnt_; i++)
      if (y >= mi_[i].y && y < mi_[i].y + mi_[i].h)
         x = min(x, mi_[i].x);

   return x;
}


static int get_maxy_at_x(int x)
{
   int y = 0;

   for (int i = 0; i < mi_cnt_; i++)
      if (x >= mi_[i].x && x < mi_[i].x + mi_[i].w)
         y = max(y, mi_[i].y + mi_[i].h);

   return y - 1;
}


static int get_miny_at_x(int x)
{
   int y = INT_MAX;

   for (int i = 0; i < mi_cnt_; i++)
      if (x >= mi_[i].x && x < mi_[i].x + mi_[i].w)
         y = min(y, mi_[i].y);

   return y;
}


void map_init(Display *dpy)
{
   memset(mi_, 0, sizeof(mi_)); 

   // safety check
   if (dpy == NULL)
      return;

   Window window = DefaultRootWindow(dpy);
   XRRScreenResources *screenr = XRRGetScreenResources(dpy, window);

   for (int i = 0; i < screenr->noutput && mi_cnt_ < MAX_MONITORS; i++)
   {
      XRROutputInfo* out_info = XRRGetOutputInfo(dpy, screenr, screenr->outputs[i]);
      if (out_info != NULL && out_info->connection == RR_Connected)
      {
         XRRCrtcInfo* crt_info = XRRGetCrtcInfo(dpy, screenr, out_info->crtc);
         mi_[mi_cnt_].x = crt_info->x;
         mi_[mi_cnt_].y = crt_info->y;
         mi_[mi_cnt_].w = crt_info->width;
         mi_[mi_cnt_].h = crt_info->height;
         mi_cnt_++;
         XRRFreeCrtcInfo(crt_info);
      }
      XRRFreeOutputInfo(out_info);
   }

   XRRFreeScreenResources(screenr);
}


void map(int *x, int *y)
{
   int x0;

   x0 = *x;
   if (x0 == get_minx_at_y(*y))
      *x = get_maxx_at_y(*y);
   else if (x0 == get_maxx_at_y(*y))
      *x = get_minx_at_y(*y);

   if (*y == get_miny_at_x(x0))
      *y = get_maxy_at_x(x0);
   else if (*y == get_maxy_at_x(x0))
      *y = get_miny_at_x(x0);
}


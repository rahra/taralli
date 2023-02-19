#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

#include "common.h"


#define PDIM(x) ((x) ^ 1)


int wrap_x_ = 1;
int wrap_y_ = 1;

//! maximum rectangular extents of display
static mon_extents_t ext_[2];
//! lookup tables for each column/row
static mon_extents_t *me_[2] = {NULL, NULL};


static inline int min(int a, int b)
{
   return a < b ? a : b;
}


static inline int max(int a, int b)
{
   return a > b ? a : b;
}


static int get_max_at_pos(const mon_info_t *mi, int mi_cnt, int p0, int dim)
{
   int p = 0;

   for (int i = 0; i < mi_cnt; i++)
      if (p0 >= mi[i].me[PDIM(dim)].min && p0 < mi[i].me[PDIM(dim)].max)
         p = max(p, mi[i].me[dim].max);

   return p;
}


static int get_min_at_pos(const mon_info_t *mi, int mi_cnt, int p0, int dim)
{
   int p = INT_MAX;

   for (int i = 0; i < mi_cnt; i++)
      if (p0 >= mi[i].me[PDIM(dim)].min && p0 < mi[i].me[PDIM(dim)].max)
         p = min(p, mi[i].me[dim].min);

   return p;
}


static int get_monitor_info(Display *dpy, mon_info_t *mi, int mi_cnt)
{
   int cnt = 0;

   Window window = DefaultRootWindow(dpy);
   XRRScreenResources *screenr = XRRGetScreenResources(dpy, window);

   for (int i = 0; i < screenr->noutput && cnt < mi_cnt; i++)
   {
      XRROutputInfo* out_info = XRRGetOutputInfo(dpy, screenr, screenr->outputs[i]);
      if (out_info != NULL && out_info->connection == RR_Connected)
      {
         XRRCrtcInfo* crt_info = XRRGetCrtcInfo(dpy, screenr, out_info->crtc);
         mi[cnt].me[DIMX].min = crt_info->x;
         mi[cnt].me[DIMX].max = crt_info->x + crt_info->width;
         mi[cnt].me[DIMY].min = crt_info->y;
         mi[cnt].me[DIMY].max = crt_info->y + crt_info->height;
         cnt++;
         XRRFreeCrtcInfo(crt_info);
      }
      XRRFreeOutputInfo(out_info);
   }

   XRRFreeScreenResources(screenr);

   return cnt;
}


static void get_max_extents(const mon_info_t *mi, int mi_cnt, mon_extents_t *me, int dim)
{
   *me = mi[0].me[dim];
   for (int i = 1; i < mi_cnt; i++)
   {
      me->min = min(me->min, mi[i].me[dim].min);
      me->max = max(me->max, mi[i].me[dim].max);
   }
}


static void print_max_extents(const mon_extents_t me[2])
{
   printf("%d/%d-%d/%d\n", me[DIMX].min, me[DIMY].min, me[DIMX].max, me[DIMY].max);
}


static void print_monitors(const mon_info_t *mi, int cnt)
{
   for (int i = 0; i < cnt; i++)
      print_max_extents(mi[i].me);
}


int map_init(Display *dpy)
{
   mon_info_t mi[MAX_MONITORS];
   int cnt;

   // safety check
   if (dpy == NULL)
      return -1;

   cnt = get_monitor_info(dpy, mi, MAX_MONITORS);
   //print_monitors(mi, cnt);

   for (int i = 0; i < 2; i++)
   {
      get_max_extents(mi, cnt, &ext_[i], i);
      if ((me_[i] = realloc(me_[i], sizeof(*me_[i]) * ext_[i].max)) == NULL)
         return -1;
   }
   //print_max_extents(ext_);

   for (int i = 0; i <= 1; i++)
   {
      for (int j = ext_[i].min; j < ext_[i].max; j++)
      {
         me_[i][j].min = get_min_at_pos(mi, cnt, j, i ^ 1);
         me_[i][j].max = get_max_at_pos(mi, cnt, j, i ^ 1) - 1;
      }
   }

   return 0;
}


int map(int *x, int *y)
{
   int x0 = *x;
   int mod = 0;

   if (wrap_x_)
   {
      if (x0 == me_[DIMY][*y].min)
      {
         *x = me_[DIMY][*y].max;
         mod++;
      }
      else if (x0 == me_[DIMY][*y].max)
      {
         *x = me_[DIMY][*y].min;
         mod++;
      }
   }
   if (wrap_y_)
   {
      if (*y == me_[DIMX][x0].min)
      {
         *y = me_[DIMX][x0].max;
         mod++;
      }
      else if (*y == me_[DIMX][x0].max)
      {
         *y = me_[DIMX][x0].min;
         mod++;
      }
   }

   return mod;
}


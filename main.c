#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <X11/extensions/Xrandr.h>

#include "common.h"


static volatile sig_atomic_t sig_ = 0;


int background(void)
{
   switch(fork())
   {
      case -1:
         return -1;

      // all errors are ignored
      case 0:
         (void) umask(0);
         (void) setsid();
         (void) chdir("/");
         (void) freopen( "/dev/null", "r", stdin);
         (void) freopen( "/dev/null", "w", stdout);
         (void) freopen( "/dev/null", "w", stderr);
         return 0;

      default:
         exit(0);
   }
}


void usage(const char *s)
{
   printf(
         "usage: %s [options]\n"
         "\n"
         "   OPTIONS\n"
         "   -b .......... Automatically background process.\n"
         "   -h .......... Display this message.\n"
         "   -X .......... Do not wrap horizontally.\n"
         "   -Y .......... Do not wrap vertically.\n",
         s);
}


void sighup(int status)
{
   sig_ = status;
}


void install_sighandler(void)
{
   struct sigaction sa;

   memset(&sa, 0, sizeof(sa));
   sa.sa_handler = sighup;
   if (sigaction(SIGHUP, &sa, NULL) == -1)
      perror("sigaction() failed"), exit(1);
   if (sigaction(SIGALRM, &sa, NULL) == -1)
      perror("sigaction() failed"), exit(1);
}


void event_loop(void)
{
#define MONITOR_CHANGE
#ifdef MONITOR_CHANGE
   int have_rr, rr_event_base, rr_error_base;
#endif

   // Initialize X and the screen edge map.
   Display *dpy = XOpenDisplay(NULL);
   if (dpy == NULL)
      fprintf(stderr, "*** Cannot open display!\n"), exit(1);

   Window root = DefaultRootWindow(dpy);
   //map_init(dpy);

   // Get the XInput opcode.
   // (Variables starting with an underscore are not used.)
   int xi_opcode, _first_event, _first_error;
   if (! XQueryExtension(dpy, "XInputExtension", &xi_opcode,
                    &_first_event, &_first_error)) {
      puts("XInput is not available.");
      exit(1);
   }

   // Tell XInput to send us all RawMotion events.
   // (Normal Motion events are blocked by some windows.)
   unsigned char mask_bytes[XIMaskLen(XI_RawMotion)];
   memset(mask_bytes, 0, sizeof(mask_bytes));
   XISetMask(mask_bytes, XI_RawMotion);

   XIEventMask mask;
   mask.deviceid = XIAllMasterDevices;
   mask.mask_len = sizeof(mask_bytes);
   mask.mask = mask_bytes;
   XISelectEvents(dpy, root, &mask, 1);

#ifdef MONITOR_CHANGE
   // get screen layout changes 
   // code derived by https://cgit.freedesktop.org/xorg/app/xev/tree/xev.c
   have_rr = XRRQueryExtension(dpy, &rr_event_base, &rr_error_base);
   if (have_rr)
   {
      int rr_major, rr_minor;

      if (XRRQueryVersion(dpy, &rr_major, &rr_minor))
      {
         int rr_mask = RRScreenChangeNotifyMask | RRCrtcChangeNotifyMask | RROutputChangeNotifyMask | RROutputPropertyNotifyMask;

         if (rr_major == 1 && rr_minor <= 1)
            rr_mask &= ~(RRCrtcChangeNotifyMask | RROutputChangeNotifyMask | RROutputPropertyNotifyMask);
         XRRSelectInput(dpy, root, rr_mask);
      }
   }
#endif

   // Receive X events forever.
   for (sig_ = 1;;)
   {
      if (sig_)
      {
         sig_ = 0;
         map_init(dpy);
      }

      XEvent event;
      XNextEvent(dpy, &event);

#ifdef MONITOR_CHANGE
      if (have_rr && (event.type == rr_event_base + RRNotify || event.type == rr_event_base + RRScreenChangeNotify))
      {
         // wait 1 second to settle then reload at the next event
         alarm(1);
         printf("SCREEN CHANGE\n");
         continue;
      }
#endif

      if ((event.xcookie.type == GenericEvent) &&
         (event.xcookie.extension == xi_opcode) &&
         XGetEventData(dpy, &event.xcookie))
      {

         // On each RawMotion event, retrieve the pointer location
         // and move the pointer if necessary.
         if (event.xcookie.evtype == XI_RawMotion)
         {
            int x, y;
            int _win_x, _win_y;
            Window _root, _child;
            unsigned int _mask;
            if (XQueryPointer(dpy, root, &_root, &_child, &x, &y,
                          &_win_x, &_win_y, &_mask))
            {
               if (map(&x, &y))
                  XWarpPointer(dpy, None, root, 0, 0, 0, 0, x, y);
            }
         }

         // Clean up after XGetEventData.
         XFreeEventData(dpy, &event.xcookie);
      }

   }
}


int main(int argc, char **argv)
{
   int bg = 0;
   int c;

   while ((c = getopt(argc, argv, "bhvXY")) != -1)
      switch (c)
      {
         case 'b':
            bg = 1;
            break;

         case 'h':
            usage(argv[0]);
            exit(0);

         case 'v':
            printf(
                  "%s by Bernhard R. Fischer <bf@abenteuerland.at>.\n"
                  "Codebase originally written by Keegan McAllister, Autotools added by Josh Max.\n",
                  PACKAGE_STRING);
            exit(0);

         case 'X':
            wrap_x_ = 0;
            break;

         case 'Y':
            wrap_y_ = 0;
            break;
      }

   install_sighandler();

   if (bg)
      background();

   event_loop();

   return 0;
}


#include "x11_probe.h"

#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

struct x11_probe
{
    Display *dpy;
    Window root;
    Atom atom_active_window;
    Atom atom_wm_pid;
};

static int read_cardinal(x11_probe_t *probe, Window win, Atom prop, unsigned long *out)
{
    Atom type;
    int format;
    unsigned long nitems, bytes_after;
    unsigned char *data = NULL;
    int status;
    
    status = XGetWindowProperty(probe->dpy, win, prop, 0, 1, False, AnyPropertyType, &type, &format, &nitems, &bytes_after, &data);
    
    if (status != Success)
    {
        return -1;
    }
    
    if (data == NULL || nitems == 0 || format != 32)
    {
        XFree(data);
        return 1;
    }
    
    *out = *(unsigned long*)data;
    XFree(data);
    
    return 0;
}

x11_probe_t *x11_probe_create(void)
{
    x11_probe_t *probe = malloc(sizeof *probe);
    if (probe == NULL)
    {
        return NULL;
    }
    
    Display *dpy = XOpenDisplay(NULL);
    if (dpy == NULL)
    {
        free(probe);
        return NULL;
    }
    
    Window root = DefaultRootWindow(dpy);
    
    Atom atom_active_window = XInternAtom(dpy, "_NET_ACTIVE_WINDOW", True);
    Atom atom_wm_pid = XInternAtom(dpy, "_NET_WM_PID", True);
    
    if (atom_active_window == None || atom_wm_pid == None)
    {
        XCloseDisplay(dpy);
        free(probe);
        return NULL;
    }
    
    /* Assigner les valeurs */
    probe->dpy = dpy;
    probe->root = root;
    probe->atom_active_window = atom_active_window;
    probe->atom_wm_pid = atom_wm_pid;
    
    return probe;
}

void x11_probe_destroy(x11_probe_t *probe)
{
    if (probe == NULL)
    {
        return;
    }
    
    XCloseDisplay(probe->dpy);
    free(probe);
}

int x11_probe_active_pid(x11_probe_t *probe, pid_t *pid)
{
    unsigned long active_window;
    unsigned long wm_pid;
    int status;
    
    if (probe == NULL || pid == NULL)
    {
        return -1;
    }
    
    status = read_cardinal(probe, probe->root, probe->atom_active_window, &active_window);
    
    if (status != 0)
    {
        return status;
    }
    
    if (active_window == 0)
    {
        return 1;
    }
    
    status = read_cardinal(probe, active_window, probe->atom_wm_pid, &wm_pid);
    
    if (status != 0)
    {
        return status;
    }

    *pid = (pid_t)wm_pid;
    return 0;
}


// Simple X keyboard layout switcher

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>

#define VERSION "1.0.0"


struct LayoutNames {
    int count;
    char **names;
};


void help()
{
    puts("Simple X keyboard layout switcher v"VERSION);
    puts("xlayout           show current layout");
    puts("xlayout <name>    set layout");
    puts("xlayout -h        show this help");
    puts("xlayout -l        list possible layouts");
}



struct LayoutNames *get_layout_names(Display *display)
{
    struct LayoutNames *names;
    XkbDescRec desc;

    memset(&desc, 0, sizeof(XkbDescRec));
    desc.device_spec = XkbUseCoreKbd;
    XkbGetControls(display, XkbGroupsWrapMask, &desc);
    XkbGetNames(display, XkbGroupNamesMask, &desc);

    names = malloc(sizeof(struct LayoutNames));
    names->count = desc.ctrls->num_groups;
    names->names = malloc(names->count * sizeof(char*));
    XGetAtomNames(display, desc.names->groups, names->count, names->names);

    XkbFreeControls(&desc, XkbGroupsWrapMask, True);
    XkbFreeNames(&desc, XkbGroupNamesMask, True);

    return names;

}



void free_layout_names(struct LayoutNames *names)
{
    int i;
    for (i=0; i<names->count; i++)
    {
        XFree(names->names[i]);
    }
    free(names->names);
    free(names);
}



int list_layouts()
{
    struct LayoutNames *names;
    Display *display;
    int i;
    
    display = XOpenDisplay(NULL);
    if (!display)
    {
        fprintf(stderr, "Can't open default display\n");
        return 1;
    }

    names = get_layout_names(display);

    for (i=0; i<names->count; i++)
    {
        printf("%s\n", names->names[i]);
    }

    free_layout_names(names);
    XCloseDisplay(display);

    return 0;
}



int show_layout()
{
    struct LayoutNames *names;
    Display *display;
    XkbStateRec state;
    
    display = XOpenDisplay(NULL);
    if (!display)
    {
        fprintf(stderr, "Can't open default display\n");
        return 1;
    }

    XkbGetState(display, XkbUseCoreKbd, &state);

    names = get_layout_names(display);
    if (state.group >= names->count)
    {
        printf("Unknown\n");
    }
    else
    {
        printf("%s\n", names->names[state.group]);
    }

    free_layout_names(names);
    XCloseDisplay(display);

    return 0;
}



int set_layout(const char *name)
{
    struct LayoutNames *names;
    Display *display;
    int i, result;
    
    display = XOpenDisplay(NULL);
    if (!display)
    {
        fprintf(stderr, "Can't open default display\n");
        return 1;
    }

    names = get_layout_names(display);
    result = 1;
    for (i=0; i<names->count; i++)
    {
        if (!strcmp(name, names->names[i]))
        {
            XkbLockGroup(display, XkbUseCoreKbd, i);
            result = 0;
            break;
        }
    }

    if (result)
    {
        fprintf(stderr, "Unknown layout name: \"%s\"\n", name);
    }

    free_layout_names(names);
    XCloseDisplay(display);

    return result;
}



int main(int argc, char **argv)
{

    if (argc == 1)
    {
        return show_layout();
    }

    if (argc == 2)
    {
        if (!strcmp(argv[1], "-l"))
        {
            return list_layouts();
        }
        if (argv[1][0] != '-')
        {
            return set_layout(argv[1]);
        }
    }

    help();
    return 1;

}


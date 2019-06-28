#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <ui.h>

#define INI_IMPLEMENTATION
#include <ini.h>

#define DELAUNCHER_INI_FILENAME "delauncher.ini"

static int OnClose(uiWindow *w, void *data)
{
    uiQuit();
    return 1;
}

static bool LoadFile(const char* path, char** data, size_t& data_size)
{
    FILE* fp = fopen(path, "r");
    if (!fp) {
        printf("Could not open %s: %s\n", path, strerror(errno));
        return false;
    }

    // get file size
    fseek(fp, 0, SEEK_END);
    data_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (data_size == -1L) {
        fclose(fp);
        printf("Could not get size of %s: %s\n", path, strerror(errno));
        return false;
    }

    // allocate enough for nullterm
    data_size+=1;
    *data = (char*)malloc(data_size);

    // read file data and make sure we have a nullterm
    fread(*data, 1, data_size - 1, fp);
    (*data)[data_size - 1] = '\0';
    fclose(fp);

    return true;
}

int main(void)
{
    const char* window_title = "Unknown";

    // load ini data
    char* data = NULL;
    size_t data_size = 0;
    if (!LoadFile(DELAUNCHER_INI_FILENAME, &data, data_size)) {
        printf("Could not load %s file.\n", DELAUNCHER_INI_FILENAME);
        return 1;
    }

    // parse ini data
    ini_t* ini = ini_load(data, NULL);
    free(data);

    int c = ini_section_count( ini );
    for (int i = 0; i < c; ++i)
    {
        const char* section_name = ini_section_name(ini, i);
        printf("section: %s\n", section_name);

        // global section
        if (i == INI_GLOBAL_SECTION)
        {
            printf("found global section\n");
            int prop_index = ini_find_property(ini, INI_GLOBAL_SECTION, "window_title", 0);
            printf("window_title prop_index: %d\n", prop_index);
            if (prop_index != INI_NOT_FOUND) {
                window_title = ini_property_value(ini, INI_GLOBAL_SECTION, prop_index);
                printf("window_title: %s\n", window_title);
            }
        } else {
            // nop
        }
    }
    // printf("sections: %d\n", );

    // cleanup

    uiInitOptions o;
    memset(&o, 0, sizeof (uiInitOptions));
    const char *err = uiInit(&o);
    if (err != NULL) {
        fprintf(stderr, "error initializing ui: %s\n", err);
        uiFreeInitError(err);
        return 1;
    }

    uiWindow *w = uiNewWindow(window_title, 320, 240, 0);
    uiWindowSetMargined(w, 1);
    uiWindowOnClosing(w, OnClose, NULL);
    uiControlShow(uiControl(w));
    uiMain();

    ini_destroy(ini);
    return 0;
}

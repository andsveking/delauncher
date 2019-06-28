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
    // load
    char* data = NULL;
    size_t data_size = 0;
    if (!LoadFile(DELAUNCHER_INI_FILENAME, &data, data_size)) {
        printf("Could not load %s file.\n", DELAUNCHER_INI_FILENAME);
        return 1;
    }

    ini_t* ini = ini_load(data, NULL);
    free(data);

    printf("sections: %d\n", ini_section_count( ini ));
    ini_destroy(ini);

    uiInitOptions o;
    memset(&o, 0, sizeof (uiInitOptions));
    const char *err = uiInit(&o);
    if (err != NULL) {
        fprintf(stderr, "error initializing ui: %s\n", err);
        uiFreeInitError(err);
        return 1;
    }

    uiWindow *w = uiNewWindow("Date / Time", 320, 240, 0);
    uiWindowSetMargined(w, 1);
    uiWindowOnClosing(w, OnClose, NULL);
    uiControlShow(uiControl(w));
    uiMain();
    return 0;
}

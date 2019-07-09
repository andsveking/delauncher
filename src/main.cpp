#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <ui.h>
#include <xxhash.h>
#include <xxh64.hpp>

#define INI_IMPLEMENTATION
#include <ini.h>

#define DELAUNCHER_INI_FILENAME "delauncher.ini"
#define DELAUNCHER_HASH_SEED 14
#define DELAUNCHER_TRUE_H CT_HASH("true")

static void OnButtonClose(uiButton *b, void *data)
{
    uiQuit();
}

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

#define CT_HASH(str) \
    xxh64::hash(str, sizeof(str)-1, DELAUNCHER_HASH_SEED)

enum ControlType
{
    CONTROL_TYPE_UNKNOWN  = 0,
    CONTROL_TYPE_CHECKBOX = CT_HASH("checkbox"),
    CONTROL_TYPE_DROPDOWN = CT_HASH("dropdown")
};

typedef struct Control Control;

struct Control
{
    ControlType m_Type;
    Control* m_Next;

    Control() { m_Next = 0x0; }
};

int main(void)
{
    // Small struct just to hold window creation params,
    // instead of having a bunch of "global" vars laying around.
    struct WinProps
    {
        const char* m_Title;
        int         m_Width;
        int         m_Height;
        WinProps(const char* title, int width, int height)
         : m_Title(title),
           m_Width(width),
           m_Height(height) {};

    } win_props("Unknown", 320, 240);

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

#define INI_GET_STRING(_out, _section, _property, _default) \
    { \
        int prop_index = ini_find_property(ini, _section, _property, 0); \
        if (prop_index != INI_NOT_FOUND) { \
            _out = ini_property_value(ini, _section, prop_index); \
        } else { \
            _out = _default; \
        } \
    }

#define INI_GET_INT(_out, _section, _property, _default) \
    { \
        int prop_index = ini_find_property(ini, _section, _property, 0); \
        if (prop_index != INI_NOT_FOUND) { \
            _out = strtol(ini_property_value(ini, _section, prop_index), 0, 10); \
        } else { \
            _out = _default; \
        } \
    }

#define INI_GET_HASH(_out, _section, _property, _default) \
    { \
        int prop_index = ini_find_property(ini, _section, _property, 0); \
        if (prop_index != INI_NOT_FOUND) { \
            const char* str_out = ini_property_value(ini, _section, prop_index); \
            _out = XXH64(str_out, strlen(str_out), DELAUNCHER_HASH_SEED); \
        } else { \
            _out = _default; \
        } \
    }

#define INI_GET_BOOL(_out, _section, _property, _default) \
    { \
        int prop_index = ini_find_property(ini, _section, _property, 0); \
        if (prop_index != INI_NOT_FOUND) { \
            const char* str_out = ini_property_value(ini, _section, prop_index); \
            _out = DELAUNCHER_TRUE_H == XXH64(str_out, strlen(str_out), DELAUNCHER_HASH_SEED); \
        } else { \
            _out = _default; \
        } \
    }


    // get generic window setup properties
    INI_GET_STRING(win_props.m_Title, INI_GLOBAL_SECTION, "window_title", "Default");
    INI_GET_INT(win_props.m_Width, INI_GLOBAL_SECTION, "window_width", 640);
    INI_GET_INT(win_props.m_Height, INI_GLOBAL_SECTION, "window_height", 320);

    // init libui
    uiInitOptions o;
    memset(&o, 0, sizeof (uiInitOptions));
    const char *err = uiInit(&o);
    if (err != NULL) {
        fprintf(stderr, "error initializing ui: %s\n", err);
        uiFreeInitError(err);
        return 1;
    }

    // open window
    uiWindow *w = uiNewWindow(win_props.m_Title, win_props.m_Width, win_props.m_Height, 0);
    uiWindowSetMargined(w, 1);
    uiWindowOnClosing(w, OnClose, NULL);

    // setup simple 2 column grid (c0: label, c1: control)
    uiGrid* grid = uiNewGrid();
    uiGridSetPadded(grid, 1);
    uiWindowSetChild(w, uiControl(grid));

    // TODO include header image?

    // go through ini properties and create ui controls
    int c = ini_section_count( ini );
    int row = 0;
    for (int i = 0; i < c; ++i)
    {
        // must ignore global section (used for internal stuff)
        if (i != INI_GLOBAL_SECTION)
        {
            const char* section_name = ini_section_name(ini, i);
            uint64_t section_type = 0x0;
            INI_GET_HASH(section_type, i, "type", CONTROL_TYPE_UNKNOWN);

            const char* section_label = 0x0;
            INI_GET_STRING(section_label, i, "label", section_name);

            // section label
            uiLabel *label = uiNewLabel(section_label);
            uiGridAppend(grid, uiControl(label),
                0, row, 1, 1,
                0, uiAlignFill, 0, uiAlignFill);

            switch (section_type)
            {
                case CONTROL_TYPE_CHECKBOX:
                    {
                        uiCheckbox* checkbox = uiNewCheckbox("");
                        uiGridAppend(grid, uiControl(checkbox),
                            1, row, 1, 1,
                            1, uiAlignStart, 0, uiAlignFill);

                        bool checked = false;
                        INI_GET_BOOL(checked, i, "default", false);
                        uiCheckboxSetChecked(checkbox, checked);
                    }
                    break;
                case CONTROL_TYPE_DROPDOWN:
                    {
                        uiCombobox* combobox = uiNewCombobox();
                        uiGridAppend(grid, uiControl(combobox),
                            1, row, 1, 1,
                            1, uiAlignFill, 0, uiAlignFill);

                        const char* choices = 0x0;
                        INI_GET_STRING(choices, i, "values", "");

                        // loop through values
                        char c = 0x0;
                        const char* p = choices;
                        char current[256] = {0};
                        while (true)
                        {
                            c = *choices;

                            if (!c || c == ',') {
                                size_t len = choices-p;
                                assert(len < sizeof(current));

                                strncpy(current, p, len);
                                current[len] = '\0';

                                uiComboboxAppend(combobox, current);
                                p = choices+1;
                            }

                            if (!c) {
                                break;
                            }

                            choices++;
                        }

                        int selection_default = 0;
                        INI_GET_INT(selection_default, i, "default", 0);
                        uiComboboxSetSelected(combobox, selection_default);
                    }
                    break;
                default:
                    printf("Unknown control type for %s\n", section_name);
                    break;
            }

            row += 1;
        }
    }

    // buttons
    uiBox* button_box = uiNewHorizontalBox();
    uiGridAppend(grid, uiControl(button_box),
        0, row, 2, 1,
        1, uiAlignEnd, 0, uiAlignFill);
    uiButton* run_button = uiNewButton("Run");
    uiButton* close_button = uiNewButton("Close");
    uiButtonOnClicked(close_button, OnButtonClose, 0x0);
    uiBoxAppend(button_box, uiControl(run_button), 0);
    uiBoxAppend(button_box, uiControl(close_button), 0);


    uiControlShow(uiControl(w));
    uiMain();

#undef INI_GET_STRING
#undef INI_GET_INT
#undef INI_GET_HASH
#undef INI_GET_BOOL

    ini_destroy(ini);
    return 0;
}

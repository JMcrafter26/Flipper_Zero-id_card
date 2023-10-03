#include "gui/canvas.h"
#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdint.h>
#include <stdlib.h>

// To do: Get the name, number and email from a file called "apps_data.txt"

// include the file string.h to use the function strcpy
#include <storage/storage.h>
#include <dolphin/dolphin.h>



#include "id_card_icons.h"

// Old code
// #define NAME "John DOE"
// #define NUMBER "06 12 34 56 78"
// #define EMAIL "john.doe@example.com"

// load_data() function
string load_data();

// Get the desired data from the file
string data = load_data();
string name = strtok(data, "\n");
string number = strtok(NULL, "\n");
string email = strtok(NULL, "\n");


// Get the name
#define NAME name
#define NUMBER number
#define EMAIL email


#define TAG "Id Card"

typedef struct {
    FuriMessageQueue* input_queue;
    ViewPort* view_port;
    Gui* gui;
} Id_card;

void draw_callback(Canvas* canvas, void* context) {
    UNUSED(context);

    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);

    canvas_draw_str_aligned(canvas, 1, 5, AlignLeft, AlignTop, "Name: ");

    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontSecondary);

    canvas_draw_str_aligned(canvas, 33, 5, AlignLeft, AlignTop, NAME);

    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);

    canvas_draw_str_aligned(canvas, 1, 20, AlignLeft, AlignTop, "N: ");

    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontSecondary);

    canvas_draw_str_aligned(canvas, 15, 20, AlignLeft, AlignTop, NUMBER);

    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);

    canvas_draw_str_aligned(canvas, 1, 35, AlignLeft, AlignTop, "EMAIL: ");

    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontSecondary);

    canvas_draw_str_aligned(canvas, 5, 45, AlignLeft, AlignTop, EMAIL);

    canvas_draw_icon(canvas, 95, 5, &I_icon_30x30);
}

void input_callback(InputEvent* event, void* context) {
    Id_card* app = context;
    furi_message_queue_put(app->input_queue, event, 0);
}

int32_t id_card_app(void* p) {
    UNUSED(p);

    Id_card app;

    // Alloc
    app.view_port = view_port_alloc();

    // Callbacks
    view_port_draw_callback_set(app.view_port, draw_callback, &app);
    view_port_input_callback_set(app.view_port, input_callback, &app);

    // GUI
    app.gui = furi_record_open("gui");
    gui_add_view_port(app.gui, app.view_port, GuiLayerFullscreen);

    // Input handling
    InputEvent input;
    app.input_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    uint8_t exit_loop = 0;
    while(1) {
        furi_check(
            furi_message_queue_get(app.input_queue, &input, FuriWaitForever) == FuriStatusOk);
        FURI_LOG_D(TAG, "In the while!!!");

        switch(input.key) {
        case InputKeyLeft:
        case InputKeyRight:
        case InputKeyOk:
        case InputKeyUp:
        case InputKeyDown:
        case InputKeyBack:
            exit_loop = 1;
            break;
        default:
            break;
        }

        if(exit_loop) {
            break;
        }

        view_port_update(app.view_port);
    }

    // Free structures
    view_port_enabled_set(app.view_port, false);
    gui_remove_view_port(app.gui, app.view_port);
    furi_record_close("gui");
    view_port_free(app.view_port);

    return 0;
}

string bool load_data() {
    Storage* storage = furi_record_open("storage");

        File* file = storage_file_alloc(storage);
    uint16_t bytes_readed = 0;
    if(storage_file_open(file, "apps_data.txt", StorageFileModeRead) == StorageStatusOk) {
        char buffer[256];
        storage_file_read(file, buffer, 256, &bytes_readed);
        buffer[bytes_readed] = '\0';
        storage_file_close(file);
        storage_file_free(file);
        furi_record_close("storage");
        return buffer;
    }
    else {
        // if the file doesn't exist, create it
        File* file = storage_file_alloc(storage);
        if(storage_file_open(file, "apps_data.txt", StorageFileModeWrite) == StorageStatusOk) {
            storage_file_write(file, "John DOE\n06 12 34 56 78\njohn.doe@example.com", 256, &bytes_readed);
            storage_file_close(file);
            storage_file_free(file);
            furi_record_close("storage");
            return "John DOE\n06 12 34 56 78\njohn.doe@example.com";

    }

}
}
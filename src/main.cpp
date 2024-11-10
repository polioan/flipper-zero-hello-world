#include <furi.h>
#include <gui/gui.h>

constexpr uint32_t msg_count = 8UL;

constexpr uint32_t timeout = 100UL;

static void draw_callback(Canvas* canvas, void* ctx) {
    UNUSED(ctx);

    canvas_clear(canvas);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(
        canvas,
        canvas_width(canvas) / 2,
        canvas_height(canvas) / 2,
        AlignCenter,
        AlignCenter,
        "Hello world!");
}

static void input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);

    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

extern "C" {
int32_t polioan_hello_world_main(void* p) {
    UNUSED(p);

    FuriMessageQueue* event_queue = furi_message_queue_alloc(msg_count, sizeof(InputEvent));

    ViewPort* view_port = view_port_alloc();

    view_port_draw_callback_set(view_port, draw_callback, nullptr);

    view_port_input_callback_set(view_port, input_callback, event_queue);

    Gui* gui = static_cast<Gui*>(furi_record_open(RECORD_GUI));

    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    InputEvent event;

    while(true) {
        if(furi_message_queue_get(event_queue, &event, timeout) == FuriStatusOk &&
           event.key == InputKeyBack) {
            break;
        }
        view_port_update(view_port);
    }

    view_port_enabled_set(view_port, false);

    gui_remove_view_port(gui, view_port);

    view_port_free(view_port);

    furi_message_queue_free(event_queue);

    furi_record_close(RECORD_GUI);

    return 0;
}
}

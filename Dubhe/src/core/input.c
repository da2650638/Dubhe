#include "input.h"

#include "core/asserts.h"
#include "core/dmemory.h"
#include "core/logger.h"
#include "core/event.h"

typedef struct keyboard_state{
    b8 keys[256];
} keyboard_state;

typedef struct mousebutton_state{
    i16 x;
    i16 y;
    b8 buttons[MOUSEBUTTONS_COUNT];
} mousebutton_state;

typedef struct input_state{
    keyboard_state keyboard_current;
    keyboard_state keyboard_previous;
    mousebutton_state mousebutton_current;
    mousebutton_state mousebutton_previous;
} input_state;

// Input internal state, not exposed to outside.
static b8 initialized = FALSE;
static input_state state;

void input_initialize()
{
    if(initialized)
    {
        return;
    }
    dzero_memory(&state, sizeof(state));
    initialized = TRUE;
    DINFO("Input subsystem initialized.");
}

void input_shutdown()
{
    // TODO: Add shutdown routines when needed.
    initialized = FALSE;
}

void input_update(f64 delta_time)
{
    if(!initialized)
    {
        return;
    }

    // Copy current states to previous states.
    dcopy_memory(&state.keyboard_previous, &state.keyboard_current, sizeof(state.keyboard_current));
    dcopy_memory(&state.mousebutton_previous, &state.mousebutton_current, sizeof(state.mousebutton_current));
}

void input_process_key(key_code key, b8 pressed)
{
    if(state.keyboard_current.keys[key] != pressed)
    {
        state.keyboard_current.keys[key] = pressed;

        event_context data;
        data.data.u16[0] = key;
        event_fire(pressed ? EVENT_CODE_KEY_PRESSED : EVENT_CODE_KEY_RELEASED,
        0,
        data);
    }
}

void input_process_button(mouse_button button, b8 pressed)
{
    if(state.mousebutton_current.buttons[button] != pressed)
    {
        state.mousebutton_current.buttons[button] = pressed;

        event_context data;
        data.data.u16[0] = button;
        event_fire(pressed ? EVENT_CODE_BUTTON_PRESSED : EVENT_CODE_BUTTON_RELEASED,
        0,
        data);
    }
}

void input_process_mouse_move(i16 x, i16 y)
{
    if(state.mousebutton_current.x != x || state.mousebutton_current.y != y)
    {
        state.mousebutton_current.x = x;
        state.mousebutton_current.y = y;

        event_context data;
        data.data.u16[0] = x;
        data.data.u16[1] = y;
        event_fire(EVENT_CODE_MOUSE_MOVE, 0, data);
    }
}

void input_process_mouse_wheel(i8 z_delta)
{
    event_context data;
    data.data.u8[0] = z_delta;
    event_fire(EVENT_CODE_MOUSE_WHEEL, 0, data);
}

b8 input_is_key_down(key_code key)
{
    if(!initialized)
    {
        return FALSE;
    }
    return state.keyboard_current.keys[key] == TRUE;
}

b8 input_was_key_down(key_code key)
{
    if(!initialized)
    {
        return FALSE;
    }
    return state.keyboard_previous.keys[key] == TRUE;
}

b8 input_is_key_up(key_code key)
{
    if(!initialized)
    {
        return FALSE;
    }
    return state.keyboard_current.keys[key] == FALSE;
}

b8 input_was_key_up(key_code key)
{
    if(!initialized)
    {
        return FALSE;
    }
    return state.keyboard_previous.keys[key] == FALSE;
}

b8 input_is_button_down(mouse_button button)
{
    if(!initialized)
    {
        return FALSE;
    }
    return state.mousebutton_current.buttons[button] == TRUE;
}

b8 input_was_button_down(mouse_button button)
{
    if(!initialized)
    {
        return FALSE;
    }
    return state.mousebutton_previous.buttons[button] == TRUE;
}

b8 input_is_button_up(mouse_button button)
{
    if(!initialized)
    {
        return FALSE;
    }
    return state.mousebutton_current.buttons[button] == FALSE;
}

b8 input_was_button_up(mouse_button button)
{
    if(!initialized)
    {
        return FALSE;
    }
    return state.mousebutton_previous.buttons[button] == FALSE;
}

void input_get_mouse_pos(i32* x, i32* y)
{
    if(!initialized)
    {
        *x = 0;
        *y = 0;
    }
    *x = state.mousebutton_current.x;
    *y = state.mousebutton_current.y;
}

void input_get_prev_mouse_pos(i32* x, i32* y)
{
    if(!initialized)
    {
        *x = 0;
        *y = 0;
    }
    *x = state.mousebutton_previous.x;
    *y = state.mousebutton_previous.y;
}
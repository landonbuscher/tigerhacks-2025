#include "config.h"

Rectangle background_rectangle = {0, 0, SCREEN_WIDTH*2, SCREEN_HEIGHT*2};
char announcement_status[256] = "";

void set_background_rectangle(Rectangle r) {
    background_rectangle = r;
}

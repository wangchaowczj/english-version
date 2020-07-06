#include "U8glib.h"

U8GLIB_SSD1306_128X32 u8g(U8G_I2C_OPT_NONE);

void draw(void) {
  u8g.setFont(u8g_font_unifont);
  u8g.drawStr( 2, 22, "Hello World!");
}

void setup(void) {
}

void loop(void) {
  u8g.firstPage();  
  do {
    draw();
  } while( u8g.nextPage() );
  
  // rebuild the picture after some delay
  delay(500);
}

#include <Arduino.h>
#include "Display_ST7701.h"
#include "TCA9554PWR.h"
#include <lvgl.h>
#include <ui/ui.h>

static const uint32_t screenWidth = 480;
static const uint32_t screenHeight = 480;

// const size_t BUFFER_PIXELS = 480 * 480;  // Total pixels per buffer
// const size_t BUFFER_SIZE_BYTES = BUFFER_PIXELS * sizeof(lv_color_t)/2;

enum { SCREENBUFFER_SIZE_PIXELS = screenWidth * screenHeight / 10 };
static lv_color_t buf [SCREENBUFFER_SIZE_PIXELS];

void* allocate_psram(size_t size) {
  void* ptr = ps_malloc(size);
  return ptr;
}

#if LV_USE_LOG != 0
/* Serial debugging */
void my_print(lv_log_level_t level, const char * buf)
{
    Serial.printf(buf);
    Serial.flush();
}
#endif

/* Display flushing */
void my_disp_flush (lv_display_t *disp, const lv_area_t *area, uint8_t *pixelmap)
{
  uint32_t w = ( area->x2 - area->x1 + 1 );
  uint32_t h = ( area->y2 - area->y1 + 1 );

  // if (LV_COLOR_16_SWAP) {
  //     size_t len = lv_area_get_size( area );
  //     lv_draw_sw_rgb565_swap( pixelmap, len );
  // }

  LCD_addWindow(area->x1, area->y1, area->x2, area->y2, (uint8_t *) pixelmap);

  lv_disp_flush_ready( disp );
}

/*Set tick routine needed for LVGL internal timings*/
static uint32_t my_tick_get_cb (void) { return millis(); }

void setup() {
  Serial.begin(115200);
  I2C_Init();
  delay(120);                //ms
  TCA9554PWR_Init(0x00); 
  Set_EXIO(EXIO_PIN8,Low);
  LCD_Init();

  Serial.print("Heap size: ");
  Serial.println(ESP.getHeapSize());
  Serial.print("Heap free size: ");
  Serial.println(ESP.getFreeHeap());
  Serial.print("PSRAM size: ");
  Serial.println(ESP.getPsramSize());
  Serial.print("PSRAM free size: ");
  Serial.println(ESP.getFreePsram());

  #if LV_USE_LOG != 0
  lv_log_register_print_cb( my_print ); /* register print function for debugging */
#endif

  lv_init();

  // lv_color_t* buf1 = (lv_color_t*)allocate_psram(BUFFER_SIZE_BYTES);
  // lv_color_t* buf2 = (lv_color_t*)allocate_psram(BUFFER_SIZE_BYTES);

  // if(!buf1 || !buf2) {
  //   Serial.println("Error allocating PSRAM buffer");
  //   while(1) { delay(100); }
  // }

  static lv_disp_t* disp;
  disp = lv_display_create( screenWidth, screenHeight );
  lv_display_set_buffers( disp, buf, NULL, SCREENBUFFER_SIZE_PIXELS * sizeof(lv_color_t), LV_DISPLAY_RENDER_MODE_PARTIAL );
  lv_display_set_flush_cb( disp, my_disp_flush );

  // lv_obj_t *label = lv_label_create( lv_screen_active() );
  // lv_label_set_text( label, "Hello Muzafar, I'm LVGL!" );
  // lv_obj_align( label, LV_ALIGN_CENTER, 0, 0 );
  ui_init();

  printf("Setup done\n");

  Serial.print("Heap size: ");
  Serial.println(ESP.getHeapSize());
  Serial.print("Heap free size: ");
  Serial.println(ESP.getFreeHeap());
  Serial.print("PSRAM size: ");
  Serial.println(ESP.getPsramSize());
  Serial.print("PSRAM free size: ");
  Serial.println(ESP.getFreePsram());
}

void loop() {
  lv_timer_handler(); 
  delay(5);
}


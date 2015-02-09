#include <display.h>
#include <assert.h>
#include <stdlib.h>

#define min(a,b) ((a)>(b)?(b):(a))

WINDOW *main_screen;

/* private functions */
void init_colors(void);

void init_screen(void){
  main_screen = initscr();
  keypad(stdscr, TRUE);           // keypad enabled
  (void) nodelay(main_screen, 1); // no blocking
  (void) noecho();                // don't print to screen
  (void) nonl();                  // no new lines
  init_colors();
}

/*
 * crazy shifting is to set up every color 
 */
void init_colors(void) {
  int i;
  start_color();
  if (COLORS < 255) {
    for (i = 0; i < (1 << 8); i ++) {
      init_pair(i, 255, 0); // 0 --> i if you want pure blocks, otherwise ascii
    }
  } else {
    for (i = 0; i < (1 << 8); i ++) {
      init_pair(i, i, 0); // 0 --> i if you want pure blocks, otherwise ascii
    }
  }
  return;
}

void end_screen(void) {
  endwin();
}

/* allow us to directly map to the 216 colors ncurses makes available */
static inline int get_color(int r, int g, int b) {
  return 16+r/48*36+g/48*6+b/48;
}

const char ascii_values[] = " ..::--==+++***###%%%%%%%%@@@@@@@";

int draw_image(char *data, int width, int height, int step, int channels) {
  char ascii_image[width*height];
  int y, x;
  unsigned char b, g, r;
  int offset = 0;
  int intensity;
  for (y = 0; y < height && y < LINES; y += 4) {
    for (x = 0; x < width && x < COLS; x++) {
      b = data[step * y + x * channels] + offset;
      g = data[step * y + x * channels + 1] + offset;
      r = data[step * y + x * channels + 2] + offset;
      //intensity = abs((int)(0.2126*r + 0.7152*g + 0.0722*b));
      intensity = (sizeof(ascii_values) - 1) * ((r/255.0 + g/255.0 + b/255.0) / 3);
      ascii_image[y * width + x] = ascii_values[intensity];
      int color = get_color(r, g, b);
      if (COLORS < 255) {
        color = 0;
      }
      mvaddch(y, x, ascii_image[y * width + x]|COLOR_PAIR(color));
    }
  } 

  refresh();
  return 0;
}
braille_values =
int draw_image_braille(char *data, int width, int height, int step, int channels) {
  char ascii_image[width*height];
  float threshold = 0.8;
  float intensity;
  int y, x; // Index of top left pixel of block
  int col, row; //Index of pixel in block
  int block_height = 4;
  int block_width = 2
  int color_offset = 0;
  unsigned char b, g, r;
  unsigned int block_b, block_g, block_r;
  unsigned char block_binary = 0b0;
  for (y = 0; y < height - block_height && y < LINES - block_height; y+= block_height) {
    for (x = 0; x < width - block_width && x < LINES - block_width; x+= block_width) {
      // (x, y) is now the index of top left of 2x4 pixel block
      block_binary = 0b0;
      block_b = block_g = block_r = 0;
      for (col = 0; col < block_height; col++) {
        for (row = 0; row < block_width; row++) {
          b = data[step * (col + y) + (row + x) * channels] + color_offset;
          g = data[step * y + (row + x) * channels + 1] + color_offset;
          r = data[step * y + (row + x) * channels + 2] + color_offset;
          intensity = (r/255.0 + g/255.0 + b/255.0) / 3;
          if (intensity > threshold) {
            block_binary |= 1 << ((col*block_height) + row);
          }
          block_b += b;
          block_g += g;
          block_r += r;
        }
      }
      // Average of block's rgb
      int color = get_color(
        (int) block_r / (block_width*block_height), 
        (int) block_g / (block_width*block_height),
        (int) block_b / (block_width*block_height));
      if (COLORS < 255) {
        color = 0;
      }
      //mvaddch(y, x, ascii_image[y * width + x]|COLOR_PAIR(color));
      mvaddstr(1, 0, "\xe2\x9c\x93");
    }
  } 

  refresh();
  return 0;
}

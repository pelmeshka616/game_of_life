
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

#define WIDTH 80
#define HEIGHT 25

unsigned char **alloc_field();
void free_field(unsigned char **f);
int load_pattern(unsigned char **field, int number);
int count_neighbors(unsigned char **field, int x, int y);
void next_generation(unsigned char **field, unsigned char **next_field);
void draw(unsigned char **field, int delay);
int update_delay(int delay, int key);
void run_game(unsigned char **field, unsigned char **next_field);

int main() {
  int pattern_number = 0;
  printf("Enter file number (1-5): ");
  scanf("%d", &pattern_number);

  if (pattern_number < 1 || pattern_number > 5) {
    printf("Invalid number\n");
    return 1;
  }

  unsigned char **field = alloc_field();
  unsigned char **next_field = alloc_field();
  if (field == NULL || next_field == NULL) {
    printf("Memory error\n");
    free_field(field);
    free_field(next_field);
    return 1;
  }

  if (load_pattern(field, pattern_number) == 0) {
    printf("File start_pattern%d.txt not found\n", pattern_number);
    free_field(field);
    free_field(next_field);
    return 1;
  }

  run_game(field, next_field);

  free_field(field);
  free_field(next_field);
  return 0;
}

unsigned char **alloc_field() {
  unsigned char **f = malloc(HEIGHT * sizeof(unsigned char *));
  for (int y = 0; y < HEIGHT; y++) {
    f[y] = calloc(WIDTH, 1);
  }
  return f;
}

void free_field(unsigned char **f) {
  for (int y = 0; y < HEIGHT; y++) {
    free(f[y]);
  }
  free(f);
}

int load_pattern(unsigned char **field, int number) {
  char *files[5] = {"start_pattern1.txt", "start_pattern2.txt",
                    "start_pattern3.txt", "start_pattern4.txt",
                    "start_pattern5.txt"};

  FILE *file = fopen(files[number - 1], "r");
  if (file == NULL) {
    return 0;
  }

  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      int c = fgetc(file);
      if (c == EOF) {
        fclose(file);
        return 1;
      }
      if (c == '1') {
        field[y][x] = 1;
      } else if (c == '0') {
        field[y][x] = 0;
      } else
        x--;
    }
  }

  fclose(file);
  return 1;
}

int count_neighbors(unsigned char **field, int x, int y) {
  int sum = 0;
  for (int dy = -1; dy <= 1; dy++) {
    for (int dx = -1; dx <= 1; dx++) {
      if (dx != 0 || dy != 0) {
        int nx = (x + dx + WIDTH) % WIDTH;
        int ny = (y + dy + HEIGHT) % HEIGHT;
        sum += field[ny][nx];
      }
    }
  }
  return sum;
}

void next_generation(unsigned char **field, unsigned char **next_field) {
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      int n = count_neighbors(field, x, y);

      if (field[y][x] == 1) {
        if (n == 2 || n == 3)
          next_field[y][x] = 1;
        else
          next_field[y][x] = 0;
      } else {
        if (n == 3)
          next_field[y][x] = 1;
        else
          next_field[y][x] = 0;
      }
    }
  }

  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      field[y][x] = next_field[y][x];
    }
  }
}

void draw(unsigned char **field, int delay) {
  clear();

  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      if (field[y][x] == 1) {
        printw("#");
      } else {
        printw(" ");
      }
    }
    printw("\n");
  }

  printw("Delay: %d ms | A - faster, Z - slower, Space - quit\n", delay);
  refresh();
}

int update_delay(int delay, int key) {
  if (key == 'a' || key == 'A') {
    delay -= 20;
    if (delay < 20) {
      delay = 20;
    }
  } else if (key == 'z' || key == 'Z') {
    delay += 20;
    if (delay > 1000)
      delay = 1000;
  }
  return delay;
}

void run_game(unsigned char **field, unsigned char **next_field) {
  initscr();
  cbreak();
  noecho();
  curs_set(0);

  int delay = 100;
  int running = 1;
  while (running) {
    draw(field, delay);
    next_generation(field, next_field);

    timeout(delay);
    int key = getch();

    if (key == ' ') {
      running = 0;
    } else {
      delay = update_delay(delay, key);
    }
  }

  endwin();
}


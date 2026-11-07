#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include <ncurses.h>

#define MAX_LEN 1024

// TODO:
//      Fix the draw_func function to make it calculate x and y values better YUH!
//      Add an option to parse equations from the command line?
//      Current operations i want to support: + - * / () ^ log 


// Each possible braille byte in order
const char *braille[256] = 
{
    // No bottom
    "⠀", "⠁", "⠂", "⠃", "⠄", "⠅", "⠆", "⠇", "⠈", "⠉", "⠊", "⠋", "⠌", "⠍", "⠎", "⠏", 
    "⠐", "⠑", "⠒", "⠓", "⠔", "⠕", "⠖", "⠗", "⠘", "⠙", "⠚", "⠛", "⠜", "⠝", "⠞", "⠟", 
    "⠠", "⠡", "⠢", "⠣", "⠤", "⠥", "⠦", "⠧", "⠨", "⠩", "⠪", "⠫", "⠬", "⠭", "⠮", "⠯", 
    "⠰", "⠱", "⠲", "⠳", "⠴", "⠵", "⠶", "⠷", "⠸", "⠹", "⠺", "⠻", "⠼", "⠽", "⠾", "⠿", 

    // Bottom left
    "⡀", "⡁", "⡂", "⡃", "⡄", "⡅", "⡆", "⡇", "⡈", "⡉", "⡊", "⡋", "⡌", "⡍", "⡎", "⡏", 
    "⡐", "⡑", "⡒", "⡓", "⡔", "⡕", "⡖", "⡗", "⡘", "⡙", "⡚", "⡛", "⡜", "⡝", "⡞", "⡟", 
    "⡠", "⡡", "⡢", "⡣", "⡤", "⡥", "⡦", "⡧", "⡨", "⡩", "⡪", "⡫", "⡬", "⡭", "⡮", "⡯", 
    "⡰", "⡱", "⡲", "⡳", "⡴", "⡵", "⡶", "⡷", "⡸", "⡹", "⡺", "⡻", "⡼", "⡽", "⡾", "⡿", 

    // Bottom right
    "⢀", "⢁", "⢂", "⢃", "⢄", "⢅", "⢆", "⢇", "⢈", "⢉", "⢊", "⢋", "⢌", "⢍", "⢎", "⢏", 
    "⢐", "⢑", "⢒", "⢓", "⢔", "⢕", "⢖", "⢗", "⢘", "⢙", "⢚", "⢛", "⢜", "⢝", "⢞", "⢟", 
    "⢠", "⢡", "⢢", "⢣", "⢤", "⢥", "⢦", "⢧", "⢨", "⢩", "⢪", "⢫", "⢬", "⢭", "⢮", "⢯", 
    "⢰", "⢱", "⢲", "⢳", "⢴", "⢵", "⢶", "⢷", "⢸", "⢹", "⢺", "⢻", "⢼", "⢽", "⢾", "⢿", 

    // Both bottom
    "⣀", "⣁", "⣂", "⣃", "⣄", "⣅", "⣆", "⣇", "⣈", "⣉", "⣊", "⣋", "⣌", "⣍", "⣎", "⣏", 
    "⣐", "⣑", "⣒", "⣓", "⣔", "⣕", "⣖", "⣗", "⣘", "⣙", "⣚", "⣛", "⣜", "⣝", "⣞", "⣟", 
    "⣠", "⣡", "⣢", "⣣", "⣤", "⣥", "⣦", "⣧", "⣨", "⣩", "⣪", "⣫", "⣬", "⣭", "⣮", "⣯", 
    "⣰", "⣱", "⣲", "⣳", "⣴", "⣵", "⣶", "⣷", "⣸", "⣹", "⣺", "⣻", "⣼", "⣽", "⣾", "⣿"
};

const int add_bit_lookup_table[2][4] = 
{
    {1, 2, 4, 64},
    {8, 16, 32, 128}
};

struct drawable_func {
    float (*func)(float);
    struct drawable_func *next;
};

struct screen {
    int width;
    int height;
    float x_zoom;
    float y_zoom;
    int x_offset;
    int y_offset;
    struct drawable_func *func_head;
};



// Draw pixel as braille on screen
void draw_px(int x, int y) {
    // Position of braille char on the screen
    int scr_x = x / 2;
    int scr_y = y / 4;
    wchar_t actual_ch = mvinch(scr_y, scr_x);
    uint8_t curr_ch;
    // The value of the smallest braille character
    if (actual_ch <= 10240) {
        curr_ch = 0;
    } else {
        curr_ch = actual_ch - L'⠀';
    }

    // Position of the pixel to add within the braille char
    int sub_x = x % 2;
    int sub_y = y % 4;
    uint8_t add_bit = add_bit_lookup_table[sub_x][sub_y];
    mvprintw(scr_y, scr_x, braille[curr_ch | add_bit]);
    
    //mvprintw(scr_x, scr_y, braille[255]);
    /*
    char debug_info[MAX_LEN];
    snprintf(debug_info, MAX_LEN, "x: %d, y: %d, found_char: %lc, add_bit:---%d---", scr_x, scr_y, actual_ch, add_bit);
    mvprintw(0, 0, debug_info);
    */
    // empty braille char
    // L'⠀'

}

// Draws a pixel assuming a normal cartesian plane where the center of the screen is 0, 0
// Width and height are also in braille pixels not actual chars
void draw_px_normalised(int x, int y, struct screen *screen_info) {

    if (((float) screen_info->height / 2) - y < 0) {
        return;
    } else if (((float) screen_info->width / 2) + x < 0) {
        return;
    }
    draw_px(x + (screen_info->width / 2), -y + (screen_info->height / 2));
}

double max(double a, double b) {
    if (a > b) {
        return a;
    }
    return b;
}

double min(double a, double b) {
    if (a < b) {
        return a;
    }
    return b;
}

int round_float(float a) {
    if (a - (int) a > 0.5) {
        return (int) a + 1;
    }
    return (int)a;
}

void draw_line(int x1, int y1, int x2, int y2, struct screen *screen_info) {
    // This means that they are further apart vertically than horizontally
    if (abs(x1 - x2) < abs(y1 - y2)) {
        int dist = abs(y1 - y2);
        float increment = (float) abs(x1 - x2) / (float) dist;
        int min_y;
        int start_x;
        if (y1 < y2) {
            min_y = y1;
            start_x = x1;
            increment = -increment;
        } else {
            min_y = y2;
            start_x = x2;
        }
        for (int i = 0; i <= dist; i++) {
            draw_px_normalised(round_float(start_x + increment * i), min_y + i, screen_info);
        }
        //char debug_str[1024];
        //snprintf(debug_str, 1024, "Start y-%d- start x-%d- end y-%d- end x-%d-", min_y
    } else {
        int dist = abs(x1 - x2);
        float increment = (float) abs(y1 - y2) / (float) dist;
        int min_x;
        int start_y;
        if (x1 < x2) {
            min_x = x1;
            start_y = y1;
        } else {
            min_x = x2;
            start_y = y2;
        }
        if (y1 < y2) {
            increment = -increment;
        }
        for (int i = 0; i <= dist; i++) {
            draw_px_normalised(min_x + i, round_float(start_y + increment * i), screen_info);
        }
    }
}

int has_asymptote_between(float (*func)(float), double x1, double x2) {
    int threshold = 1000;
    double increment = fabs(x1 - x2) / 100;
    for (double i = min(x1, x2); i <= max(x1, x2); i += increment) {
        double val = func(i);
        if (val > threshold || val < -threshold) {
            return true;
        }
        
    }
    return 0;
}

void draw_func(float (*func)(float), struct screen *sc) {
    int min_x = -(sc->width / 2) - sc->x_offset;
    int max_x = sc->width + min_x;
    char dbs[1024];
    int idx = 0;
    //snprintf(dbs, 1024, "Min: %d max: %d", min_x / sc->x_zoom + 1, max_x / sc->x_zoom + 1);
    //mvprintw(0, 0, dbs);
    // Storing whether to try and draw an asymptote
    for (int i = min_x + 1; i < max_x; i++) {
        double x1 = i;
        double y1 = func((double)x1 / (double)sc->x_zoom);
        double x2 = i - 1;
        double y2 = func((double)x2 / (double)sc->x_zoom);

        // Checking for asymptotes and handling appropriately
        if ((y1 == INFINITY || y1 == -INFINITY) && (y2 == INFINITY || y2 == -INFINITY)) {
            //snprintf(dbs, 1024, "DONT DRAW ANYTHING: x1: %lf, zoom: %d, div: %lf", x1, sc->x_zoom, x1 / sc->x_zoom);
            //mvprintw(idx, 0, dbs);
            idx++;
            continue;
        }

        if ((y1 * y2) < 0 && has_asymptote_between(func, x1, x2)) {
            continue;
        }

        if (y1 == INFINITY) {
            y1 = 1000;
        } else if (y1 == -INFINITY) {
            y1 = -1000;
        }
        if (y2 == INFINITY) {
            y2 = 1000;
        } else if (y2 == -INFINITY) {
            y2 = -1000;
        }

        y1 *= sc->y_zoom;
        y2 *= sc->y_zoom;
        x1 += sc->x_offset;
        x2 += sc->x_offset;
        y1 += sc->y_offset;
        y2 += sc->y_offset;
        draw_line((int)x1, (int)y1, (int)x2, (int)y2, sc);
        //char ch;
        //ch = getch();
    }
}

void print_axis(struct screen *sc) {
    int axis_size = 3;
    attron(COLOR_PAIR(4));
    draw_line(sc->x_offset, sc->y_offset + axis_size, sc->x_offset, sc->y_offset + -axis_size, sc);
    draw_line(sc->x_offset + axis_size, sc->y_offset, sc->x_offset + -axis_size, sc->y_offset, sc);
}

void draw_funcs(struct screen *screen_info) {
    struct drawable_func *curr_f = screen_info->func_head;
    print_axis(screen_info);
    int col = 0;
    while (curr_f != NULL) {
        attron(COLOR_PAIR(col % 3 + 1));
        col++;
        draw_func(curr_f->func, screen_info);
        curr_f = curr_f->next;
    }
}


void change_x_offset(struct screen *screen_info, int amount) {
    clear();
    screen_info->x_offset += amount;
    draw_funcs(screen_info);
}

void change_y_offset(struct screen *screen_info, double amount) {
    clear();
    screen_info->y_offset += amount;
    draw_funcs(screen_info);
}

void change_x_zoom(struct screen *screen_info, double amount) {
    float old_zoom = screen_info->x_zoom;
    clear();
    screen_info->x_zoom += amount;
    screen_info->x_zoom = max(screen_info->x_zoom, 1);
    screen_info->x_offset = (int) (screen_info->x_offset * (screen_info->x_zoom / old_zoom));
    
    draw_funcs(screen_info);
}

void change_y_zoom(struct screen *screen_info, double amount) {
    float old_zoom = screen_info->y_zoom;
    clear();
    screen_info->y_zoom += amount;
    screen_info->y_zoom = max(screen_info->y_zoom, 1);
    screen_info->y_offset = (int) (screen_info->y_offset * (screen_info->y_zoom / old_zoom));
    draw_funcs(screen_info);
}

struct drawable_func *init_drawable_func(float (*func)(float)) {
    struct drawable_func *f = malloc(sizeof(struct drawable_func));
    f->func = func;
    f->next = NULL;
    return f;
}

void add_func(float (*func)(float), struct screen *sc) {
    struct drawable_func *new_func = init_drawable_func(func);
    struct drawable_func *curr_f = sc->func_head;
    if (curr_f == NULL) {
        sc->func_head = new_func;
        return;
    }
    while (curr_f->next != NULL) {
        curr_f = curr_f->next;
    }
    curr_f->next = new_func;
}

float ret_x(float n) {
    return n;
}

float simple_parabola(float n) {
    return n * n + 1;
}

float parabola2(float n) {
    return 1.0/2 * n * n;
}

float cubic(float n) {
    return n * n * n - 1;
}

float double_num(float n) {
    return n * 2;
}

float return_num(float n) {
    return n;
}


float exponent(float n) {
    if (n > 10) {
        return 1000;
    }
    return pow(2, n);
}

float mini_exponent(float n) {
    if (n > 20) {
        return 1000;
    } else if (n < -20) {
        return 0;
    }
    return powf(1.5, n);
}

float hyperbola(float n) {
    if (fabs(n) < 0.01) {
        if (n > 0) {
            return INFINITY;
        }
        return -INFINITY;
    }
    return (double)(1) / n;
}

float hyperbola_tangent(float n) {
    return 2 - n;
}

float hyperbola_tangent_2(float n) {
    return -2 - n;
}





double parse_expression(char *exp, double x) {
    printf("%s\n", exp);
    int idx = 1;
}


int main(int argc, char *argv[]) {
    setlocale(LC_ALL, "");
    initscr();
    // Characters don't get drawn on screen while typing
    noecho();
    // Allows colours
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_BLUE, COLOR_BLACK);
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);
    attron(COLOR_PAIR(1));
    // Removes cursor
    curs_set(0);
    refresh();
    int height;
    int width;
    // Height and width are in here
    getmaxyx(stdscr, height, width);

    int x_pixels = width * 2;
    int y_pixels = height * 4;
    struct screen sc;
    sc.width = x_pixels;
    sc.height = y_pixels;
    sc.x_zoom = 16;
    sc.y_zoom = 16;
    sc.x_offset = 0;
    sc.y_offset = 0;
    //df->next = df2;
    //sc.func_head = df;
    
    add_func(hyperbola, &sc);
    add_func(hyperbola_tangent, &sc);
    add_func(hyperbola_tangent_2, &sc);
    //add_func(mini_exponent, &sc);
    //add_func(simple_parabola, &sc);
    //add_func(parabola2, &sc);
    //add_func(cubic, &sc);
    //add_func(sin, &sc);
    //add_func(double_num, &sc);
    add_func(ret_x, &sc);
    // This seems to make it draw nicer for some reason?
    /*
    for (int i = 0; i < 3; i++)  {
        change_x_zoom(&sc, sc.x_zoom);
        change_y_zoom(&sc, sc.y_zoom);
    }
    */
    draw_funcs(&sc);
    char ch = ' ';
    char *buf = malloc(1024);
    while (ch != 'q') {
        ch = getch();
        if (ch == 'h' || ch == 'a') {
            change_x_offset(&sc, 20);
        } else if (ch == 'l' || ch == 'd') {
            change_x_offset(&sc, -20);
        } else if (ch == 'j' || ch == 's') {
            change_y_offset(&sc, 20);
        } else if (ch == 'k' || ch == 'w') {
            change_y_offset(&sc, -20);
        } else if (ch == 'x') {
            change_x_zoom(&sc, sc.x_zoom);
        } else if (ch == 'X') {
            change_x_zoom(&sc, -sc.x_zoom/2);
        } else if (ch == 'y') {
            change_y_zoom(&sc, sc.y_zoom);
        } else if (ch == 'Y') {
            change_y_zoom(&sc, -sc.y_zoom/2);
        } else if (ch == 'z') {
            change_x_zoom(&sc, sc.x_zoom);
            change_y_zoom(&sc, sc.y_zoom);
        } else if (ch == 'Z') {
            change_x_zoom(&sc, -sc.x_zoom / 2);
            change_y_zoom(&sc, -sc.y_zoom / 2);
        } else if (ch == 'i') {
            snprintf(buf, 1024, "CENTER: x--%d--, y--%d--", sc.x_offset, sc.y_offset);
            mvprintw(0, 0, buf);
        }
        else {
            attron(COLOR_PAIR(1));
            mvprintw(0, 0, "INVALID COMMAND");
        }
    }
    wchar_t test_ch = mvinch(2, 0);
    endwin();
    for (int i = 1; i < argc; i++) {
        parse_expression(argv[i], 0);
    }
}

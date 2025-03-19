#include <ncurses.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#define MAX_FILES 1024
#define PATH_MAX_LEN 1024

typedef struct {
    char name[PATH_MAX_LEN];
    int is_dir;
} FileEntry;

FileEntry files_left[MAX_FILES], files_right[MAX_FILES];
int file_count_left = 0, file_count_right = 0;
int selected_left = 0, selected_right = 0;
int active_panel = 0; // 0 - левый, 1 - правый
char current_path_left[PATH_MAX_LEN] = "./";
char current_path_right[PATH_MAX_LEN] = "./";

void load_directory(const char *path, FileEntry *files, int *file_count) {
    DIR *dir = opendir(path);
    if (!dir) return;
    struct dirent *entry;
    *file_count = 0;
    while ((entry = readdir(dir)) != NULL && *file_count < MAX_FILES) {
        strncpy(files[*file_count].name, entry->d_name, PATH_MAX_LEN - 1);
        files[*file_count].name[PATH_MAX_LEN - 1] = '\0';
        files[*file_count].is_dir = (entry->d_type == DT_DIR);
        (*file_count)++;
    }
    closedir(dir);
}

void draw_interface() {
    clear();
    int mid = COLS / 2;
    
    for (int i = 0; i < file_count_left; i++) {
        if (i == selected_left && active_panel == 0) attron(A_REVERSE);
        mvprintw(i + 1, 2, "%s%s", files_left[i].name, files_left[i].is_dir ? "/" : "");
        if (i == selected_left && active_panel == 0) attroff(A_REVERSE);
    }
    for (int i = 0; i < file_count_right; i++) {
        if (i == selected_right && active_panel == 1) attron(A_REVERSE);
        mvprintw(i + 1, mid + 2, "%s%s", files_right[i].name, files_right[i].is_dir ? "/" : "");
        if (i == selected_right && active_panel == 1) attroff(A_REVERSE);
    }
    mvprintw(0, 2, "Left: %s", current_path_left);
    mvprintw(0, mid + 2, "Right: %s", current_path_right);
    refresh();
}

void enter_directory(int panel) {
    if (panel == 0 && files_left[selected_left].is_dir) {
        chdir(files_left[selected_left].name);
        getcwd(current_path_left, PATH_MAX_LEN);
        load_directory(current_path_left, files_left, &file_count_left);
        selected_left = 0;
    } else if (panel == 1 && files_right[selected_right].is_dir) {
        chdir(files_right[selected_right].name);
        getcwd(current_path_right, PATH_MAX_LEN);
        load_directory(current_path_right, files_right, &file_count_right);
        selected_right = 0;
    }
}

void open_file() {
    char command[PATH_MAX_LEN + 10];
    const char *file = active_panel == 0 ? files_left[selected_left].name : files_right[selected_right].name;
    snprintf(command, sizeof(command), "xdg-open '%.1020s' &", file);
    system(command);
}

void copy_file() {
    char new_name[PATH_MAX_LEN];
    const char *file = active_panel == 0 ? files_left[selected_left].name : files_right[selected_right].name;
    snprintf(new_name, sizeof(new_name), "%.1018s_copy", file);
    char command[PATH_MAX_LEN * 2];
    snprintf(command, sizeof(command), "cp '%.1018s' '%.1018s'", file, new_name);
    system(command);
}

void delete_file() {
    const char *file = active_panel == 0 ? files_left[selected_left].name : files_right[selected_right].name;
    remove(file);
    if (active_panel == 0)
        load_directory(current_path_left, files_left, &file_count_left);
    else
        load_directory(current_path_right, files_right, &file_count_right);
}

int main() {
    initscr();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    
    load_directory(current_path_left, files_left, &file_count_left);
    load_directory(current_path_right, files_right, &file_count_right);

    while (1) {
        draw_interface();
        int ch = getch();
        switch (ch) {
            case KEY_UP:
                if (active_panel == 0 && selected_left > 0) selected_left--;
                else if (active_panel == 1 && selected_right > 0) selected_right--;
                break;
            case KEY_DOWN:
                if (active_panel == 0 && selected_left < file_count_left - 1) selected_left++;
                else if (active_panel == 1 && selected_right < file_count_right - 1) selected_right++;
                break;
            case '\n':
                enter_directory(active_panel);
                open_file();
                break;
            case '\t': // Tab - переключение панелей
                active_panel = !active_panel;
                break;
            case KEY_F(5):
                copy_file();
                break;
            case KEY_F(8):
                delete_file();
                break;
            case 27: // Esc
                mvprintw(file_count_left + 2, 2, "Exit? (y/n)");
                refresh();
                if (getch() == 'y') { endwin(); return 0; }
                break;
        }
    }
    endwin();
    return 0;
}

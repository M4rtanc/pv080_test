#include <limits.h>
#include <stddef.h>

#ifndef HW05_MAIN_H
#define HW05_MAIN_H

#endif //HW05_MAIN_H


struct params {
    char *param_name;
    int param_sort;
    int param_mask;
    char *param_user;
    int param_min_depth;
    int param_max_depth;
    int param_all;
    char param_newline;
    char *param_path;
};

struct file {
    /** pointer to name of file */
    char *name;
    /** pointer to full address of file */
    char *path;
    /** size of file */
    int size;
};

struct params opts = {NULL, 0, -1, NULL, 1, INT_MAX, 0, '\n', NULL};
struct file *all_files = NULL;

char help[] = "Help.\n"
              "USAGE: [OPTIONS] [START_DIR]\n"
              "OPTIONS:\n"
              "-h               Show help.\n"
              "-n NAME          Filter by substring in name.\n"
              "-m MASK          Filter by permissions.\n"
              "-u USER          Filter by user name.\n"
              "-f num           Filter by minimal depth.\n"
              "-t num           Filter by maximal depth.\n"
              "-s SORT          Sorting by full path or size.\n"
              "-a               Go throught hidden dirs.\n"
              "-0               Use null byte as line separator.\n"
              "\n"
              "SORT:\n"
              "\"s\"            Sort by size.\n"
              "\"f\"            Sort by full/relative file path.\n";

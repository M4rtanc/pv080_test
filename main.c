#include "main.h"
#include <dirent.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <getopt.h>


bool check_substring(char *file_name)
{
    if ((strstr(file_name, opts.param_name)) == NULL) {
        return 0;
    }
    return 1;
}


bool check_user(char *file_path)
{
    if (opts.param_user == NULL) {
        return 1;
    }
    if (file_path == NULL) {
        return 0;
    }
    struct stat file_stat;
    if (lstat(file_path, &file_stat) == -1) {
        return 0;
    }

    struct passwd *file_passwd = NULL;
    if ((file_passwd = getpwuid(file_stat.st_uid)) == NULL) {
        return 0;
    }

    if (!strcmp(opts.param_user, file_passwd->pw_name)) {
        return 1;
    } else {
        return 0;
    }
}


bool check_mask(struct stat *stat_file)
{
    if (opts.param_mask == -1) {
        return 1;
    }

    int result = stat_file->st_mode;
    result &= 511;
    if (result == opts.param_mask) {
        return 1;
    }

    return 0;
}


int name_compare(const struct file *file1, const struct file *file2)
{
    return strcasecmp(file1->name, file2->name);
}


int size_compare(const struct file *file1, const struct file *file2)
{
    if (file1->size < file2->size) {
        return 1;
    } else if (file1->size > file2->size) {
        return -1;
    }
    return name_compare(file1, file2);
}


int path_compare(const struct file *file1, const struct file *file2)
{
    return strcmp(file1->path, file2->path);
}


int print_files(const size_t *count)
{
    if (all_files == NULL || count == NULL) {
        return 1;
    }

    for (size_t i = 0; i < *count; i++) {
        printf("%s%c", all_files[i].path, opts.param_newline);
    }

    return 0;
}


int free_all(const size_t *count)
{
    if (all_files == NULL || count == NULL) {
        return 1;
    }
    for (size_t i = 0; i < *count; i++) {
        free(all_files[i].name);
        free(all_files[i].path);
    }
    return 0;
}


int save_file(const char *file_path, const char *file_name, int size, size_t *count)
{
    if (all_files == NULL || file_path == NULL || file_name == NULL || count == NULL) {
        return 1;
    }

    if (*count && (*count % 100 == 0)) {
        if ((all_files = realloc(all_files, (*count + 100) * sizeof(struct file))) == NULL) {
            return 1;
        }
        for (size_t i = *count; i < *count + 100; i++) {
            all_files[i].name = NULL;
            all_files[i].path = NULL;
            all_files[i].size = 0;
        }
    }
    size_t name_memory = strlen(file_name) + 1;
    size_t path_memory = strlen(file_path) + 1;

    if ((all_files[*count].name = realloc(all_files[*count].name, name_memory * sizeof(char))) == NULL) {
        free(all_files[*count].name);
        return 1;
    }

    all_files[*count].size = size;

    if ((all_files[*count].path = realloc(all_files[*count].path, path_memory * sizeof(char))) == NULL) {
        free(all_files[*count].name);
        free(all_files[*count].path);
        return 1;
    }

    if ((all_files[*count].name = strcpy(all_files[*count].name, file_name)) == NULL) {
        free(all_files[*count].name);
        free(all_files[*count].path);
        return 1;
    }

    if ((all_files[*count].path = strcpy(all_files[*count].path, file_path)) == NULL) {
        free(all_files[*count].name);
        free(all_files[*count].path);
        return 1;
    }

    *count += 1;
    return 0;
}


static char* new_path(const char *old_path, const char *dir_name)
{
    if (old_path == NULL || dir_name == NULL) {
        return NULL;
    }
    size_t path_len = strlen(old_path);
    size_t needed_memory = path_len + strlen(dir_name) + 2;
    char *new_path;
    if ((new_path = calloc(needed_memory, sizeof(char))) == NULL) {
        return NULL;
    }
    if (memset(new_path, '\0', needed_memory) == NULL) {
        return NULL;
    }

    strcpy(new_path, old_path);
    if (old_path[path_len - 1] != '/') {
        new_path[path_len] = '/';
    }

    if (strcat(new_path, dir_name) == NULL) {
        return NULL;
    }

    return new_path;
}


bool find_everything(char *path, size_t *count, int min_depth, int depth, int max_depth)
{
    if (depth > max_depth) {
        return 0;
    }
    DIR *actual_dir = NULL;
    bool result = 0;
    if ((actual_dir = opendir(path)) != NULL) {
        struct dirent *actual_dir_file = NULL;
        char *file_name = NULL;
        char *file_path = NULL;
        struct stat file_stat;
        while ((actual_dir_file = readdir(actual_dir)) != NULL) {
            file_name = actual_dir_file->d_name;
            if (strcmp(file_name, ".") == 0 || strcmp(file_name, "..") == 0) {
                continue;
            }
            if (!opts.param_all && file_name[0] == '.') {
                continue;
            }
            if ((file_path = new_path(path, file_name)) == NULL) {
                fprintf(stderr,"%s/%s: Can not open\n", path, file_name);
                continue;
            }
            if (lstat(file_path, &file_stat) == -1) {
                fprintf(stderr,"%s/%s: Can not open\n", path, file_name);
                continue;
            }
            int size = file_stat.st_size;
            if (S_ISREG(file_stat.st_mode)) {
                if ((min_depth <= depth) && check_substring(file_name)
                    && check_user(file_path) && check_mask(&file_stat)) {
                    if (save_file(file_path, file_name, size, count)) {
                        fprintf(stderr,"Out of memory\n");
                        free(file_path);
                        return 1;
                    }
                }
            }
            if (S_ISDIR(file_stat.st_mode)) {
                result |= find_everything(file_path, count, min_depth, depth + 1, max_depth);
            }
            free(file_path);
        }
        closedir(actual_dir);
        return result;
    } else if (depth == 1) {
        fprintf(stderr,"%s: No such file or directory\n", path);
        return 1;
    } else {
        fprintf(stderr,"%s: Can not open\n", path);
    }

    return result;
}


int set_number_param(char *number, int base)
{
    if (number == NULL) {
        return -1;
    }
    size_t len = strlen(number);
    char *end_ptr = NULL;
    int result;

    result = strtol(number, &end_ptr, base);
    if ((result == 0 && number[0] != '0') || (result < 0) || (end_ptr - len != number)) {
        return -1;
    }

    return result;
}


int main(int argc, char *argv[])
{
    int opt;
    while ((opt = getopt(argc, argv, ":n:s:m:u:f:t:a0h")) != -1) {
        switch (opt){
            case 'n': {
                opts.param_name = optarg;
                break;
            }
            case 's': {
                if (*optarg == 'f') {
                    opts.param_sort = 1;
                    break;
                } else if (*optarg == 's') {
                    opts.param_sort = 2;
                    break;
                } else {
                    fprintf(stderr,"%s: No such comparator\n", optarg);
                    return 1;
                }
            }
            case 'm': {
                opts.param_mask = set_number_param(optarg, 8);
                if (strlen(optarg) > 3 || opts.param_mask == -1) {
                    fprintf(stderr,"%s: Invalid permission mask\n", optarg);
                    return 1;
                }
                break;
            }
            case 'u': {
                opts.param_user = optarg;
                break;
            }
            case 'f': {
                if ((opts.param_min_depth = set_number_param(optarg, 10)) == -1) {
                    fprintf(stderr,"%s: Invalid depth level\n", optarg);
                    return 1;
                }
                break;
            }
            case 't': {
                if ((opts.param_max_depth = set_number_param(optarg, 10)) == -1) {
                    fprintf(stderr,"%s: Invalid depth level\n", optarg);
                    return 1;
                }
                break;
            }
            case 'a': {
                opts.param_all = 1;
                break;
            }
            case '0': {
                opts.param_newline = '\0';
                break;
            }
            case 'h': {
                fprintf(stderr,"%s", help);
                return 0;
            }
            case ':': {
                fprintf(stderr,"option requires an argument\n");
                return 1;
            }
            case '?': {
                fprintf(stderr,"%s: invalid option\n", optarg);
                return 1;
            }
            default: {
                break;
            }
        }
    }

    opts.param_path = argv[optind];
    if (opts.param_name == NULL) {
        opts.param_name = "";
    }
    if (opts.param_path == NULL) {
        opts.param_path = "./";
    }

    size_t count = 0;
    all_files = calloc(100, sizeof(struct file));

    if (find_everything(opts.param_path, &count, opts.param_min_depth, 1, opts.param_max_depth)) {
        free_all(&count);
        free(all_files);
        return 1;
    }

    switch (opts.param_sort) {
        case 0: {
            qsort(all_files, count, sizeof(struct file), (int (*)(const void *, const void *)) name_compare);
            break;
        }
        case 1: {
            qsort(all_files, count, sizeof(struct file), (int (*)(const void *, const void *)) path_compare);
            break;
        }
        case 2: {
            qsort(all_files, count, sizeof(struct file), (int (*)(const void *, const void *)) size_compare);
            break;
        }
        default: {
            break;
        }
    }

    print_files(&count);

    free_all(&count);
    free(all_files);

    return 0;
}

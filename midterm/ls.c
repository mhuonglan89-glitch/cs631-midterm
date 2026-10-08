#include "ls.h"

Options opts = {0};

void process_directory(const char *dir_path);

void process_file_entries(FileInfo *files, int count, int is_dir) {
    int (*cmp)(const void *, const void *) = compare_lexicographical;
    if (opts.sort_time) cmp = compare_time;
    else if (opts.sort_size) cmp = compare_size;

    if (!opts.unsorted) {
        qsort(files, count, sizeof(FileInfo), cmp);
    }

    print_file_list(files, count, is_dir);

    if (opts.recursive) {
        for (int i = 0; i < count; i++) {
            if (S_ISDIR(files[i].st.st_mode)) {
                if (strcmp(files[i].name, ".") == 0 || strcmp(files[i].name, "..") == 0)
                    continue;
                printf("\n%s:\n", files[i].path);
                process_directory(files[i].path);
            }
        }
    }
}

void process_directory(const char *dir_path) {
    DIR *dir = opendir(dir_path);
    if (!dir) {
        fprintf(stderr, "ls: %s: %s\n", dir_path, strerror(errno));
        return;
    }

    struct dirent *entry;
    FileInfo *files = NULL;
    int count = 0;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.' && !opts.unsorted) {
            if (!opts.show_all && !opts.almost_all) continue;
            if (opts.almost_all && (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")))
                continue;
        }

        files = realloc(files, (count + 1) * sizeof(FileInfo));
        files[count].name = strdup(entry->d_name);

        char fullpath[4096];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", dir_path, entry->d_name);
        files[count].path = strdup(fullpath);

        if (lstat(fullpath, &files[count].st) == -1) {
            files[count].stat_ok = 0;
            memset(&files[count].st, 0, sizeof(struct stat));
        } else {
            files[count].stat_ok = 1;
        }
        count++;
    }
    closedir(dir);

    process_file_entries(files, count, 1);

    for (int i = 0; i < count; i++) {
        free(files[i].name);
        free(files[i].path);
    }
    free(files);
}

int main(int argc, char **argv) {
    int ch;
    opts.one_per_line = 1;

    /* Xác định chế độ xuất ra terminal hay redirect */
    if (isatty(STDOUT_FILENO)) {
        opts.print_safe = 1;
    } else {
        opts.print_raw = 1;
    }

    /* Đọc các flags theo NetBSD ls(1) */
    while ((ch = getopt(argc, argv, "AacdFfhiklnqRrSstuw1")) != -1) {
        switch (ch) {
            case 'A': opts.almost_all = 1; break;
            case 'a': opts.show_all = 1; break;
            case 'c': opts.time_status = 1; opts.time_access = 0; break;
            case 'd': opts.directory = 1; break;
            case 'F': opts.classify = 1; break;
            case 'f': opts.unsorted = 1; opts.show_all = 1; break;
            case 'h': opts.human = 1; opts.kbytes = 0; break;
            case 'i': opts.inode = 1; break;
            case 'k': opts.kbytes = 1; opts.human = 0; break;
            case 'l': opts.long_format = 1; break;
            case 'n': opts.numeric_id = 1; opts.long_format = 1; break;
            case 'q': opts.print_safe = 1; opts.print_raw = 0; break;
            case 'R': opts.recursive = 1; break;
            case 'r': opts.reverse = 1; break;
            case 'S': opts.sort_size = 1; break;
            case 's': opts.show_blocks = 1; break;
            case 't': opts.sort_time = 1; break;
            case 'u': opts.time_access = 1; opts.time_status = 0; break;
            case 'w': opts.print_raw = 1; opts.print_safe = 0; break;
            case '1': opts.one_per_line = 1; break;
            default:
                fprintf(stderr, "usage: ls [-AacdFfhiklnqRrSstuw1] [file ...]\n");
                exit(EXIT_FAILURE);
        }
    }
    argc -= optind;
    argv += optind;

    if (argc == 0) {
        process_directory(".");
    } else {
        FileInfo *file_operands = NULL;
        FileInfo *dir_operands = NULL;
        int file_cnt = 0, dir_cnt = 0;

        for (int i = 0; i < argc; i++) {
            struct stat st;
            if (lstat(argv[i], &st) == -1) {
                fprintf(stderr, "ls: %s: %s\n", argv[i], strerror(errno));
                continue;
            }

            if (S_ISDIR(st.st_mode) && !opts.directory) {
                dir_operands = realloc(dir_operands, (dir_cnt + 1) * sizeof(FileInfo));
                dir_operands[dir_cnt].name = argv[i];
                dir_operands[dir_cnt].path = argv[i];
                dir_operands[dir_cnt].st = st;
                dir_operands[dir_cnt].stat_ok = 1;
                dir_cnt++;
            } else {
                file_operands = realloc(file_operands, (file_cnt + 1) * sizeof(FileInfo));
                file_operands[file_cnt].name = argv[i];
                file_operands[file_cnt].path = argv[i];
                file_operands[file_cnt].st = st;
                file_operands[file_cnt].stat_ok = 1;
                file_cnt++;
            }
        }

        /* 1. In các file thường trước theo NetBSD manpage */
        if (file_cnt > 0) {
            process_file_entries(file_operands, file_cnt, 0);
            free(file_operands);
        }

        /* 2. In các thư mục sau */
        if (dir_cnt > 0) {
            int (*cmp)(const void *, const void *) = compare_lexicographical;
            if (opts.sort_time) cmp = compare_time;
            else if (opts.sort_size) cmp = compare_size;
            if (!opts.unsorted) qsort(dir_operands, dir_cnt, sizeof(FileInfo), cmp);

            for (int i = 0; i < dir_cnt; i++) {
                if (argc > 1) {
                    if (file_cnt > 0 || i > 0) printf("\n");
                    printf("%s:\n", dir_operands[i].path);
                }
                process_directory(dir_operands[i].path);
            }
            free(dir_operands);
        }
    }
    return EXIT_SUCCESS;
}

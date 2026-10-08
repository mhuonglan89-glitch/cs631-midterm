#ifndef _LS_H_
#define _LS_H_

#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 700

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <ctype.h>
#include <stdint.h>

typedef struct file_info {
    char *name;
    char *path;
    struct stat st;
    int stat_ok;
} FileInfo;

typedef struct options {
    int show_all;     /* -a */
    int almost_all;   /* -A */
    int time_status;  /* -c: status changed time */
    int directory;    /* -d */
    int classify;     /* -F */
    int unsorted;     /* -f */
    int human;        /* -h */
    int inode;        /* -i */
    int kbytes;       /* -k */
    int long_format;  /* -l */
    int numeric_id;   /* -n */
    int print_safe;   /* -q: replace non-printable with '?' */
    int recursive;    /* -R */
    int reverse;      /* -r */
    int sort_size;    /* -S */
    int show_blocks;  /* -s */
    int sort_time;    /* -t */
    int time_access;  /* -u: last access time */
    int print_raw;    /* -w */
    int one_per_line; /* -1 (default) */
} Options;

extern Options opts;

time_t get_file_time(const struct stat *st);
int compare_lexicographical(const void *a, const void *b);
int compare_time(const void *a, const void *b);
int compare_size(const void *a, const void *b);

void print_file_list(FileInfo *files, int count, int is_dir);
void print_file_name(const char *name);

#endif /* _LS_H_ */

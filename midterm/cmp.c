#include "ls.h"

time_t get_file_time(const struct stat *st) {
    if (opts.time_status) return st->st_ctime;
    if (opts.time_access) return st->st_atime;
    return st->st_mtime;
}

int compare_lexicographical(const void *a, const void *b) {
    const FileInfo *fa = (const FileInfo *)a;
    const FileInfo *fb = (const FileInfo *)b;
    int res = strcmp(fa->name, fb->name);
    return opts.reverse ? -res : res;
}

int compare_time(const void *a, const void *b) {
    const FileInfo *fa = (const FileInfo *)a;
    const FileInfo *fb = (const FileInfo *)b;
    time_t ta = get_file_time(&fa->st);
    time_t tb = get_file_time(&fb->st);

    if (ta < tb) return opts.reverse ? -1 : 1;
    if (ta > tb) return opts.reverse ? 1 : -1;
    return compare_lexicographical(a, b);
}

int compare_size(const void *a, const void *b) {
    const FileInfo *fa = (const FileInfo *)a;
    const FileInfo *fb = (const FileInfo *)b;
    if (fa->st.st_size < fb->st.st_size) return opts.reverse ? -1 : 1;
    if (fa->st.st_size > fb->st.st_size) return opts.reverse ? 1 : -1;
    return compare_lexicographical(a, b);
}

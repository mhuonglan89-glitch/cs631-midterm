#include "print.h"
#include <sys/sysmacros.h>

void print_file_name(const char *name) {
    for (const char *p = name; *p != '\0'; p++) {
        if (opts.print_safe && (!isprint((unsigned char)*p))) {
            putchar('?');
        } else {
            putchar(*p);
        }
    }
}

static void format_human_size(int64_t bytes, char *buf, size_t buflen) {
    const char *suffixes[] = {"B", "K", "M", "G", "T", "P"};
    int s = 0;
    double d = (double)bytes;
    while (d >= 1024.0 && s < 5) {
        d /= 1024.0;
        s++;
    }
    if (s == 0) {
        snprintf(buf, buflen, "%4lldB", (long long)bytes);
    } else {
        snprintf(buf, buflen, "%5.1f%s", d, suffixes[s]);
    }
}

static void print_permissions(mode_t mode) {
    char perms[11];
    if (S_ISDIR(mode))       perms[0] = 'd';
    else if (S_ISLNK(mode))  perms[0] = 'l';
    else if (S_ISCHR(mode))  perms[0] = 'c';
    else if (S_ISBLK(mode))  perms[0] = 'b';
    else if (S_ISFIFO(mode)) perms[0] = 'p';
    else if (S_ISSOCK(mode)) perms[0] = 's';
    else                     perms[0] = '-';

    perms[1] = (mode & S_IRUSR) ? 'r' : '-';
    perms[2] = (mode & S_IWUSR) ? 'w' : '-';
    perms[3] = (mode & S_IXUSR) ? ((mode & S_ISUID) ? 's' : 'x') : ((mode & S_ISUID) ? 'S' : '-');
    perms[4] = (mode & S_IRGRP) ? 'r' : '-';
    perms[5] = (mode & S_IWGRP) ? 'w' : '-';
    perms[6] = (mode & S_IXGRP) ? ((mode & S_ISGID) ? 's' : 'x') : ((mode & S_ISGID) ? 'S' : '-');
    perms[7] = (mode & S_IROTH) ? 'r' : '-';
    perms[8] = (mode & S_IWOTH) ? 'w' : '-';
    perms[9] = (mode & S_IXOTH) ? ((mode & S_ISVTX) ? 't' : 'x') : ((mode & S_ISVTX) ? 'T' : '-');
    perms[10] = '\0';
    printf("%s ", perms);
}

static void print_classified_suffix(mode_t mode) {
    if (!opts.classify) return;
    if (S_ISDIR(mode))  putchar('/');
    else if (S_ISLNK(mode)) putchar('@');
    else if (S_ISSOCK(mode)) putchar('=');
    else if (S_ISFIFO(mode)) putchar('|');
    else if (mode & (S_IXUSR | S_IXGRP | S_IXOTH)) putchar('*');
}

void print_file_entry(FileInfo *file) {
    if (opts.inode) {
        printf("%llu ", (unsigned long long)file->st.st_ino);
    }

    if (opts.show_blocks) {
        int64_t block_units;
        if (opts.human) {
            char b_buf[16];
            format_human_size(file->st.st_blocks * 512, b_buf, sizeof(b_buf));
            printf("%6s ", b_buf);
        } else if (opts.kbytes) {
            block_units = (file->st.st_blocks + 1) / 2;
            printf("%4lld ", (long long)block_units);
        } else {
            block_units = file->st.st_blocks;
            printf("%4lld ", (long long)block_units);
        }
    }

    if (opts.long_format) {
        struct passwd *pw = getpwuid(file->st.st_uid);
        struct group *gr = getgrgid(file->st.st_gid);
        char time_str[32];
        time_t ftime = get_file_time(&file->st);
        struct tm *tm_info = localtime(&ftime);

        strftime(time_str, sizeof(time_str), "%b %e %H:%M", tm_info);

        print_permissions(file->st.st_mode);
        printf("%2lu ", (unsigned long)file->st.st_nlink);

        if (opts.numeric_id) {
            printf("%-8d %-8d ", (int)file->st.st_uid, (int)file->st.st_gid);
        } else {
            printf("%-8s ", pw ? pw->pw_name : "UNKNOWN");
            printf("%-8s ", gr ? gr->gr_name : "UNKNOWN");
        }

        if (S_ISCHR(file->st.st_mode) || S_ISBLK(file->st.st_mode)) {
            printf("%4d, %4d ", major(file->st.st_rdev), minor(file->st.st_rdev));
        } else if (opts.human) {
            char size_buf[16];
            format_human_size((int64_t)file->st.st_size, size_buf, sizeof(size_buf));
            printf("%6s ", size_buf);
        } else {
            printf("%8lld ", (long long)file->st.st_size);
        }

        printf("%s ", time_str);
    }

    print_file_name(file->name);
    print_classified_suffix(file->st.st_mode);

    if (opts.long_format && S_ISLNK(file->st.st_mode)) {
        char linkbuf[1024];
        ssize_t len = readlink(file->path, linkbuf, sizeof(linkbuf) - 1);
        if (len != -1) {
            linkbuf[len] = '\0';
            printf(" -> ");
            print_file_name(linkbuf);
        }
    }
    printf("\n");
}

void print_file_list(FileInfo *files, int count, int is_dir) {
    if (is_dir && (opts.long_format || opts.show_blocks)) {
        int64_t total_blocks = 0;
        for (int i = 0; i < count; i++) {
            total_blocks += files[i].st.st_blocks;
        }
        if (opts.kbytes) {
            printf("total %lld\n", (long long)((total_blocks + 1) / 2));
        } else if (opts.human) {
            char h_total[16];
            format_human_size(total_blocks * 512, h_total, sizeof(h_total));
            printf("total %s\n", h_total);
        } else {
            printf("total %lld\n", (long long)total_blocks);
        }
    }

    for (int i = 0; i < count; i++) {
        print_file_entry(&files[i]);
    }
}

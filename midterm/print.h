#ifndef _PRINT_H_
#define _PRINT_H_

#include "ls.h"

void print_file_entry(FileInfo *file);
void print_file_list(FileInfo *files, int count, int is_dir);

#endif /* _PRINT_H_ */

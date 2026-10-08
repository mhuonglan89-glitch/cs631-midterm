# CS631 Advanced Programming in the UNIX Environment - Midterm: ls(1)

Author: Mai Huong Lan  
GitHub: https://github.com/mhuonglan89-glitch/cs631-midterm

## 1. Overview & Architecture
This project implements the NetBSD ls(1) utility in C99 following POSIX standards. The codebase is organized into modular units:

- ls.h: Data structures (FileInfo, Options), feature-test macros (_POSIX_C_SOURCE 200809L, _XOPEN_SOURCE 700), and function prototypes.
- ls.c: Command-line parsing via getopt(3), operand evaluation (non-directory files sorted and processed before directory arguments), directory traversal via opendir(3)/readdir(3), and recursive traversal (-R).
- cmp.c: Comparison callbacks for qsort(3Nhấn tổ hợp phím **`Ctrl + C`** để thoát khỏi dấu nhắc lệnh `>` đang bị kẹt.

Lý do báo lỗi là bạn dán trực tiếp văn bản thường vào Terminal, nên bash tưởng đó là các câu lệnh thực thi. Để ghi nội dung vào file `README` và `README.md`, bạn phải bọc trong lệnh `cat << 'EOF' > ...` như dưới đây:

```bash
cd ~/cs631/midterm

cat << 'EOF' > README
# CS631 Advanced Programming in the UNIX Environment - Midterm: ls(1)

Author: Mai Huong Lan  
GitHub: [https://github.com/mhuonglan89-glitch/cs631-midterm](https://github.com/mhuonglan89-glitch/cs631-midterm)

## 1. Overview & Architecture
This project implements the NetBSD ls(1) utility in C99 following POSIX standards. The codebase is organized into modular units:

- ls.h: Data structures (FileInfo, Options), feature-test macros (_POSIX_C_SOURCE 200809L, _XOPEN_SOURCE 700), and function prototypes.
- ls.c: Command-line parsing via getopt(3), operand evaluation (non-directory files sorted and processed before directory arguments), directory traversal via opendir(3)/readdir(3), and recursive traversal (-R).
- cmp.c: Comparison callbacks for qsort(3) handling lexicographical order, timestamps (-t, -c, -u), file sizes (-S), and reverse ordering (-r).
- print.h & print.c: Output formatting, file mode decoding, ownership mapping, symlink target resolution via readlink(2), classification suffixes (-F), unit conversions (-h, -k), and block count calculations.

## 2. Supported Options
- -A: List all entries except for '.' and '..'.
- -a: Include directory entries whose names begin with a dot ('.').
- -c: Use file status change time (st_ctime) for sorting (-t) or printing (-l).
- -d: Treat directories as plain files (do not inspect contents).
- -F: Display '/' for directories, '*' for executables, '@' for symlinks, '=' for sockets, '|' for FIFOs.
- -f: Output is not sorted; enables -a.
- -h: Human-readable file and block sizes (B, K, M, G, T), overriding -k.
- -i: Print inode numbers.
- -k: Display block allocation sizes in kilobytes.
- -l: List in long format (file mode, link count, owner, group, size, timestamp, name, symlink pointer).
- -n: Display numeric UIDs and GIDs instead of user and group names.
- -q: Print non-printable characters as '?'.
- -R: Recursively list subdirectories.
- -r: Reverse sort order.
- -S: Sort by file size, largest first.
- -s: Display the number of file system blocks used.
- -t: Sort by modification time (most recent first).
- -u: Use time of last access (st_atime) for sorting (-t) or printing (-l).
- -w: Force raw printing of non-printable characters.
- -1: List one entry per line.

## 3. Compilation
Build using the provided Makefile:
make clean
make

Compilation flags: -Wall -Wextra -Werror -pedantic -std=c99 -g

## 4. Testing & Verification

### Output Comparison with System ls
Program output was compared directly against system ls utilities:

diff <(./ls -1 -la .) <(/bin/ls -1 -la .)
diff <(./ls -1 -lt .) <(/bin/ls -1 -lt .)
diff <(./ls -1 -lS .) <(/bin/ls -1 -lS .)
diff <(./ls -1 -lin .) <(/bin/ls -1 -lin .)
diff <(./ls -1 -lu .) <(/bin/ls -1 -lu .)
diff <(./ls -1 -lc .) <(/bin/ls -1 -lc .)

### Edge Cases Handled
1. Broken and circular symbolic links verified using lstat(2) instead of stat(2).
2. Permission denied scenarios handled with strerror(errno) without crashing traversal.
3. Empty directories correctly print 'total 0' before returning.
4. Mixed operands (./ls file1 dir1 file2) correctly display files first, followed by directory listings.

### Memory Leak Verification
Memory leak verification conducted using Valgrind:
valgrind --leak-check=full --show-leak-kinds=all ./ls -la -R .

Result: 0 errors from 0 contexts, all allocated heap blocks freed.

#include <stdio.h>
#include <assert.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>

/* Taken from https://linux.die.net/man/3/stat */
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <langinfo.h>
#include <stdint.h>

#define CWDS "."
#define STAT_OPTION "-l"

char const * sperm(mode_t mode) {
    static char local_buff[16] = {0};
    int i = 0;
    // user permissions
    if ((mode & S_IRUSR) == S_IRUSR) local_buff[i] = 'r';
    else local_buff[i] = '-';
    i++;
    if ((mode & S_IWUSR) == S_IWUSR) local_buff[i] = 'w';
    else local_buff[i] = '-';
    i++;
    if ((mode & S_IXUSR) == S_IXUSR) local_buff[i] = 'x';
    else local_buff[i] = '-';
    i++;
    // group permissions
    if ((mode & S_IRGRP) == S_IRGRP) local_buff[i] = 'r';
    else local_buff[i] = '-';
    i++;
    if ((mode & S_IWGRP) == S_IWGRP) local_buff[i] = 'w';
    else local_buff[i] = '-';
    i++;
    if ((mode & S_IXGRP) == S_IXGRP) local_buff[i] = 'x';
    else local_buff[i] = '-';
    i++;
    // other permissions
    if ((mode & S_IROTH) == S_IROTH) local_buff[i] = 'r';
    else local_buff[i] = '-';
    i++;
    if ((mode & S_IWOTH) == S_IWOTH) local_buff[i] = 'w';
    else local_buff[i] = '-';
    i++;
    if ((mode & S_IXOTH) == S_IXOTH) local_buff[i] = 'x';
    else local_buff[i] = '-';
    return local_buff;
}

void normls(char * path) {
    /* Taken from OSTEP ch 39 pg 12. */
    DIR *dp = opendir(path);
    assert(dp != NULL);
    struct dirent *d;
    while ((d = readdir(dp)) != NULL) {
        printf("%s\n", d->d_name);
    }
    closedir(dp);
}

void statls(char * path) {
    /* Taken from https://linux.die.net/man/3/stat */
    struct dirent  *d;
    struct stat    statbuf;
    struct passwd  *pwd;
    struct group   *grp;
    struct tm      *tm;
    char            datestring[256];

    DIR *dp = opendir(path);
    assert(dp != NULL);

    chdir(path);

    /* Loop through directory entries. */
    while ((d = readdir(dp)) != NULL) {

        /* Get entry's information. */
        if (stat(d->d_name, &statbuf) == -1)
            continue;

        /* Print if entry is a directory or a file. */
        if (S_ISDIR(statbuf.st_mode) != 0) {
            printf("d");
        } else if (S_ISREG(statbuf.st_mode) != 0) {
            printf("-");
        }

        /* Print out type, permissions, and number of links. */
        // printf("%10.10s", sperm(statbuf.st_mode));
        printf("%s", sperm(statbuf.st_mode));
        printf("%4d", statbuf.st_nlink);

        /* Print out owner's name if it is found using getpwuid(). */
        if ((pwd = getpwuid(statbuf.st_uid)) != NULL)
            printf(" %-8.8s", pwd->pw_name);
        else
            printf(" %-8d", statbuf.st_uid);

        /* Print out group name if it is found using getgrgid(). */
        if ((grp = getgrgid(statbuf.st_gid)) != NULL)
            printf(" %-8.8s", grp->gr_name);
        else
            printf(" %-8d", statbuf.st_gid);

        /* Print size of file. */
        printf(" %9jd", (intmax_t)statbuf.st_size);

        tm = localtime(&statbuf.st_mtime);

        /* Get localized date string. */
        strftime(datestring, sizeof(datestring), nl_langinfo(D_T_FMT), tm);

        printf(" %s %s\n", datestring, d->d_name);
    }
}

int main(int argc, char *argv[]) {
    if (argc == 1) {                                // ./myls
        normls(CWDS);
    } else if (argc >= 2) {
        if (strcmp(argv[1], STAT_OPTION) == 0) {
            if (argc == 2)                          // ./myls -l
                statls(CWDS);
            else if (argc == 3)                     // ./myls -l [path]
                statls(argv[2]);
        } else {                                    // ./myls [path]
            normls(argv[1]);
        }
    }

    return 0;
}
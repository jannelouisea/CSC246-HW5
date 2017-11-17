#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

#define CWDS "."
#define PDIR ".."

int level = 0;

void rls(char ** path) {

    DIR *dp = opendir(*path);
    if (dp != NULL) {
        struct dirent *d;
        while ((d = readdir(dp)) != NULL) {
            if (strcmp(d->d_name, CWDS) != 0 && strcmp(d->d_name, PDIR) != 0) {
                for (int i = 0; i < level; i++) {
                    printf("    ");
                }
                printf("%d|____%s\n", level, d->d_name);

                if (d->d_type == DT_DIR) {
                    level++;

                    char * spath;
                    spath = malloc(5000 * sizeof(char));
                    strcpy(spath, *path);
                    strcat(spath, "/");
                    strcat(spath, d->d_name);
                    // printf("sub path: %s\n", spath);

                    rls(&spath);
                }
            }
        }
    }
    level--;
    closedir(dp);
}

int main(int argc, char *argv[]) {
    char * ddir = ".";

    if (argc == 1) {
        rls(&ddir);
    } else if (argc == 2) {
        rls(&argv[1]);
    }
    return 0;
}


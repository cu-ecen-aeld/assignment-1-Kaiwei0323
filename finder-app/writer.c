#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <libgen.h>
#include <syslog.h>
#include "writer.h"

int main(int argc, char *argv[]) {

    openlog("writer",SYSLOG_OPTIONS,SYSLOG_FACILITY);

    if(validate_args(argc))
        return 1; // failed validation
    
    char *filepath = argv[1];
    char *textstring = argv[2];

    syslog(LOG_DEBUG,"Writing %s to %s",textstring,filepath);

    char *textline = malloc(strlen(textstring) + 2);
    textline = strcat(textstring, "\n");

    if (mk_dir_r(filepath)) {
        syslog(LOG_ERR,"main: mk_dir_r: %m");
        return 1;
    }

    int desc = open_file(filepath); 
    if(desc == -1) {
        syslog(LOG_ERR,"main: open_file: %m");
        return 1;
    }

    if(write(desc, textline, strlen(textline)+1) == -1) {
        syslog(LOG_ERR,"main: write: %m");
        return 1;
    }
    
    if(close_file(desc)) {
        syslog(LOG_ERR,"main: close_file: %m");
        return 1;
    }

    closelog();

    return 0;
}


int validate_args(int argc) {
    if (argc != 3 ) {
        printf("Usage: writer.sh [filepath] [textstring]\n");
        printf("Example invocation: writer.sh /tmp/aesd/assignment1/sample.txt ios\n");
        return 1;
    }
    else {
        return 0;
    }
}


int open_file(char *path) {
    int errOpen = 0;
    int fileDesc = open(path, FLAGS_OPEN, MODE_OPEN);
    if (fileDesc == -1) {
        errOpen = errno;
        if (errOpen == ENOENT) {    //"no such file or directory"
            // Create directories recursively
            mk_dir_r(path);
            // Try to open again
            fileDesc = open(path, FLAGS_OPEN, MODE_OPEN);
            errOpen = errno;
            if(fileDesc == -1)
                syslog(LOG_ERR,"open_file: open() after path directory creation: %m");
        }
        else {
            syslog(LOG_ERR,"open_file: open() before path directory creation: %m");
        }
    }
    errno = errOpen;
    return fileDesc;
}


inline int close_file(int desc) {
    return (close(desc));
}


inline int mk_dir(const char *path) {
    return mkdir(path, MODE_MKDIR);
}


int mk_dir_r(const char *path) {

    int e = 0;
    char *err_context = "mk_dir_r";
    char *parent = malloc(strlen(path)+1);
    char *p = parent; // pointer so we can free the memory later
    int dir = 0;

    for(int i=0; i<254; i++) {
        strcpy(parent, path);
        dir = open(dirname(parent),__O_DIRECTORY);
        if(dir != -1){
            // success!
            close(dir);
            free(p);
            return 0;
        }
        else {
            if (errno == ENOENT) {
                int j = 0; // implements timeout for safety
                int er = 0;
                strcpy(parent, path);
                do {
                    j++;
                    parent = dirname(parent);
                    //printf("mk_dir_r: dirname: %s\n", parent);
                    er = mk_dir(parent);
                    // Don't add anything here; errno musn't change.
                } while(er && errno == ENOENT && j<254);
                if(er) {
                    err_context = "mk_dir_r: mk_dir";
                    goto ErrorCleanup;
                }      
            }
            else {
                err_context = "open(parent,__O_DIRECTORY)";
                goto ErrorCleanup;
            }
        }
    }
    ErrorCleanup:
        e = errno;
        syslog(LOG_ERR,"Error in mk_dir_r...");
        errno = e;
        syslog(LOG_ERR,"%s: %m",err_context);
        syslog(LOG_DEBUG,"parent was: %s\n",parent);
        syslog(LOG_DEBUG,"path was: %s\n", path);
        free(p);
        errno = e;
        return -1;
}

int print_args(int argc, char *argv[]) {
    if (argc > 0) {
        for(int i = 0; i<argc; i++) {
            printf("%s, ",argv[i]);
        }
        printf("\n");
        return 0;
    }
    else
        return 1;
}

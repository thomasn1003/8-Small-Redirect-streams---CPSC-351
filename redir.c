#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>

void split_cmd(char *cmd, char **args) {

    char *token;

    int i = 0;

    token = strtok(cmd, " ");

    while (token != NULL) {

        args[i++] = token;

        token = strtok(NULL, " ");

    }

    args[i] = NULL;

}



char *resolve_path(char *cmd) {

    if (cmd[0] == '/') return cmd;  

    char *path_env = getenv("PATH");

    static char full_path[_PC_PATH_MAX];

    char *dir = strtok(path_env, ":");

    while (dir != NULL) {

        snprintf(full_path, sizeof(full_path), "%s/%s", dir, cmd);

        if (access(full_path, X_OK) == 0) { 

            return full_path;

        }

        dir = strtok(NULL, ":");

    }

    return cmd; 

}



int main(int argc, char *argv[]) {

    if (argc != 4) {

        fprintf(stderr, "Usage: %s <inp> <cmd> <out>\n", argv[0]);

        exit(1);

    }

    char *inp = argv[1];

    char *cmd = argv[2];

    char *out = argv[3];
    
    char *args[256];

    split_cmd(cmd, args);

    char *cmd_path = resolve_path(args[0]);
    
    pid_t pid = fork();

    if (pid == 0) { 

        if (strcmp(inp, "-") != 0) {

            int fd_in = open(inp, O_RDONLY);

            if (fd_in < 0) {

                perror("Failed to open input file");

                exit(1);

            }

            dup2(fd_in, STDIN_FILENO);

            close(fd_in);

        }

        if (strcmp(out, "-") != 0) {

            int fd_out = open(out, O_WRONLY | O_CREAT | O_TRUNC, 0644);

            if (fd_out < 0) {

                perror("Failed to open output file");

                exit(1);

            }

            dup2(fd_out, STDOUT_FILENO);

            close(fd_out);

        }
        

        execv(cmd_path, args);

        perror("execv failed");

        exit(1);

    } else if (pid > 0) { 

        wait(NULL); 

    } else {

        perror("fork failed");

        exit(1);
        
    }
    
    return 0;
}

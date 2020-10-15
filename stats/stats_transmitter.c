#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "stats_both.h"

pid_t start_transmitter(int fd, const char *com_, const char *file_, size_t dtau_, float prob_data_, float prob_head_, size_t capacity_){
    fprintf(stderr, "        starting transmitter, com=%s\n", com_);
    pid_t pid = fork();
    if(pid == 0) /* Child */{
        dup2(fd, STDOUT_FILENO);

        char dtau_s     [64]; sprintf(dtau_s     , "%lu", dtau_     );
        char prob_data_s[64]; sprintf(prob_data_s, "%f" , prob_data_);
        char prob_head_s[64]; sprintf(prob_head_s, "%f" , prob_head_);
        char capacity_s [64]; sprintf(capacity_s , "%lu", capacity_ );

        execl("../transmitter", "../transmitter", com_, file_, "-T", dtau_s, "-d", prob_data_s, "-h", prob_head_s, "-b", capacity_s, NULL);
        perror("execl");
        exit(1);
    } else {
        close(fd);
        return pid;
    }
}

int main(int argc, char *argv[]){
    if(argc < 2){
        fprintf(stderr, "Usage: ./stats_transmitter FROM\n");
        exit(1);
    }

    fprintf(stdout, "L,Lf,N,Ne,Nt,C,T\n");

    for(size_t i = 0; i < file_size; ++i){
        fprintf(stderr, "i=%lu, file=%s\n", i, file[i]);
        for(size_t j = 0; j < dtau_size; ++j){
            fprintf(stderr, "  j=%lu, dtau=%lu\n", j, dtau[j]);
            for(size_t k = 0; k < prob_size; ++k){
                fprintf(stderr, "    k=%lu, prob_data=%f, prob_head=%f\n", k, prob_data[k], prob_head[k]);
                for(size_t l = 0; l < capacity_size; ++l){
                    fprintf(stderr, "      l=%lu, capacity=%lu\n", l, capacity[l]);
                    
                    int pipe_transmitter[2]; pipe(pipe_transmitter);
                    pid_t transmitter_pid = start_transmitter(pipe_transmitter[1], argv[1], file[i], dtau[j], prob_data[k], prob_head[k], capacity[l]);

                    int transmitter_status;
                    fprintf(stderr, "        waiting for transmitter\n");
                    waitpid(transmitter_pid, &transmitter_status, 0);
                    if(WIFEXITED(transmitter_status)){
                        fprintf(stderr, "        transmitter exited with status %d\n", WEXITSTATUS(transmitter_status));
                        if(WEXITSTATUS(transmitter_status) != 0){
                            fprintf(stderr, "        ERROR: exited with failure\n");
                            exit(1);
                        }
                    } else {
                        fprintf(stderr, "        ERROR: transmitter did not exit\n");
                        exit(1);
                    }
                    FILE *pipe_transmitter_read = fdopen(pipe_transmitter[0], "r");
                    size_t      L ; fscanf(pipe_transmitter_read, "%lu", &L );
                    size_t      Lf; fscanf(pipe_transmitter_read, "%lu", &Lf);
                    size_t      N ; fscanf(pipe_transmitter_read, "%lu", &N );
                    size_t      Ne; fscanf(pipe_transmitter_read, "%lu", &Ne);
                    size_t      Nt; fscanf(pipe_transmitter_read, "%lu", &Nt);
                    size_t      C ; fscanf(pipe_transmitter_read, "%lu", &C );
                    suseconds_t T ; fscanf(pipe_transmitter_read, "%lu", &T );
                    fclose(pipe_transmitter_read);
                    fprintf(stdout, "%lu,%lu,%lu,%lu,%lu,%lu,%lu\n", L, Lf, N, Ne, Nt, C, T);

                    sleep(1);
                }
            }
        }
    }
    return 0;
}

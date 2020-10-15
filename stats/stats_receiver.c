#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "stats_both.h"

pid_t start_receiver(int fd, const char *com_, size_t dtau_, float prob_data_, float prob_head_, size_t capacity_){
    fprintf(stderr, "        starting receiver, com=%s\n", com_);
    pid_t pid = fork();
    if(pid == 0) /* Child */{
        dup2(fd, STDOUT_FILENO);

        char dtau_s     [64]; sprintf(dtau_s     , "%lu", dtau_     );
        char prob_data_s[64]; sprintf(prob_data_s, "%f" , prob_data_);
        char prob_head_s[64]; sprintf(prob_head_s, "%f" , prob_head_);
        char capacity_s [64]; sprintf(capacity_s , "%lu", capacity_ );

        execl("../receiver", "../receiver", com_, "-T", dtau_s, "-d", prob_data_s, "-h", prob_head_s, "-b", capacity_s, NULL);
        perror("execl");
        exit(1);
    } else {
        close(fd);
        return pid;
    }
}

int main(int argc, char *argv[]){
    if(argc < 2){
        fprintf(stderr, "Usage: ./stats_receiver TO\n");
        exit(1);
    }

    for(size_t i = 0; i < file_size; ++i){
        fprintf(stderr, "i=%lu, file=%s\n", i, file[i]);
        for(size_t j = 0; j < dtau_size; ++j){
            fprintf(stderr, "  j=%lu, dtau=%lu\n", j, dtau[j]);
            for(size_t k = 0; k < prob_size; ++k){
                fprintf(stderr, "    k=%lu, prob_data=%f, prob_head=%f\n", k, prob_data[k], prob_head[k]);
                for(size_t l = 0; l < capacity_size; ++l){
                    fprintf(stderr, "      l=%lu, capacity=%lu\n", l, capacity[l]);

                    int pipe_receiver[2]; pipe(pipe_receiver);
                    pid_t receiver_pid = start_receiver(pipe_receiver[1], argv[1], dtau[j], prob_data[k], prob_head[k], capacity[l]);

                    int receiver_status;
                    fprintf(stderr, "        waiting for receiver\n");
                    waitpid(receiver_pid, &receiver_status, 0);
                    if(WIFEXITED(receiver_status)){
                        fprintf(stderr, "        receiver exited with status %d\n", WEXITSTATUS(receiver_status));
                        if(WEXITSTATUS(receiver_status) != 0){
                            fprintf(stderr, "        ERROR: exited with failure\n");
                            exit(1);
                        }
                    } else {
                        fprintf(stderr, "        ERROR: receiver did not exit\n");
                        exit(1);
                    }
                    close(pipe_receiver[0]);

                    system("rm -f *.jpg");
                }
            }
        }
    }
    return 0;
}

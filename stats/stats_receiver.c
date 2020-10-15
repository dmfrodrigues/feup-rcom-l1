#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

pid_t start_receiver(int fd, const char *com, const char *filepath, size_t baud_rate, float prob_data, float prob_head, size_t retransmissions, size_t size, size_t timeout, size_t tau, size_t verbosity){
    pid_t pid = fork();
    if(pid == 0) /* Child */{
        dup2(fd, STDOUT_FILENO);

        char baud_rate_s      [64]; sprintf(baud_rate_s      , "%lu", baud_rate      );
        char prob_data_s      [64]; sprintf(prob_data_s      , "%f" , prob_data      );
        char prob_head_s      [64]; sprintf(prob_head_s      , "%f" , prob_head      );
        char retransmissions_s[64]; sprintf(retransmissions_s, "%lu", retransmissions);
        char size_s           [64]; sprintf(size_s           , "%lu", size           );
        char timeout_s        [64]; sprintf(timeout_s        , "%lu", timeout        );
        char tau_s            [64]; sprintf(tau_s            , "%lu", tau            );
        char verbosity_s      [64]; sprintf(verbosity_s      , "%lu", verbosity      );

        execl("../receiver", "../receiver", com,
            "-b", baud_rate_s,
            "-d", prob_data_s,
            "-h", prob_head_s,
            "-r", retransmissions_s,
            "-s", size_s,
            "-t", timeout_s,
            "-T", tau_s,
            "-v", verbosity_s,
            NULL
        );
        perror("execl");
        exit(1);
    } else {
        close(fd);
        return pid;
    }
}

int main(int argc, char *argv[]){
    if(argc < 3){
        fprintf(stderr, "Usage: ./stats_receiver TO\n");
        exit(1);
    }

    const char *stats_file_path = argv[1];
    const char *com             = argv[2];

    FILE *stats_file = fopen(stats_file_path, "r");
    char filepath[1024];
    size_t baud_rate;
    float prob_data;
    float prob_head;
    size_t retransmissions;
    size_t size;
    size_t timeout;
    size_t tau;
    size_t verbosity;
    fprintf(stderr, "                     File path   Rate  Pr data  Pr head Try Size  Timeout    Tau V Ret\n");
    fprintf(stderr, "--------------------------------------------------------------------------------------\n");
    while(fscanf(stats_file, "%s %lu %f %f %lu %lu %lu %lu %lu",
      filepath,
      &baud_rate,
      &prob_data,
      &prob_head,
      &retransmissions,
      &size,
      &timeout,
      &tau,
      &verbosity) == 9){
        fprintf(stderr, "%30s %6lu %8f %8f %3lu %4lu %8lu %6lu %1lu ",
            filepath,
            baud_rate,
            prob_data,
            prob_head,
            retransmissions,
            size,
            timeout,
            tau,
            verbosity
        );

        int pipe_receiver[2]; pipe(pipe_receiver);
        pid_t receiver_pid = start_receiver(pipe_receiver[1], com, filepath, baud_rate, prob_data, prob_head, retransmissions, size, timeout, tau, verbosity);

        int receiver_status;
        waitpid(receiver_pid, &receiver_status, 0);
        if(WIFEXITED(receiver_status)){
            fprintf(stderr, "%3d\n", WEXITSTATUS(receiver_status));
            if(WEXITSTATUS(receiver_status) != 0){
                fprintf(stderr, "\n    ERROR: exited with failure\n");
                exit(1);
            }
        } else {
            fprintf(stderr, "\n    ERROR: receiver did not exit\n");
            exit(1);
        }
        close(pipe_receiver[0]);

        system("rm -f *.jpg");
    }

    return 0;
}

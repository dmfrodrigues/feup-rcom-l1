#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

pid_t start_transmitter(int fd, const char *com, const char *filepath, size_t baud_rate, float prob_data, float prob_head, size_t retransmissions, size_t size, size_t timeout, size_t tau, size_t verbosity){
    fprintf(stderr, "    starting transmitter, com=%s\n", com);
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
        
        execl("../transmitter", "../transmitter", com, filepath,
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
        fprintf(stderr, "Usage: ./stats_transmitter STATS_FILE FROM\n");
        exit(1);
    }
    const char *stats_file_path = argv[1];
    const char *com             = argv[2];

    fprintf(stdout, "L,Lf,N,Ne,Nt,C,T\n");

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
        printf("Sending %s %lu %f %f %lu %lu %lu %lu %lu\n",
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
        
        int pipe_transmitter[2]; pipe(pipe_transmitter);
        pid_t transmitter_pid = start_transmitter(pipe_transmitter[1], com, filepath, baud_rate, prob_data, prob_head, retransmissions, size, timeout, tau, verbosity);

        int transmitter_status;
        fprintf(stderr, "    waiting for transmitter\n");
        waitpid(transmitter_pid, &transmitter_status, 0);
        if(WIFEXITED(transmitter_status)){
            fprintf(stderr, "    transmitter exited with status %d\n", WEXITSTATUS(transmitter_status));
            if(WEXITSTATUS(transmitter_status) != 0){
                fprintf(stderr, "    ERROR: exited with failure\n");
                exit(1);
            }
        } else {
            fprintf(stderr, "    ERROR: transmitter did not exit\n");
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

    return 0;
}

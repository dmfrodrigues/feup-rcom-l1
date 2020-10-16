#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define SECONDS_TO_NANOS 1000000000

size_t count_lines(const char *filepath){
    size_t ret = 0;

    FILE *file = fopen(filepath, "r");
    char buf[1024];
    while(fscanf(file, "%s", buf) == 1) ++ret;
    fclose(file);

    return ret/9;
}

pid_t start_transmitter(
  int fd,
  const char *com,
  const char *filepath,
  size_t baudrate,
  float prob_data,
  float prob_head,
  size_t retransmissions,
  size_t size,
  size_t timeout,
  size_t tau,
  size_t verbosity){
    pid_t pid = fork();
    if(pid == 0) /* Child */{
        dup2(fd, STDOUT_FILENO);

        char baud_rate_s      [64]; sprintf(baud_rate_s      , "%lu", baudrate      );
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

    const size_t num_lines = count_lines(stats_file_path);

    fprintf(stdout, "file,C,pd,ph,retransmissions,size,timeout,dtau,L,Lf,N,Ne,Nt,C,T\n");

    FILE *stats_file = fopen(stats_file_path, "r");
    char filepath[1024];
    size_t baudrate;
    float prob_data;
    float prob_head;
    size_t retransmissions;
    size_t size;
    size_t timeout;
    size_t tau;
    size_t verbosity;
    fprintf(stderr, "                     File path   Rate  Pr data  Pr head "
                    "Try Size  Timeout    Tau V Ret        T\n");
    fprintf(stderr, "--------------------------------------------------------"
                    "---------------------------------------\n");

    size_t idx_line = 0;
    struct timespec start; clock_gettime(CLOCK_REALTIME, &start);
    while(fscanf(stats_file, "%s %lu %f %f %lu %lu %lu %lu %lu",
      filepath,
      &baudrate,
      &prob_data,
      &prob_head,
      &retransmissions,
      &size,
      &timeout,
      &tau,
      &verbosity) == 9){
        fprintf(stderr, "%30s %6lu %8f %8f %3lu %4lu %8lu %6lu %1lu ",
            filepath,
            baudrate,
            prob_data,
            prob_head,
            retransmissions,
            size,
            timeout,
            tau,
            verbosity
        );
        
        int pipe_transmitter[2]; pipe(pipe_transmitter);
        pid_t transmitter_pid = start_transmitter(
            pipe_transmitter[1],
            com,
            filepath,
            baudrate,
            prob_data,
            prob_head,
            retransmissions,
            size,
            timeout,
            tau,
            verbosity
        );

        int transmitter_status;
        waitpid(transmitter_pid, &transmitter_status, 0);
        if(WIFEXITED(transmitter_status)){
            fprintf(stderr, "%3d ", WEXITSTATUS(transmitter_status));
            if(WEXITSTATUS(transmitter_status) != 0){
                fprintf(stderr, "\n    ERROR: exited with failure\n");
                exit(1);
            }
        } else {
            fprintf(stderr, "\n    ERROR: transmitter did not exit\n");
            exit(1);
        }

        ++idx_line;
        struct timespec now; clock_gettime(CLOCK_REALTIME, &now);
        float time_now =        (now.tv_sec  - start.tv_sec ) +
                         (float)(now.tv_nsec - start.tv_nsec)/(float)SECONDS_TO_NANOS;
        float total_time = (time_now*num_lines)/idx_line;
        float left_time  = total_time-time_now;

        FILE *pipe_transmitter_read = fdopen(pipe_transmitter[0], "r");
        size_t      L ; fscanf(pipe_transmitter_read, "%lu", &L );
        size_t      Lf; fscanf(pipe_transmitter_read, "%lu", &Lf);
        size_t      N ; fscanf(pipe_transmitter_read, "%lu", &N );
        size_t      Ne; fscanf(pipe_transmitter_read, "%lu", &Ne);
        size_t      Nt; fscanf(pipe_transmitter_read, "%lu", &Nt);
        size_t      C ; fscanf(pipe_transmitter_read, "%lu", &C );
        suseconds_t T ; fscanf(pipe_transmitter_read, "%lu", &T );
        fclose(pipe_transmitter_read);
        fprintf(stdout, "%s,%lu,%.10f,%.10f,%lu,%lu,%lu,%lu,"
                        "%lu,%lu,%lu,%lu,%lu,%lu,%lu\n",
            filepath, baudrate, prob_data, prob_head, retransmissions, size, timeout, tau,
            L, Lf, N, Ne, Nt, C, T);
        fprintf(stderr, "%8lu %6.2f%% (%6.1fs; %6.1fs left)\n",
            T, (100.0*idx_line)/num_lines, total_time, left_time);

        usleep(100000);
    }

    return 0;
}

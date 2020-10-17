// Copyright (C) 2020 Diogo Rodrigues, Breno Pimentel
// Distributed under the terms of the GNU General Public License, version 3

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define SECONDS_TO_NANOS 1000000000

/**
 * @ingroup stats_separate
 * @brief Count number of non-empty lines in file.
 * 
 * @param filepath      Path to file to analyse
 * @return size_t       Number of non-empty lines
 */
size_t count_lines(const char *filepath){
    size_t ret = 0;

    FILE *file = fopen(filepath, "r");
    char buf[1024];
    while(fscanf(file, "%s", buf) == 1) ++ret;
    fclose(file);

    return ret/9;
}

/**
 * @ingroup stats_separate
 * @brief Start transmiter
 * 
 * @param fd                File descriptor where output should be written to
 *                          (typically a pipe)
 * @param com               COM to be used
 * @param filepath          Path of file to be transferred
 * @param baud_rate         Baud rate
 * @param prob_data         Probability of bit swap in data
 * @param prob_head         Probability of bit swap in header
 * @param retransmissions   Number of retransmissions
 * @param size              Maximum frame size
 * @param timeout           Timeout
 * @param tau               Artificial propagation delay
 * @param verbosity         Verbosity
 * @return pid_t            ID of child process that was created to run transmitter
 */
pid_t start_transmitter(
  int fd,
  const char *com,
  const char *filepath,
  const char *baud_rate,
  const char *prob_data,
  const char *prob_head,
  const char *retransmissions,
  const char *size,
  const char *timeout,
  const char *tau,
  const char *verbosity){
    pid_t pid = fork();
    if(pid == 0) /* Child */{
        dup2(fd, STDOUT_FILENO);

        execl("../transmitter", "../transmitter", com, filepath,
            "-b", baud_rate,
            "-d", prob_data,
            "-h", prob_head,
            "-r", retransmissions,
            "-s", size,
            "-t", timeout,
            "-T", tau,
            "-v", verbosity,
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
    char filepath       [1024];
    char baudrate       [1024];
    char prob_data      [1024];
    char prob_head      [1024];
    char retransmissions[1024];
    char size           [1024];
    char timeout        [1024];
    char tau            [1024];
    char verbosity      [1024];
    fprintf(stderr, "                     File path   Rate  Pr data  Pr head "
                    "Try Size  Timeout    Tau V Ret        T\n");
    fprintf(stderr, "--------------------------------------------------------"
                    "---------------------------------------\n");

    size_t idx_line = 0;
    struct timespec start; clock_gettime(CLOCK_REALTIME, &start);
    while(fscanf(stats_file, "%s %s %s %s %s %s %s %s %s",
      filepath,
      baudrate,
      prob_data,
      prob_head,
      retransmissions,
      size,
      timeout,
      tau,
      verbosity) == 9){
        fprintf(stderr, "%30s %6s %8s %8s %3s %4s %8s %6s %1s ",
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
        fprintf(stdout, "%s,%s,%s,%s,%s,%s,%s,%s,"
                        "%lu,%lu,%lu,%lu,%lu,%lu,%lu\n",
            filepath, baudrate, prob_data, prob_head, retransmissions, size, timeout, tau,
            L, Lf, N, Ne, Nt, C, T);
        fprintf(stderr, "%8lu %6.2f%% (%6.1fs; %6.1fs left)\n",
            T, (100.0*idx_line)/num_lines, total_time, left_time);

        usleep(100000);
    }

    return 0;
}

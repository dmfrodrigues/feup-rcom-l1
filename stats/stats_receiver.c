// Copyright (C) 2020 Diogo Rodrigues, Breno Pimentel
// Distributed under the terms of the GNU General Public License, version 3

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/**
 * @ingroup stats_separate
 * @brief Start receiver
 * 
 * @param fd                File descriptor where output should be written to
 *                          (typically a pipe)
 * @param com               COM to be used
 * @param filepath          Path of file to be transferred (not used in this case)
 * @param baud_rate         Baud rate
 * @param prob_data         Probability of bit swap in data
 * @param prob_head         Probability of bit swap in header
 * @param retransmissions   Number of retransmissions
 * @param size              Maximum frame size
 * @param timeout           Timeout
 * @param tau               Artificial propagation delay
 * @param verbosity         Verbosity
 * @return pid_t            ID of child process that was created to run receiver
 */
pid_t start_receiver(
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

        execl("../receiver", "../receiver", com,
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
        fprintf(stderr, "Usage: ./stats_receiver TO\n");
        exit(1);
    }

    const char *stats_file_path = argv[1];
    const char *com             = argv[2];

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
                    "Try Size  Timeout    Tau V Ret\n");
    fprintf(stderr, "--------------------------------------------------------"
                    "------------------------------\n");
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
        fprintf(stderr, "%30s %6s %8s %8s %3s %4s %8s %6s %s ",
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

        int pipe_receiver[2]; pipe(pipe_receiver);
        pid_t receiver_pid = start_receiver(
            pipe_receiver[1],
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

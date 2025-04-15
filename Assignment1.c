#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <mqueue.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <errno.h>


// POSIX message queue names
#define TASK_QUEUE "/task_queue"
#define RESULT_QUEUE "/result_queue"


// Structure
typedef struct {
    int worker_id;
    pid_t pid;
    int task_count;
    int total_sleep;
} result_msg;


#define MAX_MSG_SIZE sizeof(result_msg)


// Set up signal handling to automatically restart interrupted system calls
void setup_signal_handling() {
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;      // Ignore signals
    sa.sa_flags = SA_RESTART;     // Restart system calls if interrupted
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
}


// Worker process function
void worker_process(int worker_id) {
    setup_signal_handling();  // Ensure signals don't break blocking calls


    // Open the task queue for reading and result queue for writing
    mqd_t task_q = mq_open(TASK_QUEUE, O_RDONLY);
    mqd_t result_q = mq_open(RESULT_QUEUE, O_WRONLY);
    if (task_q == -1 || result_q == -1) {
        perror("Worker mq_open");
        exit(EXIT_FAILURE);
    }

    char msg[32];  			// Buffer for task messages
    int task_count = 0;
    int total_sleep = 0;

    while (1) {


        // Blocking receive: wait for a task
        if (mq_receive(task_q, msg, sizeof(msg), NULL) > 0) {
            int effort = atoi(msg);  // Convert task string to int
            if (effort == 0) break;  // 0 means termination signal
            task_count++;
            total_sleep += effort;

            printf("%s | Worker #%02d | Received task with effort %d\n", __TIME__, worker_id, effort);
            sleep(effort);  // Simulate work
        }
    }


    // Prepare and send result message back to the ventilator
    result_msg res = { worker_id, getpid(), task_count, total_sleep };
    mq_send(result_q, (char*)&res, sizeof(res), 0);


    // Clean up
    mq_close(task_q);
    mq_close(result_q);
    exit(0);
}


// Main function
int main(int argc, char* argv[]) {
    setup_signal_handling();  // Ensure signals don't interrupt blocking calls


    // Parse command-line arguments
    int opt, workers = 0, tasks = 0, queue_size = 0;
    while ((opt = getopt(argc, argv, "w:t:s:")) != -1) {
        switch (opt) {
        case 'w': workers = atoi(optarg); break;
        case 't': tasks = atoi(optarg); break;
        case 's': queue_size = atoi(optarg); break;
        default:
            fprintf(stderr, "Usage: %s -w <workers> -t <tasks> -s <queue_size>\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }


    // Validate input
    if (workers <= 0 || tasks <= 0 || queue_size <= 0) {
        fprintf(stderr, "All parameters must be positive integers.\n");
        exit(EXIT_FAILURE);
    }


    // Define message queue attributes
    struct mq_attr attr = {
        .mq_flags = 0,
        .mq_maxmsg = queue_size,
        .mq_msgsize = MAX_MSG_SIZE,
        .mq_curmsgs = 0
    };


    // Clean up old queues in case they still exist
    mq_unlink(TASK_QUEUE);
    mq_unlink(RESULT_QUEUE);


    // Create new task and result queues
    mqd_t task_q = mq_open(TASK_QUEUE, O_CREAT | O_WRONLY, 0644, &attr);
    mqd_t result_q = mq_open(RESULT_QUEUE, O_CREAT | O_RDONLY, 0644, &attr);
    if (task_q == -1 || result_q == -1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    printf("%s | Ventilator | Starting %d workers for %d tasks and a queue size of %d\n", __TIME__, workers, tasks, queue_size);


    // Fork worker processes
    for (int i = 0; i < workers; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            worker_process(i + 1);  // Child runs worker logic
        }
        else if (pid > 0) {
            printf("%s | Worker #%02d | Started worker PID %d\n", __TIME__, i + 1, pid);
        }
        else {
            perror("fork");
            exit(EXIT_FAILURE);
        }
    }


    // Seed random number generator
    srand(time(NULL));


    // Send tasks to the queue
    printf("%s | Ventilator | Distributing tasks\n", __TIME__);
    for (int i = 0; i < tasks; i++) {
        int effort = (rand() % 10) + 1;  // Random effort between 1 and 10
        char msg[32];
        snprintf(msg, sizeof(msg), "%d", effort);
        mq_send(task_q, msg, strlen(msg) + 1, 0);
        printf("%s | Ventilator | Queuing task #%d with effort %d\n", __TIME__, i + 1, effort);
    }


    // Send termination tasks (effort = 0) to all workers
    printf("%s | Ventilator | Sending termination tasks\n", __TIME__);
    for (int i = 0; i < workers; i++) {
        mq_send(task_q, "0", 2, 0);
    }


    // Wait for results from all workers
    printf("%s | Ventilator | Waiting for workers to terminate\n", __TIME__);
    for (int i = 0; i < workers; i++) {
        char buffer[MAX_MSG_SIZE];
        ssize_t bytes = mq_receive(result_q, buffer, MAX_MSG_SIZE, NULL);
        if (bytes > 0) {
            result_msg res;
            memcpy(&res, buffer, sizeof(result_msg));
            printf("%s | Ventilator | Worker %d processed %d tasks in %d seconds\n",
                __TIME__, res.worker_id, res.task_count, res.total_sleep);
        }
        else {
            perror("mq_receive");
        }
        wait(NULL);  // Wait for the worker process to exit
    }


    // Cleanup message queues
    mq_close(task_q);
    mq_close(result_q);
    mq_unlink(TASK_QUEUE);
    mq_unlink(RESULT_QUEUE);

    return 0;
}


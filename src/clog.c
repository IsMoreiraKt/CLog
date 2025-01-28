#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <uuid/uuid.h>

/** Static variables */
static FILE *log_file = NULL; /**< Pointer to the current log file. */
static pthread_mutex_t log_mutex =
    PTHREAD_MUTEX_INITIALIZER; /**< Mutex for thread-safe logging. */
static LogConfig log_config = {
    .log_to_stdout = 1,     /**< Default: log to stdout. */
    .log_to_stderr = 0,     /**< Default: do not log to stderr. */
    .log_file_path = NULL,  /**< Default: no log file. */
    .rotation_interval = 0, /**< Default: no file rotation. */
    .ignored_domains = NULL /**< Default: no ignored domains. */
};
static time_t last_rotation_time =
    0; /**< Timestamp of the last log file rotation. */
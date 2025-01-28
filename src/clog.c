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

/**
 * @brief Converts a log level to a human-readable string with formatting.
 *
 * @param level The log level to convert.
 * @return A string representing the log level.
 */
static const char *log_level_str(LogLevel level) {
  switch (level) {
  case LOG_LEVEL_ERROR:
    return "   \033[1;31mERROR\033[0m";
  case LOG_LEVEL_CRITICAL:
    return "\033[1;35mCRITICAL\033[0m";
  case LOG_LEVEL_WARNING:
    return " \033[1;33mWARNING\033[0m";
  case LOG_LEVEL_MESSAGE:
    return " \033[1;34mMESSAGE\033[0m";
  case LOG_LEVEL_INFO:
    return "    \033[1;32mINFO\033[0m";
  case LOG_LEVEL_DEBUG:
    return "   \033[1;32mDEBUG\033[0m";
  case LOG_LEVEL_TRACE:
    return "   \033[1;36mTRACE\033[0m";
  default:
    return " UNKNOWN";
  }
}
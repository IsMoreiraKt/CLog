#include "clog.h"
#include <pthread.h>
#include <stdarg.h>
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

/**
 * @brief Checks if a domain is in the ignored domains list.
 *
 * @param domain The domain to check.
 * @return 1 if the domain is ignored, 0 otherwise.
 */
static int is_ignored_domain(const char *domain) {
  if (!log_config.ignored_domains || !domain)
    return 0;

  for (char **d = log_config.ignored_domains; *d; d++) {
    if (strcmp(*d, domain) == 0)
      return 1;
  }

  return 0;
}

/**
 * @brief Generates a UUID and writes it to a buffer.
 *
 * @param uuid_buffer The buffer to store the UUID.
 * @param size The size of the buffer.
 */
static void generate_uuid(char *uuid_buffer, size_t size) {
  uuid_t uuid;
  uuid_generate(uuid);
  uuid_unparse(uuid, uuid_buffer);
}

/**
 * @brief Resolves placeholders in the log file path (e.g., {uuid},
 * {timestamp}).
 *
 * @param resolved_path The buffer to store the resolved path.
 * @param size The size of the buffer.
 */
static void resolve_file_path(char *resolved_path, size_t size) {
  if (!log_config.log_file_path)
    return;

  snprintf(resolved_path, size, "%s", log_config.log_file_path);

  char uuid[37];
  generate_uuid(uuid, sizeof(uuid));
  char *uuid_placeholder = strstr(resolved_path, "{uuid}");

  if (uuid_placeholder) {
    snprintf(uuid_placeholder, size - (uuid_placeholder - resolved_path),
             "%s%s", uuid, uuid_placeholder + 6);
  }

  time_t now = time(NULL);
  struct tm *tm_now = localtime(&now);
  char timestamp[64];
  strftime(timestamp, sizeof(timestamp), "%Y%m%d_%H%M%S", tm_now);
  char *timestamp_placeholder = strstr(resolved_path, "{timestamp}");

  if (timestamp_placeholder) {
    snprintf(timestamp_placeholder,
             size - (timestamp_placeholder - resolved_path), "%s%s", timestamp,
             timestamp_placeholder + 11);
  }
}

/**
 * @brief Initializes the logging system with the provided configuration.
 *
 * @param config Pointer to a configuration structure containing logging
 * preferences.
 */
void log_init(LogConfig *config) {
  if (config) {
    if (config->log_to_stdout || config->log_to_stderr ||
        config->log_file_path || config->rotation_interval ||
        config->ignored_domains) {
      log_config = *config;
    }
  }

  if (log_config.log_file_path) {
    char resolved_path[1024];
    resolve_file_path(resolved_path, sizeof(resolved_path));

    log_file = fopen(resolved_path, "a");

    if (!log_file) {
      perror("Failed to open log file");
      log_file = NULL;
    }
  }

  last_rotation_time = time(NULL);
}

/**
 * @brief Rotates the log file based on the configured interval.
 */
static void rotate_log_file() {
  if (!log_config.rotation_interval || !log_config.log_file_path)
    return;

  time_t now = time(NULL);

  if (difftime(now, last_rotation_time) < log_config.rotation_interval * 60)
    return;

  pthread_mutex_lock(&log_mutex);

  if (log_file)
    fclose(log_file);

  char resolved_path[1024];
  resolve_file_path(resolved_path, sizeof(resolved_path));

  log_file = fopen(resolved_path, "a");

  if (!log_file) {
    perror("Failed to rotate log file");
    log_file = NULL;
  }

  last_rotation_time = now;

  pthread_mutex_unlock(&log_mutex);
}

/**
 * @brief Logs a message to the configured outputs.
 *
 * @param domain The domain associated with the log message.
 * @param level The severity level of the log message.
 * @param format A printf-like format string.
 * @param ... Variadic arguments for the formatted message.
 */
void log_message(const char *domain, LogLevel level, const char *format, ...) {
  if (is_ignored_domain(domain))
    return;

  rotate_log_file();

  time_t now = time(NULL);
  struct tm *tm_now = localtime(&now);
  char time_buffer[64];
  strftime(time_buffer, sizeof(time_buffer), "%H:%M:%S", tm_now);

  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  int microsecond = ts.tv_nsec / 1000;

  va_list args;
  va_start(args, format);
  char message[1024];
  vsnprintf(message, sizeof(message), format, args);
  va_end(args);

  pthread_mutex_lock(&log_mutex);

  if (log_config.log_to_stdout) {
    fprintf(stdout, "%s.%06d  %28s: %s: %s\n", time_buffer, microsecond,
            domain ? domain : "General", log_level_str(level), message);
    fflush(stdout);
  }

  if (log_config.log_to_stderr) {
    fprintf(stderr, "%s.%06d  %28s: %s: %s\n", time_buffer, microsecond,
            domain ? domain : "General", log_level_str(level), message);
    fflush(stderr);
  }

  if (log_file) {
    fprintf(log_file, "%s.%06d  %28s: %s: %s\n", time_buffer, microsecond,
            domain ? domain : "General", log_level_str(level), message);
    fflush(log_file);
  }

  pthread_mutex_unlock(&log_mutex);
}

/**
 * @brief Closes the logging system and releases resources.
 */
void log_close(void) {
  pthread_mutex_lock(&log_mutex);

  if (log_file)
    fclose(log_file);

  log_file = NULL;
  pthread_mutex_unlock(&log_mutex);
}
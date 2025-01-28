#ifndef CLOG_H
#define CLOG_H

/**
 * @enum LogLevel
 * @brief Represents the available log levels.
 */
typedef enum {
  LOG_LEVEL_ERROR,    /**< Errors that require immediate attention. */
  LOG_LEVEL_CRITICAL, /**< Critical issues that may affect functionality. */
  LOG_LEVEL_WARNING,  /**< Warnings about potential issues. */
  LOG_LEVEL_MESSAGE,  /**< General informational messages. */
  LOG_LEVEL_INFO,     /**< Information about program operation. */
  LOG_LEVEL_DEBUG,    /**< Debugging messages. */
  LOG_LEVEL_TRACE     /**< Detailed trace messages for tracking. */
} LogLevel;

/**
 * @struct LogConfig
 * @brief Configuration structure for the logging system.
 *
 * @var LogConfig::log_to_stdout
 * Indicates whether logs should be sent to standard output.
 *
 * @var LogConfig::log_to_stderr
 * Indicates whether logs should be sent to standard error output.
 *
 * @var LogConfig::log_file_path
 * Path to the file where logs will be stored.
 *
 * @var LogConfig::rotation_interval
 * File rotation interval in minutes.
 *
 * @var LogConfig::ignored_domains
 * Array of log domains to be ignored.
 */
typedef struct {
  int log_to_stdout;   /**< Enable/disable logging to standard output. */
  int log_to_stderr;   /**< Enable/disable logging to standard error output. */
  char *log_file_path; /**< Path to the log file. */
  int rotation_interval;  /**< Log file rotation interval (in minutes). */
  char **ignored_domains; /**< List of domains to be ignored in logging. */
} LogConfig;

/**
 * @brief Initializes the logging system with the provided configuration.
 *
 * @param config Pointer to a configuration structure containing log
 * preferences.
 */
void log_init(LogConfig *config);

#endif // CLOG_H
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

#endif // CLOG_H
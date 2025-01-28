# CLog

<img
	src="./assets/logo.png"
	alt="logo"
	align="left"
/>

CLog is a lightweight and flexible C logging library that supports formatted logs with severity levels, file rotation, domain filtering and thread safety. It's designed to be highly configurable yet simple to use, making it an excellent choice for logging in both small and complex C projects.

<br><br>

## How to Install
To install CLog, clone the repository and use the provided Makefile to copy the library files to the appropriate system directories.

1. Clone the repository:
```bash
git clone https://github.com/IsMoreiraKt/CLog
```

2. Navigate to the project directory:
```bash
cd CLog
```

3. Build and install the library:
```bash
# unix
sudo make install

# windows
make install
```

> **Note for Linux Users:** Add /usr/local/lib to your LD_LIBRARY_PATH in your shell (e.g., .zshrc or .bashrc):
```bash
echo 'export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH' >> ~/.zshrc
source ~/.zshrc
```

## How to Use
### Basic Usage
Using CLog involves initializing the library, logging messages, and closing the library when done. Here’s a basic example:

```c
#include <clog.h>

int main() {
  log_init(NULL); // Use default configuration
  log_message(NULL, LOG_LEVEL_INFO, "CLog has been initialized!");
  log_close(); // Free resources
  return 0;
}
```

If no configuration is provided (log_init(NULL)), CLog defaults to logging to stdout with no log file or domain filtering.

### Advanced Configuration
CLog allows you to fully customize its behavior through the LogConfig struct. You can configure it in a modular way by setting only the options you need.

##### Example: Configuring Logs to File
```c
#include <clog.h>

int main() {
  LogConfig config = {
      .log_to_stdout = 0,
      .log_to_stderr = 0,
      .log_file_path = "logs/application.log",
      .rotation_interval = 0 // No rotation
  };

  log_init(&config);
  log_message("App", LOG_LEVEL_INFO, "Logs are now written to a file!");
  log_close();
  return 0;
}
```

##### Example: Combining Outputs
You can enable multiple outputs at once:

```c
LogConfig config = {
  .log_to_stdout = 1,
  .log_to_stderr = 1,
  .log_file_path = "logs/{timestamp}_log.txt", // Supports placeholders
  .rotation_interval = 30 // Rotate log file every 30 minutes
};

log_init(&config);
```

##### Example: Filtering Domains
Exclude specific log domains from being logged:

```c
char *ignored[] = {"Debug", "Internal", NULL};

LogConfig config = {
  .ignored_domains = ignored
};

log_init(&config);
log_message("Debug", LOG_LEVEL_DEBUG, "This won't be logged.");
log_message("App", LOG_LEVEL_INFO, "This will be logged.");
```

### Severity Levels
CLog supports the following log levels, each with distinct formatting:
- **LOG_LEVEL_ERROR**: Critical errors that require immediate attention.
- **LOG_LEVEL_CRITICAL**: High-severity issues that may impact functionality.
- **LOG_LEVEL_WARNING**: Warnings about potential issues.
- **LOG_LEVEL_MESSAGE**: General informational messages.
- **LOG_LEVEL_INFO**: Operational information.
- **LOG_LEVEL_DEBUG**: Debugging messages for development.
- **LOG_LEVEL_TRACE**: Detailed trace logs for troubleshooting.

```c
log_message("System", LOG_LEVEL_ERROR, "Disk is full!");
log_message("App", LOG_LEVEL_INFO, "Application started successfully.");
log_message(NULL, LOG_LEVEL_TRACE, "Detailed trace log.");
```

### Modular Configuration
The library is designed to allow modular configuration. If a specific option is omitted, CLog will use its default behavior:

- **Default Outputs**: Logs to stdout only.
- **File Rotation**: Disabled by default.
- **Domain Filtering**: All domains are logged unless explicitly ignored.
- **Log Level Formatting**: Predefined and consistent.

You can choose to configure only specific parts of the system without needing to define the full LogConfig structure.

### Full Example
```c
#include <stdio.h>
#include <clog.h>

int main() {
  // Example configuration
  char *ignored_domains[] = {"Debug", NULL};
  LogConfig config = {
    .log_to_stdout = 1,
    .log_to_stderr = 1,
    .log_file_path = "logs/app_{uuid}.log",
    .rotation_interval = 15, // Rotate file every 15 minutes
    .ignored_domains = ignored_domains
  };

  log_init(&config);

  log_message("App", LOG_LEVEL_INFO, "Application initialized.");
  log_message("Debug", LOG_LEVEL_DEBUG, "This message will not be logged.");
  log_message("App", LOG_LEVEL_WARNING, "Low memory warning!");

  log_close();
  return 0;
}
```

## Compilation
Once installed, compile your program by linking to the clog library and including its headers:

```bash
# unix 
gcc -I/usr/local/include -L/usr/local/lib -o program main.c -lclog

# windows
gcc -o my_program main.c -IC:\Program Files\clog\include -LC:\Program Files\clog\bin -lclog
```

## Features
- **Thread-Safe Logging**: Safely logs from multiple threads using mutexes.
- **Dynamic File Paths**: Supports placeholders like {uuid} and {timestamp} in file paths.
- **Flexible Output**: Logs to stdout, stderr, and files simultaneously.
- **Domain Filtering**: Ignore logs from specified domains.
- **Log Rotation**: Automatically rotate log files based on time intervals.
- **Rich Log Levels**: Supports multiple log levels with clear formatting.

## License
This project is licensed under the LGPL-2.1 license. See the [LICENSE](./LICENSE) file for details.

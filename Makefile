#############################
#      GLOBAL SETTINGS      #
#############################
LIBRARY_NAME = clog
PREFIX = /usr/local
INCLUDE_DIR = include
HEADER = include/clog.h
SRC_DIR = src
LIBRARY_SRC = $(SRC_DIR)/clog.c
LIBRARY_OBJ = $(LIBRARY_SRC:.c=.o)
LIBRARY_SUFFIX = .so
LIBRARY_PATH = $(PREFIX)/lib
UNAME := $(shell uname)


#############################
#  COMPATIBILITY SETTINGS   #
#############################
ifeq ($(UNAME), Linux)
    INSTALL_CMD = cp
    RM_CMD = rm -f
    MKDIR_CMD = mkdir -p
    LIBRARY_SUFFIX = .so
    LDFLAGS = -shared -luuid
    CFLAGS = -fPIC
endif

ifeq ($(UNAME), Darwin)
    INSTALL_CMD = cp
    RM_CMD = rm -f
    MKDIR_CMD = mkdir -p
    LIBRARY_SUFFIX = .dylib
    LDFLAGS = -shared -luuid
    CFLAGS = -fPIC
endif

ifeq ($(OS), Windows_NT)
    INSTALL_CMD = copy
    RM_CMD = del /f /q
    MKDIR_CMD = mkdir
    LIBRARY_SUFFIX = .dll
    LIBRARY_PATH = $(PREFIX)/bin
    INCLUDE_DIR = C:\Program Files\clog\include
    LDFLAGS = -shared -luuid
    CFLAGS = -DWIN32
endif


#############################
#           RULES           #
#############################
all: lib$(LIBRARY_NAME)$(LIBRARY_SUFFIX) install

lib$(LIBRARY_NAME)$(LIBRARY_SUFFIX): $(LIBRARY_OBJ)
	@echo "Compiling the dynamic library..."
	$(CC) $(LDFLAGS) -o $@ $(LIBRARY_OBJ)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -I$(PREFIX)/include -c $< -o $@

install: lib$(LIBRARY_NAME)$(LIBRARY_SUFFIX)
	@echo "Installing header and library..."
	$(MKDIR_CMD) $(PREFIX)/$(INCLUDE_DIR)
	@if [ ! -f $(PREFIX)/$(INCLUDE_DIR)/clog.h ]; then cp $(HEADER) $(PREFIX)/$(INCLUDE_DIR); fi
	$(MKDIR_CMD) $(LIBRARY_PATH)
	$(INSTALL_CMD) lib$(LIBRARY_NAME)$(LIBRARY_SUFFIX) $(LIBRARY_PATH)
	@echo "Running ldconfig to update the library cache..."
	@if [ -f /usr/sbin/ldconfig ]; then sudo ldconfig; fi
	@echo "Installation complete..."

uninstall:
	@echo "Uninstalling header and library..."
	$(RM_CMD) $(PREFIX)/include/clog.h
	$(RM_CMD) $(LIBRARY_PATH)/lib$(LIBRARY_NAME)$(LIBRARY_SUFFIX)
	@echo "Uninstallation complete..."

clean:
	@echo "Cleaning up..."
	$(RM_CMD) $(LIBRARY_OBJ) lib$(LIBRARY_NAME)$(LIBRARY_SUFFIX)
	@echo "Clean complete..."

########################################
# This file was automatially generated
#

# The name of the project component
NAME := hurricane

# The name of the project
PROJECT_NAME := hurricane

# Enable/disable automatic generation of static function prototypes in project source code
$(NAME)_AUTO_PROTOTYPE := 0

# List of files to include in the project build. Paths relative to the project's directory
$(NAME)_SOURCES := hurricane_app.c \
                   hurricane_display.c \
                   hurricane_setup.c \
                   hurricane_dms.c \
                   hurricane_sensors.c \
                   hurricane_gpio.c \
                   drivers/si7013.c \
                   drivers/pca9535.c \
                   drivers/si115xdrv.c \
                   drivers/si115x_sys_out.c \
                   drivers/si72xx.c \
                   drivers/bmp280.c \
                   drivers/bmp280_sys_out.c \
                   drivers/icm20648.c \
                   drivers/ili9163c.c \
                   algorithms/si115x_gesture_init.c \
                   algorithms/si115x_gesture_algorithm.c \
                   gui_config/GUIConf.c \
                   gui_config/GUI_X_Ex.c 

# List of referenced libraries
$(NAME)_REQUIRED_COMPONENTS := resources.gecko_os.webapp \
                               components.gecko_os.third_party.graphics.emwin

# Pre-processor symbols for this project component only (not referenced libraries)
$(NAME)_DEFINES :=

# Pre-processor symbols for the entire build (this project and all referenced libraries)
$(NAME)_GLOBAL_DEFINES :=

# Includes file paths for project component only (not referenced libraries)
$(NAME)_INCLUDES := . \
                    drivers \
                    gui_config

# Includes file paths for the entire build (this project and all referenced libraries)
$(NAME)_GLOBAL_INCLUDES :=

# C compiler flags for project component only (not referenced libraries)
$(NAME)_CFLAGS :=

# C compiler flags for the entire build (this project and all referenced libraries)
$(NAME)_GLOBAL_CFLAGS :=

# Assembler flags for project component only (not referenced libraries)
$(NAME)_ASMFLAGS :=

# Assembler flags for the entire build (this project and all referenced libraries)
$(NAME)_GLOBAL_ASMFLAGS :=

# Linker flags for the entire build (this project and all referenced libraries)
$(NAME)_GLOBAL_LDFLAGS :=

# Path to resource manifest .json file (path is relative to project directory)
$(NAME)_RESOURCE_MANIFEST_PATH := manifest.cfg

# Paths to app settings .ini files  (paths are relative to project directory)
$(NAME)_SETTINGS_INCLUDES := resources/hurricane_settings.ini




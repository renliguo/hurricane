########################################
# This file was automatially generated
#

# The name of the project component
NAME := hurricane

# The name of the project
PROJECT_NAME := hurricane

# Enable/disable automatic generation of static function prototypes in project source code
$(NAME)_AUTO_PROTOTYPE := 0

# Require the THERMOMETER sensor types
$(NAME)_REQUIRED_FEATURES += SENSOR_TYPE_THERMOMETER   DRIVER_THERMOMETER_SI7013 \
                             SENSOR_TYPE_ACCELEROMETER DRIVER_ACCELEROMETER_ICM20648

# List of files to include in the project build. Paths relative to the project's directory
$(NAME)_SOURCES := hurricane_app.c \
                   hurricane_display.c \
                   hurricane_setup.c \
                   hurricane_dms.c \
                   hurricane_sensors.c \
                   gui_config/GUIConf.c \
                   gui_config/GUI_X_Ex.c 

# List of referenced libraries
$(NAME)_REQUIRED_COMPONENTS := resources.gecko_os.webapp \
                               components.gecko_os.sensor \
                               components.gecko_os.graphics.emwin \
                               components.gecko_os.secure_element.microchip.cryptoauthlib \
                               components.gecko_os.drivers.hygrometers.si7013 \
                               components.gecko_os.drivers.barometers.bmp280 \
                               components.gecko_os.drivers.accelerometers.icm20648 \
                               components.gecko_os.drivers.magnetometers.si72xx \
                               components.gecko_os.drivers.proximity.si115x \
                               components.gecko_os.drivers.display.ili9163c \
                               hardware.boards.gecko_os.hurricane


# Pre-processor symbols for this project component only (not referenced libraries)
$(NAME)_DEFINES :=

# Pre-processor symbols for the entire build (this project and all referenced libraries)
$(NAME)_GLOBAL_DEFINES :=

# Includes file paths for project component only (not referenced libraries)
$(NAME)_INCLUDES := . \
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
$(NAME)_RESOURCE_MANIFEST_PATH := resources/manifest.cfg

# Paths to app settings .ini files  (paths are relative to project directory)
$(NAME)_SETTINGS_INCLUDES := resources/hurricane_settings.ini




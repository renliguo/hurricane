NAME := drivers_accelerometer_icm20648

$(NAME)_SOURCES := accelerometer_sensor_api.c \
                   icm20648.c
                   
$(NAME)_GLOBAL_INCLUDES := .

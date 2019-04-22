NAME := drivers_proximity_si115x



$(NAME)_SOURCES := si115xdrv.c \
                   si115x_sys_out.c \
                   si115x_gesture_algorithm.c \
                   si115x_gesture_init.c

$(NAME)_GLOBAL_INCLUDES := .

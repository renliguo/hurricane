NAME := board_hurricane


$(NAME)_GLOBAL_INCLUDES := .

$(NAME)_GLOBAL_DEFINES := GOS_BOARD=HURRICANE


$(NAME)_REQUIRED_COMPONENTS := $(COMPONENTS_ROOT).drivers.ioexpanders.pca9535

$(NAME)_SOURCES := gos_board_hardware_init.c \
                   hurricane_gpio.c
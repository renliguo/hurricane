NAME := lib_sensor


$(NAME)_GLOBAL_INCLUDES := .

$(NAME)_SOURCES := sensor.c




include $(CURRENT_COMPONENT_DIRECTORY)types/supported_hygrometer_drivers.mk
include $(CURRENT_COMPONENT_DIRECTORY)types/supported_thermometer_drivers.mk
include $(CURRENT_COMPONENT_DIRECTORY)types/supported_accelerometer_drivers.mk
include $(CURRENT_COMPONENT_DIRECTORY)types/supported_gyroscope_drivers.mk
include $(CURRENT_COMPONENT_DIRECTORY)types/supported_magnetometer_drivers.mk


$(NAME)_SUPPORTED_FEATURES := $(addprefix SENSOR_TYPE_, $(SUPPORTED_SENSOR_TYPES))
$(NAME)_SUPPORTED_FEATURES := $(addprefix DRIVER_, $(SUPPORTED_DRIVERS))
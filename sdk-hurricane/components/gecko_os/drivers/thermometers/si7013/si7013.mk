NAME := drivers_thermometer_si7013


$(NAME)_GLOBAL_INCLUDES := .

# The SI7013 features both a thermoeter and hygrometer
# Therefore this sensor lib just uses the hyogrometer implementation
$(NAME)_REQUIRED_FEATURES := SENSOR_TYPE_HYGROMETER DRIVER_HYGROMETER_SI7013


$(NAME)_REQUIRED_COMPONENTS  := $(COMPONENTS_ROOT).drivers.hygrometers.si7013


$(NAME)_SUPPORTED_FEATURES := SENSOR_TYPE_THERMOMETER

$(NAME)_SENSOR_TYPE_THERMOMETER_SOURCES := thermometer_sensor_api.c 

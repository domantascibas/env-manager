MODULE_PATH = ../Core/modules
MODULE_DIRS = ds18b20

C_SOURCES += $(wildcard $(patsubst %, $(MODULE_PATH)/%/*.c, $(MODULES_DIR)))
C_INCLUDES += -I$(MODULE_PATH)/ds18b20

MODULE_PATH = ../Core/modules
MODULE_DIRS = $(patsubst $(MODULE_PATH)/%, %, $(sort \
	$(dir $(wildcard $(MODULE_PATH)/*/)) \
	))

C_SOURCES += $(wildcard $(patsubst %, $(MODULE_PATH)/%*.c, $(MODULE_DIRS)))
C_INCLUDES += $(patsubst %, -I$(MODULE_PATH)/%, $(MODULE_DIRS))

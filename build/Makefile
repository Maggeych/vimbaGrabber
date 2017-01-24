VIMBASDK_DIR		= ../Vimba_2_0

PROJECT_NAME            = vimbaGrabber

PROJECT_DIR             = ..
EXAMPLES_DIR            = $(VIMBASDK_DIR)/VimbaCPP/Examples
MAKE_INCLUDE_DIR        = $(EXAMPLES_DIR)/Build/Make

include $(MAKE_INCLUDE_DIR)/Common.mk

CONFIG_DIR          = $(ARCH)_$(WORDSIZE)bit
BIN_FILE            = $(PROJECT_NAME)
BIN_DIR             = binary/$(CONFIG_DIR)
OBJ_DIR             = object/$(CONFIG_DIR)
BIN_PATH            = $(BIN_DIR)/$(BIN_FILE)

all: $(BIN_PATH)

include $(MAKE_INCLUDE_DIR)/VimbaCPP.mk

include $(MAKE_INCLUDE_DIR)/VimbaImageTransform.mk

SOURCE_DIR          = $(PROJECT_DIR)
INCLUDE_DIRS        = -I$(SOURCE_DIR) \
                      -I$(EXAMPLES_DIR) \

LIBS                = $(VIMBACPP_LIBS) \
                      $(VIMBAIMAGETRANSFORM_LIBS) \
					  -lrt -lavdevice -lavfilter -lavformat -lavcodec -lswresample -lswscale -lavutil -pthread

DEFINES             =

CFLAGS              = $(COMMON_CFLAGS) \
                      $(VIMBACPP_CFLAGS) \
                      $(VIMBAIMAGETRANSFORM_CFLAGS)

OBJ_FILES           = $(OBJ_DIR)/FFmpegOutput.o $(OBJ_DIR)/CameraGrabber.o \
                      $(OBJ_DIR)/main.o

DEPENDENCIES        = VimbaCPP \
                      VimbaImageTransform

$(OBJ_DIR)/%.o: $(SOURCE_DIR)/%.cpp $(OBJ_DIR)
	$(CXX) -c $(INCLUDE_DIRS) $(DEFINES) $(CFLAGS) -o $@ $<

$(BIN_PATH): $(DEPENDENCIES) $(OBJ_FILES) $(BIN_DIR)
	$(CXX) $(ARCH_CFLAGS) -o $(BIN_PATH) $(OBJ_FILES) $(LIBS) -Wl,-rpath,'$$ORIGIN'

clean:
	$(RM) binary -r -f
	$(RM) object -r -f

$(OBJ_DIR):
	$(MKDIR) -p $(OBJ_DIR)

$(BIN_DIR):
	$(MKDIR) -p $(BIN_DIR)

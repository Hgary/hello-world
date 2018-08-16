######################################################################
# 		 MediaServer Makefile     			                  #
######################################################################
CC         := g++
ROOT_DIR   := $(shell pwd)
SRC_DIR    := ./src
LIB_DIR    := ../lib
INC_DIR    := ../include 
INC_DIR    += ../common/CoreBase 
INC_DIR    += ../common/CommonLib/src
INC_DIR    += ../common/common 
INC_DIR    += ../common/include/zookeeper/include/zookeeper/
INC_DIR    += ../common/include/

INSTALL_PATH := ../bin/TestRoute/

SRC_FILES  := $(wildcard $(SRC_DIR)/*.cpp)                                                                                                                               
OBJ_FILES  := $(SRC_FILES:.cpp=.o)
LD_LIBS    :=  corebase net rt netlog common2 curl xredis hiredis
#net rt netlog common2 curl
AR_LIBS     := zookeeper_mt
STATIC_LIBS := -Wl,-Bstatic $(addprefix -l,$(AR_LIBS)) -Wl,-Bdynamic
######################################################################
#		 LIB				                       #
######################################################################
#LIB_NAME   := openapi
#LIB_SUFFIX := .a
#LIB_TARGET := lib$(LIB_NAME)$(LIB_SUFFIX)

APP_NAME	:= TestRoute
APP_SUFFIX	:= 
APP_TARGET	:= $(APP_NAME)$(APP_SUFFIX)
APP_VERSION_H := ./src/version.h

.PHONY: all clean

LDFLAGS    := $(addprefix -L,$(LIB_DIR)) $(addprefix -l,$(LD_LIBS)) $(STATIC_LIBS)
CFLAGS     := -O2  -Werror -DTRACE_LOG -D__STDC_FORMAT_MACROS -Wno-deprecated  -I$(SRC_DIR) $(addprefix -I,$(INC_DIR))

#ifeq ($(DEBUG),yes)
CFLAGS     += -ggdb2 -D__DEBUG 
#endif

all: $(APP_TARGET);

$(APP_TARGET): $(OBJ_FILES)
	$(CC) -o $(APP_TARGET) $(CFLAGS) $(OBJ_FILES) $(LDFLAGS)
	@rm $(APP_VERSION_H)
	@echo *********Build $@ $(APP_TARGET:.a=.so) Successful*********
	@echo

%.o: %.cpp
	@echo "#define _GIT_VERSION_ \"$(shell git log -1 --format=%h)\" "   > $(APP_VERSION_H)
	@echo "#define _GIT_BRANCH_ \"$(shell git rev-parse --abbrev-ref HEAD)\" "   >> $(APP_VERSION_H)
	@echo "#define _RELEASE_VERSION_ \"$(SERVER_VERSION)\" "   >> $(APP_VERSION_H)
	$(CC) $(CFLAGS) -c $< -o $@


install:
	@echo install...
	#cp $(APP_TARGET) ../update -r
	cp $(APP_TARGET) $(INSTALL_PATH)$(APP_TARGET) -r

clean:
	@echo clean...
	@rm -rf $(APP_TARGET) $(LIB_TARGET) $(LIB_TARGET:.a=.so) $(OBJ_FILES) $(LIB_TARGET:.a=.bin) *.log

	

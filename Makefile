CXX=g++
TARGET=postjail
BUILD=_build/
SRC=src/

CFLAGS = -O2 -Wall -Wextra -std=c++11
CLIBS = -lcurl

OBJS= \
	$(BUILD)constants.o \
	$(BUILD)cfgFiles.o \
	$(BUILD)postSender.o \
	$(BUILD)main.o
	
all: $(TARGET)

$(TARGET): $(BUILD) $(OBJS)
	$(CXX) $(CFLAGS) -o $@ $(CLIBS) $(OBJS)

$(BUILD)%.o: $(SRC)%.cpp
	$(CXX) $(CFLAGS) -c $< -o $@
	
$(BUILD):
	mkdir -p $@
	
clean:
	rm -rf $(BUILD)

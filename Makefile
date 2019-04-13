CC=g++
LINKER=g++
INC_DIR=src
INCLUDES=-I. -I$(INC_DIR)
CFLAGS=-I. -c -std=c++17 -g -Wall $(INCLUDES)
LINKARGS=-lm -g -pthread
TARGET=out
BUILD_DIR=build
OUTPUT_DIR=output
LIBS=-lm -llua
SOURCE_DIR=src
LIB_DIR=$(SOURCE_DIR)/lib
SRCEXT=cpp
OBJEXT=o
DEPEXT=d

SOURCES=$(shell find $(SOURCE_DIR) -type f -name *.$(SRCEXT))
OBJECTS=$(patsubst $(SOURCE_DIR)/%,$(BUILD_DIR)/%,$(SOURCES:.$(SRCEXT)=.$(OBJEXT)))


all : $(TARGET)

$(TARGET) : $(OBJECTS)
	mkdir -p $(OUTPUT_DIR)
	$(LINKER) $(LINKARGS) -g $^ -o $(OUTPUT_DIR)/$@ $(LIBS) 

$(BUILD_DIR)/%.$(OBJEXT) : $(SOURCE_DIR)/%.$(SRCEXT)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<
	$(CC) $(CFLAGS) $(INC_DIR) -MM $(SOURCE_DIR)/$*.$(SRCEXT) > $(BUILD_DIR)/$*.$(DEPEXT)
	cp -f $(BUILD_DIR)/$*.$(DEPEXT) $(BUILD_DIR)/$*.$(DEPEXT).tmp
	sed -e 's|.*:|$(BUILD_DIR)/$*.$(OBJEXT):|' < $(BUILD_DIR)/$*.$(DEPEXT).tmp > $(BUILD_DIR)/$*.$(DEPEXT)
	sed -e 's/.*://' -e 's/\\$$//' < $(BUILD_DIR)/$*.$(DEPEXT).tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(BUILD_DIR)/$*.$(DEPEXT)
	rm -f $(BUILD_DIR)/$*.$(DEPEXT).tmp
	
clean :
	rm -rf $(BUILD_DIR)
	rm -rf $(OUTPUT_DIR)/$(TARGET)
	rm -rf $(OUTPUT_DIR)/*.log

doc :
	doxygen Doxyfile

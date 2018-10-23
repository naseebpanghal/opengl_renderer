
include makefile.head

all: $(DIR) $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(CXX) -fno-rtti $^ -o $@ $(LDFLAGS) $(LIBS)
	rm -f $(OBJ_FILES) $(DWO_FILES)

%.o: %.cpp
	$(CXX) $(CPPFLAGS) -c $< -o $@
%.o: %.cc
	$(CXX) $(CPPFLAGS) -c $< -o $@
%.o: %.cxx
	$(CXX) $(CPPFLAGS) -c $< -o $@
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(DIR):
	mkdir -p $(BIN_DIR)


clean:
	rm -f $(BIN_DIR)/$(BIN_NAME)
	rm -f $(OBJ_FILES) $(DWO_FILES)

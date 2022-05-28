include config.mk

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^

-include $(DEP)

$(OBJ): config.mk

.PHONY: clean
clean:
	$(RM) -rf $(OBJ) $(DEP) $(TARGET)

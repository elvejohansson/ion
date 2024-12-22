TARGET := ion

SRC_DIR := src
OUT_DIR := build

SRCS := $(shell find $(SRC_DIR) -name '*.c')
OBJS := $(SRCS:%=$(OUT_DIR)/%.o)

.PHONY: run
run: $(OUT_DIR)/$(TARGET)
	./$(OUT_DIR)/$(TARGET)

$(OUT_DIR)/$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@

$(OUT_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(OUT_DIR)

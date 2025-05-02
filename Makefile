CC := cc
CFLAGS := -Wall -Wextra -std=c11 -g -O0

BUILD_DIR := build

SRCS := partyline.c example.c

TARGET := $(BUILD_DIR)/partyline_example

partyline_example:
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -rf $(BUILD_DIR)

JSON = test/test.json
CFLAGS = -Wall -Wextra -g -O0

TST_PATH = ./test
SRC_PATH = ./src
INC_PATH = ./inc
BUILD = ./build

tests: $(BUILD)/testjson $(BUILD)/testsv

$(BUILD)/testjson: $(TST_PATH)/testjson.c $(SRC_PATH)/** $(INC_PATH)/**
	@mkdir -p $(@D)
	gcc $(CFLAGS) -I$(INC_PATH) $(SRC_PATH)/*.c $< -o $@

$(BUILD)/testsv: $(TST_PATH)/testsv.c $(SRC_PATH)/sv.c $(INC_PATH)/sv.h
	@mkdir -p $(@D)
	gcc $(CFLAGS) -I$(INC_PATH) $(SRC_PATH)/sv.c $< -o $@

clean:
	rm -rf $(BUILD)

run: $(TARGET)
	$(TARGET) $(JSON)
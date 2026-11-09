EXE = test
TEST = test/test1.c
JSON = test/test.json

# stupid Makefile just to make life easier
build/$(EXE): ./src/** ./inc/**
	@mkdir -p $(@D)
	gcc -Wall -Wextra -g -Iinc ./src/*.c $(TEST) -o ./build/$(EXE)

clean:
	rm -rf build

run: build/$(EXE)
	./build/$(EXE) $(JSON)
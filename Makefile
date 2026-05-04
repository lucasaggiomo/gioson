EXE = test
TEST = test/test1.c
JSON = test/test.json

# stupid Makefile just to make life easier
build/$(EXE): ./src/**
	@mkdir -p $(@D)
	gcc -Wall -Wextra -g -Isrc ./src/*.c $(TEST) -o ./build/$(EXE)

clean:
	rm -rf build

run: build/$(EXE)
	./build/$(EXE) $(JSON)
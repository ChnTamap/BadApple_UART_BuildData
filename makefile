CMD_CLANG := clang++ -g -Wall -static-libgcc -fcolor-diagnostics --target=x86_64-w64-mingw -std=c++11
# CMD_CLANG := g++ -g
obj/EnvTest.exe:output/main.o output/test_TransPhoto.o output/test_match.o output/bmp.o output/graphics.o output/test_Conv.o output/test_HSL.o output/test_Check.o
	$(CMD_CLANG) -o $@ $^
output/main.o:src/main.cpp
	$(CMD_CLANG) -o $@ -c $^
output/test_TransPhoto.o:src/test_TransPhoto.cpp
	$(CMD_CLANG) -o $@ -c $^
output/test_match.o:src/test_match.cpp
	$(CMD_CLANG) -o $@ -c $^
output/bmp.o:src/bmp.cpp
	$(CMD_CLANG) -o $@ -c $^
output/graphics.o:src/graphics.cpp
	$(CMD_CLANG) -o $@ -c $^
output/test_Conv.o:src/test_Conv.cpp
	$(CMD_CLANG) -o $@ -c $^
output/test_HSL.o:src/test_HSL.cpp
	$(CMD_CLANG) -o $@ -c $^
output/test_Check.o:src/test_Check.cpp
	$(CMD_CLANG) -o $@ -c $^
clean:
	del -rf output/*.o
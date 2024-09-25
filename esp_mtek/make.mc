
/////////////////////////////////////////////////////////////////////////////////////////

PATH    <- ["$PATH","/home/marius/.arduino15/packages/esp32/tools/xtensa-esp32-elf-gcc/1.22.0-97-gc752ad5-5.2.0/bin/"];
ARCH    <- "ARM"
PREFIX  <- "xtensa-esp32-elf-"
CC      <-  "gcc";
CPP     <-  "g++";
AR      <-  "ar";

FILES   <-  ["*.cpp",   "libraries/*/*.cpp"]
print(FILES);
//INCLUDES <- ["./",  "libraries/",   "libraries/*/*.h"]


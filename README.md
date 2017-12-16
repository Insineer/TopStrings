# TopStrings

Solution for multi-threaded search of the most frequent lines in a large file

# Compile

Run CMake

# Usage

./TopStrings  
Return top of strings from "input.txt". Output: "output.txt". Use 1 thread and 200 bytes of RAM.

./TopStrings -j4 -m500 -n20 input.txt  
* -j4: 4 threads  
* -m500: 500 bytes RAM  
* -n20: Top will contain 20 lines  

./TopStrings --generate-test=500 output.txt  
Generate test file which size is 500 bytes  
This option use file "template.txt" to get words.

You can use numbers up to 2 * 10 ^ 9. Also it is possible to expand this restrictions, but this will require a little bit more code.  
With large sizes, there shouldn't be problems, but testing will take more time.

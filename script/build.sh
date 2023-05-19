rm -rf build
rm compile_commands.json
cmake -B build
ln build/compile_commands.json compile_commands.json 
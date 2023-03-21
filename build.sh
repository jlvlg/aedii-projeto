git submodule update --init
cmake -S . -B build
cmake --build build --config Release --target install
rm -r build
ln -s bin/bin/main main
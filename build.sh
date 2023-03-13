cmake -S . -B build
cmake --build build --config Release --target install
rm -r build
ln -s bin/bin/Main main
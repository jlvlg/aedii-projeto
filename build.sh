git submodule update --init
cmake -DBUILD_SHARED_LIBS=OFF -S . -B build
cmake --build build --config Release --target install
rm -r build
ln -s bin/bin/main main
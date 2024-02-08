sudo rm build/* -rf
cmake -DCMAKE_BUILD_TYPE=Debug -B build
cmake --build build -j $(nproc)
sudo cp build/libhycov.so  /usr/lib


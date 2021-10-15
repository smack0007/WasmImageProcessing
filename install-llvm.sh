wget https://apt.llvm.org/llvm.sh
chmod +x llvm.sh
sudo ./llvm.sh 13

sudo ln -s /usr/bin/clang-13 /usr/bin/clang
sudo ln -s /usr/bin/wasm-ld-13 /usr/bin/wasm-ld
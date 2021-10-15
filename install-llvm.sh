wget https://apt.llvm.org/llvm.sh
chmod +x llvm.sh
sudo ./llvm.sh 13

if [ -f "/usr/bin/clang" ]; then
  sudo rm /usr/bin/clang
fi
sudo ln -s /usr/bin/clang-13 /usr/bin/clang

if [ -f "/usr/bin/wasm-ld" ]; then
  sudo rm /usr/bin/wasm-ld
fi
sudo ln -s /usr/bin/wasm-ld-13 /usr/bin/wasm-ld
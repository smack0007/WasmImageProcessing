@ECHO OFF
docker run --rm -it -v "%cd%:/app" -w /app emscripten/emsdk /bin/bash
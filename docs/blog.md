# Practical WebAssembly

WebAssembly (WASM) is an interesting technology that
allows us to write code for the Web in a language other
than JavaScript. In this article we'll convert images to
grayscale via WASM. The code to convert images will be
written in C but we'll also need to write some HTML and
JavaScript to marry the C code with browser functionality.

The sample repository can be found [here][repo]. Prerequisites
are [docker] and if you use [VS Code][vs-code] you can compile
the sample by starting the build task or using `Ctrl + Shift + B`.

You can also compile the sample by hand with [emscripten] and
[make].

## The C Code

The function to convert the image to grayscale takes the width and
height of the image plus an array of bytes which represent the
pixels of the image as arguments. Each pixel is 4 bytes long
and is assumed to be in the [RGBA32][rgba] format:

```c
#include "image.h"

void process(uint32 width, uint32 height, byte* bytes) {
  for (uint32 y = 0; y < height; y++) {
    for (uint32 x = 0; x < width; x++) {
      uint64 pixelOffset = (y * width * 4) + (x * 4);

      byte r = bytes[pixelOffset];
      byte g = bytes[pixelOffset + 1];
      byte b = bytes[pixelOffset + 2];

      byte gray = (byte)(((uint32)r + (uint32)g + (uint32)b) / 3);

      bytes[pixelOffset] = gray;
      bytes[pixelOffset + 1] = gray;
      bytes[pixelOffset + 2] = gray;
    }
  }
}
```

> See [image.c].
> Note that `uint32` is defined as a 32 bit unsigned
> integer, `uint64` as a 64 bit unsigned integer and `byte`
> as an unsigned 8 bit integer.

## Preparing to compile Web Assembly via Emscripten

We'll use [emscripten] to compile the C code to WASM.
[clang](https://clang.llvm.org/) could also be used but would
require more work as compiling with clang will not provide a C
Standard Library, so functions like `malloc` and `printf` would
have to implemented on your own.

We're [emscripten] a docker container in this article but the documentation for emscripten provides detailed
[installation instructions](https://emscripten.org/docs/getting_started/downloads.html#installation-instructions-using-the-emsdk-recommended).
Here’s how to start the docker container:

```
docker run --rm -it -v "$(pwd):/app" -w /app emscripten/emsdk /bin/bash
```

This command will work for PowerShell or for bash. Replace
`$(pwd)` with `%cd%` if you're running via `cmd` on Windows.

## Creating the WASM Interface

Before we compile our C code we'll need to create a simple
WASM Interface so that our JavaScript code later can talk to our
C code:

```c
#include <emscripten.h>
#include <stdlib.h>
#include "image.h"

EMSCRIPTEN_KEEPALIVE
byte* wasmAlloc(uint32 width, uint32 height) {
  return malloc(width * height * 4);
}

EMSCRIPTEN_KEEPALIVE
void wasmFree(byte* p) {
  free(p);
}

EMSCRIPTEN_KEEPALIVE
void wasmProcess(uint32 width, uint32 height, byte* buffer) {
  process(width, height, buffer);
}
```

> See [wasm.c].
> `EMSCRIPTEN_KEEPALIVE` is a macro that will prevent
> these functions from being eliminated as dead code.
> For more information read the [documentation](https://emscripten.org/docs/api_reference/emscripten.h.html#c.EMSCRIPTEN_KEEPALIVE).

Once our code is compiled to WASM and running inside
a browser you can think of it as a Virtual Machine (VM)
that we can interact with via JavaScript. `wasmAlloc` will
allocate memory inside our VM which we can then write to
from JavaScript. `wasmProcess` simply exposes our existing
`process` function from [image.c]. `wasmFree` will
be used to free the memory we allocated via `wasmAlloc`.

## Compiling to WASM

We're finally ready to compile our C code to WASM. There
is a [Makefile][makefile] provided in the example project
but I'll describe how to do it by hand here:

```bash
emcc -O3 --no-entry -s EXPORTED_RUNTIME_METHODS='["cwrap"]' -s NO_EXIT_RUNTIME=1 -s ALLOW_MEMORY_GROWTH=1 -o "/app/bin/image.js" "/app/src/image.c" "/app/src/wasm.c"
```

That's a long command but let's break down what each piece
actually means.

- `emcc` The [emscripten] compiler.
- `-O3` Enables code optimization. This can be left out
  until you know your code is working in WASM for
  debugging purposes.
- `--no-entry` There is no `main` function in our C code
  as we're compiling a library.
- `-s EXPORTED_RUNTIME_METHODS='["cwrap"]'` -s is for
  setting options in `emcc`. `EXPORTED_RUNTIME_METHODS` is
  used to define a list of built in [emscripten] functions
  to expose to JavaScript.
- `-s NO_EXIT_RUNTIME=1` Normally [emscripten] would shut
  down our VM once `main` exits. This is a library and has
  no `main` so don't shut down the VM.
- `-s ALLOW_MEMORY_GROWTH=1` Allow the memory pool of the
  WASM VM to grow.
- `-o "/app/bin/image.js"` Write the output to
  `/app/bin/image.js`. `/app` is the mapped path for the
  repository in docker. We'll put all output in the `/bin/`
  directory. Although we're specifying `image.js` this will
  actually produce 2 files: `image.js` and `image.wasm`. `image.js`
  is a bootstrapper for `image.wasm` and is the file we'll
  include in our HTML file in the next step.

## HTML and JavaScript

We're finally ready to load the WASM into the browser. You can
see the complete [index.html here][index.html] but we'll go through it step by step. First let's create the HTML we need:

```html
<!DOCTYPE html>
<html>
  <head>
    <title>WASM Image Processing</title>
  </head>

  <body>
    <h1>WASM Image Processing</h1>
    <h2 id="loading">Loading...</h2>
    <canvas id="canvas" style="display: none"></canvas>
    <p>Drag and drop a photo.</p>

    <!-- Load the WASM bootstrapper. -->
    <script src="image.js"></script>
    <script>
      // We'll put our JavaScript code here.
    </script>
  </body>
</html>
```

Initially we have a simple `Loading...` text that is visible
and the canvas we'll use to display images is hidden. All JavaScript
code that follows will be placed inside the last `script` tag.
Now let's fetch some DOM elements and set up a helper function:

```js
const loading = document.getElementById("loading");
const canvas = document.getElementById("canvas");
const context = canvas.getContext("2d");

// We can use this function to show or hide the canvas.
function toggleCanvas(show) {
  loading.style.display = !show ? "block" : "none";
  canvas.style.display = show ? "block" : "none";
}
```

Next we'll create a callback function for when the WASM VM is loaded
and ready to be used:

```js
Module.onRuntimeInitialized = () => {
  // All JavaScript code that follows will be placed inside here.
};
```

Once the WASM VM is loaded we have to create bindings between the
JavaScript world and the WASM world:

```js
const wasm = {
  alloc: Module.cwrap("wasmAlloc", "number", ["number", "number"]),
  free: Module.cwrap("wasmFree", "void", ["number"]),
  process: Module.cwrap("wasmProcess", "void", ["number", "number", "number"]),
};
```

[Module.cwrap][cwrap] is the function we exposed in the compile
command via `EXPORTED_RUNTIME_METHODS`. It helps us create bindings
from JavaScript to WASM and takes 3 arguments:

1. The function name.
2. The return type.
3. An array for the function argument types.

All types are the JavaScript types and pointers can be passed as
`number`(s).

Next we'll create a detached `Image` DOM element and use it to
load images into the browser. Every time an image is loaded we'll
convert it to grayscale via WASM:

```js
const image = new Image();

image.addEventListener("load", () => {
  // Draw the original image one time to the canvas.
  canvas.width = image.width;
  canvas.height = image.height;
  context.drawImage(image, 0, 0);

  // Get the image data (pixels RGBA) for the canvas.
  const imageData = context.getImageData(0, 0, image.width, image.height);

  // Allocate a buffer for the pixels in WASM.
  const wasmBufferPtr = wasm.alloc(image.width, image.height);

  // Copy the image pixels into the buffer.
  Module.HEAPU8.set(imageData.data, wasmBufferPtr);

  // Process the image in WASM.
  wasm.process(imageData.width, imageData.height, wasmBufferPtr);

  // Draw the image back to the canvas.
  const newImageData = new ImageData(
    new Uint8ClampedArray(Module.HEAPU8.buffer, wasmBufferPtr, image.width * image.height * 4),
    image.width,
    image.height
  );
  context.putImageData(newImageData, 0, 0);

  // Free the buffer we allocated for the image pixels.
  wasm.free(wasmBufferPtr);

  // Show the the canvas again.
  toggleCanvas(true);
});

// Handle errors related to loading an image.
image.addEventListener("error", () => {
  console.error("Failed to load image.");

  // Show the canvas again with the old image.
  toggleCanvas(true);
});

// Load the default image.
image.src = "default.png";
```

At this point we have a working application as long as you copy
`index.html` and a `default.png` file into the `bin` directory. If
you've cloned the source repository the `Makefile` will take care of
all that.

You'll need to start a local http server in order for the sample
to work locally, otherwise the browser will fail to load the
`image.wasm` file. If you have [node] installed on your system
I recommend using [http-server]:

```bash
npx http-server ./bin
```

Load `http://localhost:8080/` in your browser and you should see
your image presented in all it's grayscale glory.

## Drag and Drop

This part really doesn't have anything to do with WASM anymore
but I feel it makes the application a more complete example. We'll
add drag and drop support so that images can be dropped onto the
browser and they'll be presented in grayscale.

```js
// To enable drag and drop we need to call preventDefault
// on dragover.
document.addEventListener("dragover", (event) => {
  event.preventDefault();
});

// When a user drops a file on the browser.
document.addEventListener("drop", (event) => {
  event.preventDefault();

  // There are multiple ways the file can come in. Try and
  // find a file.
  let file = undefined;
  if (event.dataTransfer.items && event.dataTransfer.items[0] && event.dataTransfer.items[0].kind === "file") {
    file = event.dataTransfer.items[0].getAsFile();
  } else if (event.dataTransfer.files[0]) {
    file = event.dataTransfer.files[0];
  }

  // If we found the file, hide the canvas and trigger
  // the image loading. Otherwise log an error.
  if (file) {
    toggleCanvas(false);
    image.src = URL.createObjectURL(file);
  } else {
    console.error("Failed to find file in drop event.");
  }
});
```

Reload the browser and you should be able to drag and drop images
onto the page and have them loaded in. I've tested with `png` and
`jpeg` images but any image that can be loaded by the browser
should work. If you drop a file that can't be loaded as an image you
should see an error written to the console.

[rgba]: https://en.wikipedia.org/wiki/RGBA_color_model
[docker]: https://www.docker.com/
[vs-code]: https://code.visualstudio.com/
[emscripten]: https://emscripten.org/
[make]: https://www.gnu.org/software/make/
[repo]: https://github.com/conciso/WasmImageProcessing
[example]: https://smack0007.github.io/WasmImageProcessing/
[node]: https://nodejs.org/en/
[http-server]: https://www.npmjs.com/package/http-server
[image.c]: https://github.com/conciso/WasmImageProcessing/blob/main/src/image.c
[index.html]: https://github.com/conciso/WasmImageProcessing/blob/main/src/index.html
[wasm.c]: https://github.com/conciso/WasmImageProcessing/blob/main/src/wasm.c
[makefile]: https://github.com/conciso/WasmImageProcessing/blob/main/Makefile
[cwrap]: https://emscripten.org/docs/porting/connecting_cpp_and_javascript/Interacting-with-code.html#calling-compiled-c-functions-from-javascript-using-ccall-cwrap

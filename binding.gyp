{
  "targets": [
    {
      "target_name": "liboqs_node",
      "cflags": [
        "-fexceptions",
        "-std=c++2a"
      ],
      "cflags_cc": [
        "-fexceptions",
        "-std=c++2a"
      ],
      "sources": [
        "./src/addon.cpp",
        "./src/KEMs.cpp",
        "./src/KeyEncapsulation.cpp",
        "./src/Random.cpp",
        "./src/Signature.cpp",
        "./src/Sigs.cpp"
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")",
        "./deps/liboqs/build/include",
        "./deps/liboqs-cpp/include"
      ],
      "libraries": [
        "../deps/liboqs/build/lib/liboqs.a"
      ],
      "defines": [
        "NAPI_CPP_EXCEPTIONS"
      ]
    }
  ]
}

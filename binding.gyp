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
      "actions": [
        {
          "action_name": "ensure_deps",
          "inputs": [],
          "outputs": [
            "./deps/liboqs/src/oqs.h",
            "./deps/liboqs-cpp/include/oqs_cpp.h"
          ],
          "action": ["npm", "run", "ensure_submodules"],
          "message": "Ensuring submodules"
        },
        {
          "action_name": "ensure_liboqs",
          "inputs": [],
          "outputs": [
            "./deps/liboqs/build/include/oqs/oqs.h"
          ],
          "action": ["npm", "run", "liboqs:build_if_not_exists"],
          "message": "Ensuring liboqs is built"
        }
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

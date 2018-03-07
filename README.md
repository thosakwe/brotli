# brotli
[![Pub](https://img.shields.io/pub/v/brotli.svg)](https://pub.dartlang.org/packages/brotli)

Native bindings for the [Brotli](https://github.com/google/brotli)
compressor. Supports a `dart:convert` codec.

## Example
```dart
import 'dart:convert';
import 'package:brotli/brotli.dart';

main() {
  var str = 'Hello, world!'.codeUnits;
  var encoded = BROTLI.encode(str);
  print(encoded);

  var decoded = BROTLI.decode(encoded);
  print(new String.fromCharCodes(decoded));
}
```

## Building
1. Set `DART_SDK` in your environment to the path of your Dart SDK.
2. Ensure you have at least CMake 3.8.0 on your system.
3. Run `cmake --build .`.
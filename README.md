# brotli
[![Pub](https://img.shields.io/pub/v/brotli.svg)](https://pub.dartlang.org/packages/brotli)

Native bindings for the [Brotli](https://github.com/google/brotli)
compressor. Supports a `dart:convert` codec.

## Building
Pub currently has no support for building native extensions on-the-fly,
so you will need to build it yourself. This can be split into the
following steps:

1. Ensure CMake is present on your system.
2. Install `package:brotli`. This is as simple as adding it to `dependencies` in
a `pubspec.yaml`.
3. In your command line, `cd` to where `brotli` is saved locally on your system
On Linux/MacOS/Unix, this should be `~/.pub-cache/hosted/pub.dartlang.org/brotli`.
On Windows, this should be `%APPDATA%\Pub\Cache\hosted\pub.dartlang.org\brotli`.
4. `cd` to where your desired version of `brotli` is. ex. `1.1.0`.
5. Configure the project with `cmake .`
6. Build `libdart_brotli` by running `cmake --build .`

`package:brotli` should now be built, and ready for use in projects on
your system without having to rebuild again.

There are some prebuilt binaries in `lib`, but there is no guarantee that they
will work with your system.

## Example
```dart
import 'dart:convert';
import 'package:brotli/brotli.dart';

main() {
  var str = 'Hello, world!'.codeUnits;
  var encoded = brotli.encode(str);
  print(encoded);

  var decoded = brotli.decode(encoded);
  print(new String.fromCharCodes(decoded));
}
```

## Building
1. Set `DART_SDK` in your environment to the path of your Dart SDK.
2. Ensure you have at least CMake 3.8.0 on your system.
3. Run `cmake --build .`.
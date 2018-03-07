import 'dart:convert';
import 'package:brotli/brotli.dart';

main() {
  var str = 'Hello, world!'.codeUnits;
  var encoded = BROTLI.encode(str);
  print(encoded);

  var decoded = BROTLI.decode(encoded);
  print(new String.fromCharCodes(decoded));
}
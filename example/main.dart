import 'package:brotli/brotli.dart';

main() {
  var str = 'Hello, world!';
  print('Original: $str');

  var encoded = brotli.encode(str.codeUnits);
  print('Encoded: $encoded');

  var decoded =  new String.fromCharCodes(brotli.decode(encoded));
  print('Decoded: $decoded');
}
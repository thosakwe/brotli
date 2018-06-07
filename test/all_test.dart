import 'package:brotli/brotli.dart';
import 'package:test/test.dart';

void main() {
  var original = 'Hello, world!';
  var encoded = brotli.encode(original.codeUnits);
  var decoded = new String.fromCharCodes(brotli.decode(encoded));

  test('encode', () {
    print('Original: $original');
    print('Encoded: $encoded');
    print('Decoded: $decoded');
    expect(decoded, original);
  });
}

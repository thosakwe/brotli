import 'dart:convert';
import 'dart-ext:dart_brotli';

const BrotliCodec BROTLI = const BrotliCodec();

class BrotliCodec extends Codec<List<int>, List<int>> {
  final BrotliEncoder encoder = const BrotliEncoder();
  final BrotliDecoder decoder = const BrotliDecoder();
  const BrotliCodec();
}

class BrotliEncoder extends Converter<List<int>, List<int>> {
  const BrotliEncoder();

  @override
  List<int> convert(List<int> input) {
    return _encode(input);
  }
}

class BrotliDecoder extends Converter<List<int>, List<int>> {
  const BrotliDecoder();

  @override
  List<int> convert(List<int> input) {
    return _decode(input);
  }
}

List<int> _encode(List<int> input) native "brotli_encode";

List<int> _decode(List<int> input) native "brotli_decode";
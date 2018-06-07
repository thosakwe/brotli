import 'dart:convert';
import 'dart:typed_data';
import 'dart-ext:dart_brotli';

const BrotliCodec brotli = const BrotliCodec();

/// Use [brotli] instead.
@deprecated
const BrotliCodec BROTLI = brotli;

class BrotliCodec extends Codec<List<int>, List<int>> {
  final BrotliEncoder encoder = const BrotliEncoder();
  final BrotliDecoder decoder = const BrotliDecoder();

  const BrotliCodec();
}

class BrotliEncoder extends Converter<List<int>, List<int>> {
  const BrotliEncoder();

  @override
  List<int> convert(List<int> input) {
    return _encode(input is Uint8List ? input : new Uint8List.fromList(input));
  }
}

class BrotliDecoder extends Converter<List<int>, List<int>> {
  const BrotliDecoder();

  @override
  List<int> convert(List<int> input) {
    return _decode(input is Uint8List ? input : new Uint8List.fromList(input));
  }
}

List<int> _encode(Uint8List input) native "brotli_encode";

List<int> _decode(Uint8List input) native "brotli_decode";

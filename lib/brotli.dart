import 'dart:convert';
import 'dart:io';
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
    var result =
        _decode(input is Uint8List ? input : new Uint8List.fromList(input));
    if (result[0] != null) throw new BrotliException(result[2], result[0]);
    return (result[1] as BytesBuilder).takeBytes();
  }
}

List<int> _encode(Uint8List input) native "brotli_encode";

List _decode(Uint8List input) native "brotli_decode";

class BrotliException implements Exception {
  final int code;
  final String message;

  const BrotliException(this.code, this.message);

  @override
  String toString() => 'Brotli exception $code: $message';
}

import 'dart:io';
import 'package:angel_framework/angel_framework.dart';
import 'package:angel_http2/angel_http2.dart';
import 'package:angel_static/angel_static.dart';
import 'package:brotli/brotli.dart';
import 'package:file/local.dart';

const fs = const LocalFileSystem();

main() async {
  var app = new Angel()
    ..lazyParseBodies = true
    ..injectEncoders({
      //'gzip': GZIP.encoder,
      //'deflate': ZLIB.encoder,
      'br': BROTLI.encoder,
    });

  var vDir = new VirtualDirectory(
    app,
    fs,
    source: fs.directory(fs.currentDirectory),
    allowDirectoryListing: true,
  );

  app.get('a', 'b');

  app.use(vDir.handleRequest);

  var ctx = new SecurityContext()
    ..useCertificateChain('dev.pem')
    ..usePrivateKey('dev.key', password: 'dartdart');
  ctx.setAlpnProtocols(['h2'], true);

  var http = new AngelHttp(app);
  var http2 = new AngelHttp2(app, ctx)..onHttp1.listen(http.handleRequest);
  var server = await http2.startServer('127.0.0.1', 3000);
  print('Listening at https://${server.address.address}:${server.port}');
}

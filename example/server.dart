import 'package:angel_framework/angel_framework.dart';
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
      'br': brotli.encoder,
    });

  var vDir = new VirtualDirectory(
    app,
    fs,
    source: fs.directory(fs.currentDirectory),
    allowDirectoryListing: true,
  );

  app.get('a', 'b');

  app.use(vDir.handleRequest);

  var http = new AngelHttp(app);
  var server = await http.startServer('127.0.0.1', 3000);
  print('Listening at http://${server.address.address}:${server.port}');
}

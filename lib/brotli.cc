#include <cstring>
#include <dart_api.h>
#include <brotli/decode.h>
#include <brotli/encode.h>
#include <vector>

// Forward declaration of ResolveName function.
Dart_NativeFunction ResolveName(Dart_Handle name, int argc, bool *auto_setup_scope);

// The name of the initialization function is the extension name followed
// by _Init.
DART_EXPORT Dart_Handle dart_brotli_Init(Dart_Handle parent_library) {
    if (Dart_IsError(parent_library)) return parent_library;

    Dart_Handle result_code =
            Dart_SetNativeResolver(parent_library, ResolveName, nullptr);
    if (Dart_IsError(result_code)) return result_code;

    return Dart_Null();
}

Dart_Handle HandleError(Dart_Handle handle) {
    if (Dart_IsError(handle)) Dart_PropagateError(handle);
    return handle;
}

void brotli_encode(Dart_NativeArguments arguments) {
    // Get the input byte array
    Dart_Handle list_handle = Dart_GetNativeArgument(arguments, 0);
    intptr_t length;
    Dart_ListLength(list_handle, &length);
    uint8_t input_data[length];
    Dart_ListGetAsBytes(list_handle, 0, input_data, length);
    size_t size = BrotliEncoderMaxCompressedSize(static_cast<size_t>(length));
    uint8_t output_data[size];

    // Compress the buffer.
    BrotliEncoderCompress(
            BROTLI_DEFAULT_QUALITY,
            BROTLI_DEFAULT_WINDOW,
            BROTLI_DEFAULT_MODE,
            static_cast<size_t>(length),
            input_data,
            &size,
            output_data
    );

    // Create a new byte array
    Dart_Handle output_handle = Dart_NewList(size);

    // Write the data in
    Dart_ListSetAsBytes(output_handle, 0, output_data, size);

    // And return
    Dart_SetReturnValue(arguments, output_handle);
}

void brotli_decode(Dart_NativeArguments arguments) {
    // Get the input byte array
    Dart_Handle list_handle = Dart_GetNativeArgument(arguments, 0);
    intptr_t length;
    Dart_ListLength(list_handle, &length);
    uint8_t input_data[length];
    Dart_ListGetAsBytes(list_handle, 0, input_data, length);
    auto size = static_cast<size_t>(length);
    auto *output = new std::vector<uint8_t>();

    BrotliDecoderState *state = BrotliDecoderCreateInstance(nullptr, nullptr, nullptr);

    auto available_in = static_cast<size_t>(length);
    const uint8_t* next_in = input_data;
    size_t available_out = 0;
    uint8_t* next_out = nullptr;

    BrotliDecoderResult result = BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT;
    while (result == BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT) {
        result = BrotliDecoderDecompressStream(state,
                                               &available_in, &next_in,
                                               &available_out, &next_out, nullptr);
        size_t buffer_length = 0; // Request all available output.
        const uint8_t* buffer = BrotliDecoderTakeOutput(state, &buffer_length);
        if (buffer_length) {
            (*output).insert((*output).end(), buffer, buffer + buffer_length);
        }
    }


    BrotliDecoderDestroyInstance(state);

    // Create a new byte array
    Dart_Handle output_handle = Dart_NewList(output->size());

    // Write the data in
    Dart_ListSetAsBytes(output_handle, 0, output->data(), output->size());

    // And return
    Dart_SetReturnValue(arguments, output_handle);
}

Dart_NativeFunction ResolveName(Dart_Handle name, int argc, bool *auto_setup_scope) {
    // If we fail, we return NULL, and Dart throws an exception.
    if (!Dart_IsString(name)) return nullptr;
    Dart_NativeFunction result = nullptr;
    const char *cname;
    HandleError(Dart_StringToCString(name, &cname));

    if (strcmp("brotli_encode", cname) == 0) result = brotli_encode;
    if (strcmp("brotli_decode", cname) == 0) result = brotli_decode;
    return result;
}
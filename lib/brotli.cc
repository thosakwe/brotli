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
    void *input_data;
    intptr_t length;
    Dart_TypedData_Type type;

    // Get the input byte array.
    Dart_Handle list_handle = Dart_GetNativeArgument(arguments, 0);
    HandleError(Dart_TypedDataAcquireData(list_handle, &type, &input_data, &length));

    // Get the max output size.
    size_t size = BrotliEncoderMaxCompressedSize((size_t) length);
    uint8_t *output_data = Dart_ScopeAllocate(size);

    // Compress the buffer.
    BrotliEncoderCompress(
            BROTLI_DEFAULT_QUALITY,
            BROTLI_DEFAULT_WINDOW,
            BROTLI_DEFAULT_MODE,
            (size_t) length,
            (const uint8_t *) input_data,
            &size,
            output_data
    );

    // Release the input data.
    HandleError(Dart_TypedDataReleaseData(list_handle));

    // And return.
    Dart_Handle output_handle = Dart_NewExternalTypedData(Dart_TypedData_kUint8, output_data, size);
    Dart_SetReturnValue(arguments, output_handle);
}

void brotli_decode(Dart_NativeArguments arguments) {
    void *input_data;
    intptr_t length;
    Dart_TypedData_Type type;

    // Get the input byte array.
    Dart_Handle list_handle = Dart_GetNativeArgument(arguments, 0);
    HandleError(Dart_TypedDataAcquireData(list_handle, &type, &input_data, &length));


    auto size = (size_t) length;
    auto *output = new std::vector<uint8_t>(size);

    BrotliDecoderState *state = BrotliDecoderCreateInstance(nullptr, nullptr, nullptr);

    auto available_in = (size_t) length;
    auto *next_in = (const uint8_t *) input_data;
    size_t available_out = 0;
    uint8_t *next_out = nullptr;

    BrotliDecoderResult result = BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT;
    while (result == BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT) {
        result = BrotliDecoderDecompressStream(state,
                                               &available_in, &next_in,
                                               &available_out, &next_out, nullptr);
        size_t buffer_length = 0; // Request all available output.
        const uint8_t *buffer = BrotliDecoderTakeOutput(state, &buffer_length);
        if (buffer_length) {
            (*output).insert((*output).end(), buffer, buffer + buffer_length);
        }
    }

    BrotliDecoderDestroyInstance(state);

    // Create a new byte array.
    Dart_Handle output_handle = Dart_NewExternalTypedData(Dart_TypedData_kUint8, output->data(), output->size());

    // And return.
    Dart_SetReturnValue(arguments, output_handle);

    // Delete the vector we created.
    delete output;
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
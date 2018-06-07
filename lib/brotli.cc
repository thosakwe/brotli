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

void *brotli_scope_allocate(void *opaque, size_t size) {
    return Dart_ScopeAllocate(size);
}

void brotli_scope_deallocate(void *opaque, void *address) {
    // Do nothing
}

void brotli_decode(Dart_NativeArguments arguments) {
    void *input_data;
    intptr_t length;
    Dart_TypedData_Type type;

    // Get the input byte array.
    Dart_Handle list_handle = Dart_GetNativeArgument(arguments, 0);
    HandleError(Dart_TypedDataAcquireData(list_handle, &type, &input_data, &length));
    Dart_ThrowException(list_handle);

    BrotliDecoderState *state = BrotliDecoderCreateInstance(brotli_scope_allocate, brotli_scope_deallocate, nullptr);
    auto available_in = (size_t) length;
    auto *next_in = (const uint8_t *) input_data;
    size_t available_out = 0;
    uint8_t *next_out = nullptr;

    // Create a BytesBuilder.
    Dart_Handle add = Dart_NewStringFromCString("add");
    Dart_Handle dart_io = Dart_LookupLibrary(Dart_NewStringFromCString("dart:io"));
    Dart_Handle bytesBuilder_type = Dart_GetType(dart_io, Dart_NewStringFromCString("BytesBuilder"), 0, nullptr);
    Dart_Handle bytes_builder = Dart_New(bytesBuilder_type, Dart_EmptyString(), 0, nullptr);

    BrotliDecoderResult result;

    do {
        result = BrotliDecoderDecompressStream(state,
                                               &available_in, &next_in,
                                               &available_out, &next_out, nullptr);
        size_t buffer_length = 0; // Request all available output.
        const uint8_t *buffer = BrotliDecoderTakeOutput(state, &buffer_length);

        // Create a new Uint8List, and write to the builder.
        Dart_Handle uint8List = Dart_NewExternalTypedData(Dart_TypedData_kUint8, (void *) buffer, buffer_length);
        HandleError(Dart_Invoke(bytes_builder, add, 1, &uint8List));

        /*
        if (buffer_length) {
            (*output).insert((*output).end(), buffer, buffer + buffer_length);
        }
        */
    } while (result == BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT);

    // Release the input data.
    HandleError(Dart_TypedDataReleaseData(list_handle));

    Dart_Handle output_handle = Dart_NewList(3);
    Dart_ListSetAt(output_handle, 1, bytes_builder);
    Dart_ListSetAt(output_handle, 2, Dart_Null());

    // Throw an error if there was one.
    if (result != BROTLI_DECODER_RESULT_ERROR) Dart_ListSetAt(output_handle, 0, Dart_Null());
    else {
        BrotliDecoderErrorCode code = BrotliDecoderGetErrorCode(state);
        Dart_ListSetAt(output_handle, 0, Dart_NewStringFromCString(BrotliDecoderErrorString(code)));
        Dart_ListSetAt(output_handle, 2, Dart_NewInteger(code));
    }

    // And return.
    Dart_SetReturnValue(arguments, output_handle);

    // Delete the data we created.
    BrotliDecoderDestroyInstance(state);
    //delete output;
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
#include "napi.h"
#include <node_api.h>

#include <memory>

#include "marker/marker.hxx"
#include "marker/def/logging.dxx"
#include "marker/data/unmanaged_buffer.hxx"

#include "lib/str.hxx"
#include "lib/remem.hxx"

void finalize_buffer(Napi::Env env, uint8_t* data) {
    LOG_DEBUG("Finalizing buffer %p", data);
    REMEM_FREE(data);
}

Napi::Buffer<uint8_t> compile(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    std::unique_ptr<char, decltype(re::free)*> alias(
        strDup(info[0].As<Napi::String>().Utf8Value().c_str()), re::free
    );

    try {
        mker::Marker marker(alias.get());
        UnmanagedBuffer<char> rendered = marker.compile();

        return Napi::Buffer<uint8_t>::New<decltype(finalize_buffer)*>(
            env, (uint8_t*) rendered.get_data(), rendered.get_size(), finalize_buffer
        );
    } catch(std::exception &e) {
        throw Napi::Error::New(env, std::string("Compilation error error\n") + e.what());
    }
}

void destroy(void* args) {
    LOG_DEBUG("Destroying...");

    #ifdef REMEM_ENABLE_MAPPING
        re::mem_print();
    #endif
}

Napi::Object init(Napi::Env env, Napi::Object exports) {
    LOG_DEBUG("Init...\n");

    napi_add_env_cleanup_hook((napi_env) env, destroy, nullptr);

    exports["compile"] = Napi::Function::New(env, compile);

    return exports;
}

NODE_API_MODULE(MODULE_NAME, init)
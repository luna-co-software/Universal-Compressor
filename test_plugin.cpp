#include <iostream>
#include <dlfcn.h>

int main() {
    const char* plugin_path = "/home/marc/.vst3/Universal Compressor.vst3/Contents/x86_64-linux/Universal Compressor.so";
    
    std::cout << "Testing plugin loading..." << std::endl;
    
    // Try to load the plugin library
    void* handle = dlopen(plugin_path, RTLD_NOW | RTLD_LOCAL);
    
    if (!handle) {
        std::cerr << "Failed to load plugin: " << dlerror() << std::endl;
        return 1;
    }
    
    std::cout << "Plugin loaded successfully!" << std::endl;
    
    // Look for the VST3 entry point
    void* entry = dlsym(handle, "GetPluginFactory");
    if (!entry) {
        std::cerr << "Warning: GetPluginFactory not found: " << dlerror() << std::endl;
    } else {
        std::cout << "VST3 factory function found!" << std::endl;
    }
    
    // Clean up
    dlclose(handle);
    
    std::cout << "Test completed successfully - no segmentation fault detected" << std::endl;
    return 0;
}
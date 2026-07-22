#ifndef CHIP8_EXPORT_H
#define CHIP8_EXPORT_H

#ifdef _WIN32
    #ifdef CHIP8_SHARED_EXPORTS
        #define CHIP8_API __declspec(dllexport)
    #else
        #define CHIP8_API __declspec(dllimport)
    #endif
#else
    #define CHIP8_API __attribute__((visibility("default")))
#endif

#endif /* CHIP8_EXPORT_H */

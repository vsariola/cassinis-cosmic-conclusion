/*
            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
                    Version 2, December 2004

 Copyright (C) 2024 Mårten Rånge

 Everyone is permitted to copy and distribute verbatim or modified
 copies of this license document, and changing it is allowed as long
 as the name is changed.

            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

  0. You just DO WHAT THE FUCK YOU WANT TO.
*/

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#define WINDOWS_IGNORE_PACKING_MISMATCH
#define ATOM_STATIC 0xC019

#include "assert.h"
#include <stdio.h>
#include <windows.h>
#include <winuser.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <GL/gl.h>

#include "glext.h"
#include "song.h"

#define READ_PIXEL_WIDTH 512
#define READ_PIXEL_HEIGHT 512
#define READ_PIXEL_COUNT (READ_PIXEL_WIDTH * READ_PIXEL_HEIGHT)
//#define READ_PIXEL_CALLS (2 * SU_BUFFER_LENGTH + READ_PIXEL_COUNT - 1) / READ_PIXEL_COUNT
#define READ_PIXEL_CALLS 128
#define TOTAL_PIXELS_READ (READ_PIXEL_CALLS * READ_PIXEL_COUNT)
#define WAVEBUFFER_LEN (TOTAL_PIXELS_READ*4) // some extra because we're gonna overread

extern "C"
{

  struct MINI_DEVMODE
  {
    BYTE _a[36];
    DWORD dmSize;
    DWORD dmFields;
    struct
    {
      BYTE _b[4];
      DWORD pfdFlags;
    } pfd;
    BYTE _c[56];
    DWORD width;
    DWORD height;
    BYTE _d[8];
  };

  struct MINI_WAVEHDR
  {
    LPSTR lpData;
    DWORD dwBufferLength;
    DWORD dwBytesRecorded;
    DWORD_PTR dwUser;
    DWORD dwFlags;
    DWORD dwLoops;
  };

#pragma bss_seg(".wavbuf")
  unsigned char waveBuffer[WAVEBUFFER_LEN];
#pragma bss_seg(".fragprog")
  GLint fragmentShaderProgram;
#pragma bss_seg(".hwo")
  HWAVEOUT hwo;
#pragma bss_seg(".winsize")
  RECT windowSize;

#pragma data_seg(".waveFormatSpecification")
  WAVEFORMATEX waveFormatSpecification = {
      WAVE_FORMAT_PCM,                                      // wFormatTag
      SU_CHANNEL_COUNT,                                     // nChannels
      SU_SAMPLE_RATE,                                       // nSamplesPerSec
      SU_SAMPLE_RATE * sizeof(SUsample) * SU_CHANNEL_COUNT, // nAvgBytesPerSec
      sizeof(SUsample) * SU_CHANNEL_COUNT,                  // nBlockAlign
      sizeof(SUsample) * 8,                                 // wBitsPerSample
      0                                                     // cbSize
  };

#pragma data_seg(".waveHeader")
  MINI_WAVEHDR waveHeader = {
      (LPSTR)waveBuffer,                   // lpData
      SU_BUFFER_LENGTH * sizeof(SUsample), // dwBufferLength
      0,                                   // dwBytesRecorded
      0,                                   // dwUser
      WHDR_PREPARED,                       // dwFlags
      0                                    // dwLoops
  };

#pragma data_seg(".waveTime")
  MMTIME waveTime = {TIME_BYTES, 0};

  extern const char nm_glCreateShaderProgramv[];
  extern const char nm_glUseProgram[];
  extern const char nm_glUniform3i[];

#pragma data_seg(".fragmentShaders")
#include <shader.inl>

#ifdef _DEBUG
  void APIENTRY debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const *message, void const *userParam)
  {
    printf(message);
    printf("\n");
  }
  char debugLog[0xFFFF];
#endif

} // extern "C"

#pragma code_seg(".main")
#ifdef USE_CRINKLER
_declspec(naked) void entrypoint() {
#else
int __cdecl main() {
#endif

#if _DEBUG
#if FULLSCREEN
  auto dwStyle = WS_POPUP | WS_VISIBLE | WS_MAXIMIZE;
  auto xres = 0;
  auto yres = 0;
#else
  auto dwStyle = WS_VISIBLE | WS_OVERLAPPEDWINDOW | WS_POPUP;
  auto xres = 1920;
  auto yres = 1080;
#endif

  // Create the window using the STATIC class
  auto hwnd = CreateWindowExA(
      0,                                     // dwExStyle Special name for STATIC window class
      reinterpret_cast<LPCSTR>(ATOM_STATIC), // lpClassName
      nullptr,                               // lpWindowName
      dwStyle,                               // dwStyle
      0,                                     // nX
      0,                                     // nY
      xres,                                  // nWidth
      yres,                                  // nHeight
      nullptr,                               // hWndParent
      nullptr,                               // hMenu
      nullptr,                               // hInstance
      nullptr                                // lpParam
  );
  assert(hwnd);

  // We need the Device Context to do Windows graphics
  auto hdc = GetDC(hwnd);
  assert(hdc);

  GetWindowRect(hwnd, &windowSize);

  // Set the pixel format on the Device Context to prepare it for OpenGL
  auto setOk = SetPixelFormat(hdc, 8, nullptr);
  assert(setOk);

  // Create OpenGL Context
  auto hglrc = wglCreateContext(hdc);
  assert(hglrc);

  // Sets the OpenGL context as the current one
  auto makeOk = wglMakeCurrent(hdc, hglrc);
  assert(makeOk);

  // Init our demo
  // Bit of debugging info during debug builds
  //  Don't want to waste bytes on that in Release mode
  glEnable(GL_DEBUG_OUTPUT);
  ((PFNGLDEBUGMESSAGECALLBACKPROC)wglGetProcAddress("glDebugMessageCallback"))(debugCallback, 0);

  // Compiles the provided fragment shader into a shader program
  fragmentShaderProgram = ((PFNGLCREATESHADERPROGRAMVPROC)wglGetProcAddress(nm_glCreateShaderProgramv))(GL_FRAGMENT_SHADER, 1, &shader_frag);

  ((PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetProgramInfoLog"))(fragmentShaderProgram, sizeof(debugLog), NULL, debugLog);
  printf(debugLog);
  glDisable(GL_DEBUG_OUTPUT);

  auto waveOpenOk = waveOutOpen(&hwo, WAVE_MAPPER, &waveFormatSpecification, NULL, 0, CALLBACK_NULL);
  assert(waveOpenOk == MMSYSERR_NOERROR);

  auto done = false;
  auto music_saved = false;
  auto time_in_bytes = -TOTAL_PIXELS_READ;

  // Loop until done
  do
  {    
    MSG msg;
    // The classic window message pump
    while (PeekMessageA(&msg, 0, 0, 0, PM_REMOVE))
    {
      if (msg.message == WM_QUIT)
        done = 1;
      // Result intentionally ignored
      TranslateMessage(&msg);
      // Result intentionally ignored
      DispatchMessageA(&msg);
    }    
    
    // Use the previously compiled shader program
    ((PFNGLUSEPROGRAMPROC)wglGetProcAddress(nm_glUseProgram))(fragmentShaderProgram);

    // Set the uniform values for the shader, giving resolution and time
    ((PFNGLUNIFORM3IPROC)wglGetProcAddress("glUniform3i"))(
        0,
        windowSize.right,
        windowSize.bottom,
        time_in_bytes);

    // Draws a rect over the entire window with fragment shader providing the gfx
    glRects(-1, -1, 1, 1);

    glReadPixels(0, 0, READ_PIXEL_WIDTH, READ_PIXEL_HEIGHT, GL_RED, GL_UNSIGNED_BYTE, waveBuffer + TOTAL_PIXELS_READ + time_in_bytes);
    time_in_bytes += READ_PIXEL_COUNT;
    if (time_in_bytes < 0)      
      continue;    
    
#if SAVE_MUSIC
    if (!music_saved)
    {
        FILE* f = fopen("buffer", "wb");
        fwrite(waveBuffer, sizeof(SUsample), SU_BUFFER_LENGTH, f);
        fclose(f);
        music_saved = true;
    }
#endif

    // start music
    auto waveWriteOk = waveOutWrite(hwo, (LPWAVEHDR)&waveHeader, 0x20);
    assert(waveWriteOk == MMSYSERR_NOERROR);

    // Get current wave position
    auto waveGetPosOk = waveOutGetPosition(hwo, &waveTime, sizeof(MMTIME));
    assert(waveGetPosOk == MMSYSERR_NOERROR);
    time_in_bytes = waveTime.u.cb;

  } while (!GetAsyncKeyState(VK_ESCAPE) && time_in_bytes < SU_LENGTH_IN_SAMPLES * 4);

  // We are done, just exit. No need to waste bytes on cleaning
  //  up resources. Windows will do it for us.

#ifdef USE_CRINKLER
      ExitProcess(0);
#else
      return 0;
#endif

#else
  _asm {
    xor esi, esi
    mov ebp, -TOTAL_PIXELS_READ

    push esi // ExitProcess.uExitCode

    push esi // waveOutOpen.fdwOpen
    push esi // waveOutOpen.dwInstance
    push esi // waveOutOpen.dwCallback
    push offset waveFormatSpecification // waveOutOpen.pwfx
    push - 1 // waveOutOpen.uDeviceID
    push offset hwo // waveOutOpen.phwo

    push offset shader_frag // glCreateShaderProgram.strings
    push 1 // glCreateShaderProgram.count
    push GL_FRAGMENT_SHADER // glCreateShaderProgram.type
    push offset nm_glCreateShaderProgramv // wglGetProcAddress.procName

    push esi // ShowCursor.bShow

    push esi // CreateWindowExA.lpParam
    push esi // CreateWindowExA.hInstance
    push esi // CreateWindowExA.hMenu
    push esi // CreateWindowExA.hWndParent
    push esi // CreateWindowExA.nHeight
    push esi // CreateWindowExA.nWidth
    push esi // CreateWindowExA.Y
    push esi // CreateWindowExA.X
    push WS_POPUP | WS_VISIBLE | WS_MAXIMIZE // CreateWindowExA.dwStyle
    push esi // CreateWindowExA.lpWindowName
    push ATOM_STATIC // CreateWindowExA.lpClassName
    push esi // CreateWindowExA.dwExStyle

    call CreateWindowExA

    push eax // GetDC.hWnd

    push offset windowSize // GetWindowRect.lprect
    push eax // GetWindowRect.hWnd
    call GetWindowRect

    call GetDC

    xchg edi, eax

    push edi // wglCreateContext.hdc

    push esi // SetPixelFormat.ppfd
    push 8 // SetPixelFormat.format
    push edi // SetPixelFormat.hdc

    call SetPixelFormat

    call wglCreateContext

    push eax // wglMakeCurrent.glrc
    push edi // wglMakeCurrent.hdc

    call wglMakeCurrent

    call ShowCursor

    call wglGetProcAddress

    call eax // glCreateShaderProgram (indirect call)
    xchg edi, eax

    call waveOutOpen

  mainloop:
    push VK_ESCAPE // GetAsyncKeyState.vKey

    push 0x20 // waveOutWrite.cbwh
    push offset waveHeader // waveOutWrite.pwh
    push [hwo] // waveOutWrite.hwo

    push 0xC // waveOutGetPosition.cbmmt
    push offset waveHeader + 16 // waveOutGetPosition.pmmt
    push[hwo] // waveOutGetPosition.hwo

readloop:
    push 1 // glRects.y2
    push 1 // glRects.x2
    push -1 // glRects.y1
    push -1 // glRects.x1

    push            ebp // glUniform3i.v2 (time in bytes)
    push            dword ptr[windowSize.bottom] // glUniform3i.v1
    push            dword ptr[windowSize.right] // glUniform3i.v0
    push            esi // glUniform3i.location

    push            offset nm_glUniform3i // wglGetProcAddress.procName

    push            edi // glUseProgram.pid

    push            offset nm_glUseProgram // wglGetProcAddress.procName

    call            wglGetProcAddress

    call            eax // glUseProgram (indirect call)

    call            wglGetProcAddress

    call            eax // glUniform4i (indirect call)

    call            glRects

    lea             eax, [waveBuffer + TOTAL_PIXELS_READ + ebp]
    push            eax // glReadPixels.data
    push            GL_UNSIGNED_BYTE // glReadPixels.type
    push            GL_RED // glReadPixels.format
    push            READ_PIXEL_HEIGHT // glReadPixels.height
    push            READ_PIXEL_WIDTH // glReadPixels.width
    push            esi // glReadPixels.y
    push            esi // glReadPixels.x
    call            glReadPixels
        
    add             ebp, READ_PIXEL_COUNT
    js              readloop

    call waveOutGetPosition    
    call waveOutWrite
    call GetAsyncKeyState
    sahf
    js exit
    mov  ebp, dword ptr[waveHeader + 20]
    cmp ebp, SU_LENGTH_IN_SAMPLES*4
    js mainloop

  exit:

    call ExitProcess
  }
#endif
}

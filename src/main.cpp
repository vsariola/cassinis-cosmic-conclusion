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
#define READ_PIXEL_CALLS (2 * SU_BUFFER_LENGTH + READ_PIXEL_COUNT - 1) / READ_PIXEL_COUNT

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

#pragma bss_seg(".wavbuf")
  unsigned char waveBuffer[READ_PIXEL_CALLS * READ_PIXEL_COUNT];
#pragma bss_seg(".fragprog")
  GLint fragmentShaderProgram;
#pragma bss_seg(".hwo")
  HWAVEOUT hwo;
#pragma bss_seg(".winsize")
  RECT windowSize;

#pragma data_seg(".devmode")
  MINI_DEVMODE devmode = {
      "", sizeof(devmode), DM_PELSWIDTH | DM_PELSHEIGHT, "", PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL, "", XRES, YRES, ""};

#pragma data_seg(".pixelFormatDescriptor")
  PIXELFORMATDESCRIPTOR pixelFormatSpecification{
      sizeof(PIXELFORMATDESCRIPTOR),                              // nSize
      1,                                                          // nVersion
      PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, // dwFlags
      PFD_TYPE_RGBA,                                              // iPixelType
      32,                                                         // cColorBits
      0,                                                          // cRedBits
      0,                                                          // cRedShift
      0,                                                          // cGreenBits
      0,                                                          // cGreenShift
      0,                                                          // cBlueBits
      0,                                                          // cBlueShift
      8,                                                          // cAlphaBits
      0,                                                          // cAlphaShift
      0,                                                          // cAccumBits
      0,                                                          // cAccumRedBits
      0,                                                          // cAccumGreenBits
      0,                                                          // cAccumBlueBits
      0,                                                          // cAccumAlphaBits
      32,                                                         // cDepthBits
      0,                                                          // cStencilBits
      0,                                                          // cAuxBuffers
      PFD_MAIN_PLANE,                                             // iLayerType
      0,                                                          // bReserved
      0,                                                          // dwLayerMask
      0,                                                          // dwVisibleMask
      0,                                                          // dwDamageMask
  };

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
  WAVEHDR waveHeader = {
      (LPSTR)waveBuffer,                   // lpData
      SU_BUFFER_LENGTH * sizeof(SUsample), // dwBufferLength
      0,                                   // dwBytesRecorded
      0,                                   // dwUser
      WHDR_PREPARED,                       // dwFlags
      0,                                   // dwLoops
      0,                                   // lpNext
      0                                    // reserved
  };

#pragma data_seg(".waveTime")
  MMTIME waveTime = {TIME_SAMPLES, 0};

  extern const char nm_glCreateShaderProgramv[];
  extern const char nm_glUseProgram[];
  extern const char nm_glUniform4i[];

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
_declspec(naked) void entrypoint()
{
#else
int __cdecl main()
{

#endif

#if _DEBUG
  auto dwStyle = WS_VISIBLE | WS_OVERLAPPEDWINDOW | WS_POPUP;

  // Create the window using the STATIC class
  auto hwnd = CreateWindowExA(
      0,                                     // dwExStyle Special name for STATIC window class
      reinterpret_cast<LPCSTR>(ATOM_STATIC), // lpClassName
      nullptr,                               // lpWindowName
      dwStyle,                               // dwStyle
      0,                                     // nX
      0,                                     // nY
      XRES,                                  // nWidth
      YRES,                                  // nHeight
      nullptr,                               // hWndParent
      nullptr,                               // hMenu
      nullptr,                               // hInstance
      nullptr,                               // lpParam
  );
  assert(hwnd);

  // We need the Device Context to do Windows graphics
  auto hdc = GetDC(hwnd);
  assert(hdc);

  GetWindowRect(hwnd, &windowSize);

  // Set the pixel format on the Device Context to prepare it for OpenGL
  auto setOk = SetPixelFormat(
      hdc, 8, nullptr);
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
#ifdef _DEBUG
  glEnable(GL_DEBUG_OUTPUT);
  ((PFNGLDEBUGMESSAGECALLBACKPROC)wglGetProcAddress("glDebugMessageCallback"))(debugCallback, 0);
#endif

  // Compiles the provided fragment shader into a shader program
  fragmentShaderProgram = ((PFNGLCREATESHADERPROGRAMVPROC)wglGetProcAddress(nm_glCreateShaderProgramv))(GL_FRAGMENT_SHADER, 1, &shader_frag);

#ifdef _DEBUG
  ((PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetProgramInfoLog"))(fragmentShaderProgram, sizeof(debugLog), NULL, debugLog);
  printf(debugLog);
  glDisable(GL_DEBUG_OUTPUT);
#endif
  auto waveOpenOk = waveOutOpen(
      &hwo, WAVE_MAPPER, &waveFormatSpecification, NULL, 0, CALLBACK_NULL);
  assert(waveOpenOk == MMSYSERR_NOERROR);

  auto wavePrepareOk = waveOutPrepareHeader(hwo, &waveHeader, sizeof(waveHeader));
  assert(wavePrepareOk == MMSYSERR_NOERROR);

  auto waveWriteOk = waveOutWrite(hwo, &waveHeader, sizeof(waveHeader));
  assert(waveWriteOk == MMSYSERR_NOERROR);

  auto done = false;
  auto readpixel_offset = (READ_PIXEL_CALLS - 1) * READ_PIXEL_COUNT;
  auto buffer = waveBuffer;
  auto music_saved = false;

  // Loop until done
  while (!done)
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

    // If ESCAPE is pressed we are done
    if (GetAsyncKeyState(VK_ESCAPE))
      done = 1;

    // Windows message handling done, let's draw some gfx

    // Get current wave position
    auto waveGetPosOk = waveOutGetPosition(hwo, &waveTime, sizeof(MMTIME));
    assert(waveGetPosOk == MMSYSERR_NOERROR);

    // Have we passed the end sample? If so then we are done
    auto currentSample = waveTime.u.sample;
    if (currentSample >= SU_LENGTH_IN_SAMPLES)
      done = 1;

    ((PFNGLUNIFORM4IPROC)wglGetProcAddress("glUniform4i"))(0, windowSize.right, windowSize.bottom, currentSample, readpixel_offset);

    // Use the previously compiled shader program
    ((PFNGLUSEPROGRAMPROC)wglGetProcAddress(nm_glUseProgram))(fragmentShaderProgram);

    // Draws a rect over the entire window with fragment shader providing the gfx
    glRects(-1, -1, 1, 1);

    if (readpixel_offset >= 0)
    {
      glReadPixels(0, 0, READ_PIXEL_WIDTH, READ_PIXEL_HEIGHT, GL_RED, GL_UNSIGNED_BYTE, waveBuffer + readpixel_offset);
#if SAVE_MUSIC
    }
    else if (!music_saved)
    {
      FILE *f = fopen("buffer", "wb");
      fwrite(waveBuffer, sizeof(SUsample), SU_BUFFER_LENGTH, f);
      fclose(f);
      music_saved = true;
#endif
    }
    readpixel_offset -= READ_PIXEL_COUNT;

    // Swap the buffers to present the gfx
    auto swapOk = SwapBuffers(hdc);
    assert(swapOk);
  }

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
    mov ebx, READ_PIXEL_CALLS * READ_PIXEL_COUNT

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
    push offset windowSize // GetWindowRect.rect
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
    mov fragmentShaderProgram, eax

    call waveOutOpen

    mainloop:
      push VK_ESCAPE // GetAsyncKeyState.vKey

                // push esi // PeekMessage.wRemoveMsg
                // push esi // PeekMessage.wMsgFilterMax
                // push esi // PeekMessage.wMsgFilterMin
                // push esi // PeekMessage.hWnd
                // push esi // PeekMessage.lpMsg

                // push edi // SwapBuffers.hdc

        push 0x20 // waveOutWrite.cbwh
        push offset waveHeader // waveOutWrite.pwh
        push[hwo] // waveOutWrite.hwo

readloop:
      push 1 // glRects.y2
      push 1 // glRects.x2
      push -1 // glRects.y1
      push -1 // glRects.x1

      push 0xC // waveOutGetPosition.cbmmt
      push offset waveTime // waveOutGetPosition.pmmt
      push [hwo] // waveOutGetPosition.hwo

      call waveOutGetPosition

      mov  ebp, dword ptr[waveTime.u.sample]

      push            ebx // frame
      push            ebp // time
      push            dword ptr[windowSize.bottom]
      push            dword ptr[windowSize.right]
      push            esi // glUniform4i.location

      push            offset nm_glUniform4i // wglGetProcAddress.procName

      push            [fragmentShaderProgram] // glUseProgram.pid

      push            offset nm_glUseProgram // wglGetProcAddress.procName

      call            wglGetProcAddress

      call            eax // glUseProgram (indirect call)

      call            wglGetProcAddress

      call            eax // glUniform4i (indirect call)

      call            glRects

      sub             ebx, READ_PIXEL_COUNT
      js              noread

      lea             eax, [waveBuffer+ebx]
      push            eax // glReadPixels.data
      push            GL_UNSIGNED_BYTE // glReadPixels.type
      push            GL_RED // glReadPixels.format
      push            READ_PIXEL_HEIGHT // glReadPixels.height
      push            READ_PIXEL_WIDTH // glReadPixels.width
      push            esi // glReadPixels.y
      push            esi // glReadPixels.x
      call            glReadPixels
      jmp             readloop
    noread:

      call waveOutWrite // start music only after all reads are done

            // call SwapBuffers
            // call PeekMessageA
      call GetAsyncKeyState
      sahf
      js exit
      cmp ebp, SU_LENGTH_IN_SAMPLES
      js mainloop

  exit:

    call ExitProcess
  }
#endif
}

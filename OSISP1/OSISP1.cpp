// OSISP1.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "OSISP1.h"
#include <iostream>

#define WM_UPDATE_SPRITE WM_USER
#define BACKGROUND_COLOR 18
#define TIMER_START_MOVEMENTS 1001
#define TIMER_CONTINUE_MOVEMENTS 1002

constexpr auto PATH = L"C:\\Users\\Veronika\\Documents\\work\\5 сем\\ОСиСП\\OSISP1\\sprite.bmp";
constexpr auto step = 10;

constexpr auto VK_W = 0x57;
constexpr auto VK_A = 0x41;
constexpr auto VK_S = 0x53;
constexpr auto VK_D = 0x44;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
bool showSprite(HWND hwnd, COORD coordiantes);
void loadSprite(HWND hwnd);
COORD createNewSpritePosition(HWND hwnd,short xStep, short yStep);
SIZE GetSpriteSize(HBITMAP hBitmap);
SIZE GetClientWindowSize(HWND hWnd);
int FillWindowWithColor(HWND hWnd, COLORREF color);
void changeSpritePosition(HWND hwnd, short x, short y);
void setTimer(HWND hwnd, UINT_PTR timerId);
void createAutomaticMovements(HWND hwnd);

HBITMAP hSprite;
COORD spritePosition = { 0, 0 };
SIZE spriteSize;
bool reversedx = false;
bool reversedy = false;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    // Register the window class.
    const wchar_t CLASS_NAME[] = L"LR1";

    WNDCLASS wc = { };

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"OSaSP Laboratory work 1",     // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    loadSprite(hwnd);
    SetTimer(hwnd,TIMER_START_MOVEMENTS,8000,NULL);

    MSG msg = { }; 
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);        
        DispatchMessage(&msg); //отправляет сообщение оконной процедуре
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_UPDATE_SPRITE:
        showSprite(hwnd,spritePosition);
        break;
    case WM_TIMER:
        if (wParam == TIMER_START_MOVEMENTS)
        {
            setTimer(hwnd, TIMER_CONTINUE_MOVEMENTS);
        }
        else {
            createAutomaticMovements(hwnd);
        }
        break;
    case WM_DESTROY:
        KillTimer(hwnd, TIMER_START_MOVEMENTS);
        KillTimer(hwnd, TIMER_CONTINUE_MOVEMENTS);
        PostQuitMessage(0);//отправляет сообщение WM_QUIT в очередь сообщений потока. WM_QUIT завершает программу путем возвращения в GetMessage нуля
        break;
    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_UP:
        case VK_W:
            changeSpritePosition(hwnd, 0, -step);
            setTimer(hwnd, TIMER_START_MOVEMENTS);
            break;      
        case VK_RIGHT:
        case VK_D:
            changeSpritePosition(hwnd, step, 0);
            setTimer(hwnd, TIMER_START_MOVEMENTS);
            break;
        case VK_DOWN:
        case VK_S:
            changeSpritePosition(hwnd, 0, step);
            setTimer(hwnd, TIMER_START_MOVEMENTS);
            break;
        case VK_LEFT:
        case VK_A:
            changeSpritePosition(hwnd, -step, 0);
            setTimer(hwnd, TIMER_START_MOVEMENTS);
            break;
        default:
            break;
        }
        break;
    case WM_MOUSEWHEEL:
        if (GET_KEYSTATE_WPARAM(wParam) != MK_SHIFT)
        {
            if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
            {
                changeSpritePosition(hwnd, 0, step);
                setTimer(hwnd, TIMER_START_MOVEMENTS);
            }
            else
            {
                changeSpritePosition(hwnd, 0, -step);
                setTimer(hwnd, TIMER_START_MOVEMENTS);
            }
        }
        else {
            if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
            {
                changeSpritePosition(hwnd, -step, 0);
                setTimer(hwnd, TIMER_START_MOVEMENTS);
            }
            else
            {
                changeSpritePosition(hwnd, step, 0);
                setTimer(hwnd, TIMER_START_MOVEMENTS);
            }
        }
        break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);//обеспечивает обработку всех сообщений
    }
    return 0;
}

bool showSprite(HWND hwnd, COORD coordiantes) {
    HDC winDC = GetDC(hwnd); 
    HDC memDC = CreateCompatibleDC(winDC); 
    HGDIOBJ oldBmp = SelectObject(memDC, hSprite);

    BitBlt(winDC, coordiantes.X, coordiantes.Y, spriteSize.cx, spriteSize.cy, memDC, 0, 0, SRCCOPY);
    SelectObject(memDC, oldBmp);
    DeleteDC(memDC);
    ReleaseDC(hwnd, winDC);
    return true;
}

void loadSprite(HWND hwnd) {
    hSprite = (HBITMAP)LoadImageW(NULL, PATH, IMAGE_BITMAP, 200, 200, LR_LOADFROMFILE);

    if (hSprite != NULL) {
        spriteSize = GetSpriteSize(hSprite);
        showSprite(hwnd, spritePosition);
    }
}


SIZE GetClientWindowSize(HWND hWnd)
{
    RECT windowRectangle;
    GetClientRect(hWnd, &windowRectangle);

    SIZE windowSize;
    windowSize.cx = windowRectangle.right - windowRectangle.left;
    windowSize.cy = windowRectangle.bottom - windowRectangle.top;

    return windowSize;
}

SIZE GetSpriteSize(HBITMAP hBitmap)
{
    BITMAP sprite;
    GetObject(hBitmap, sizeof(BITMAP), &sprite);

    SIZE spriteSize;
    spriteSize.cx = sprite.bmWidth;
    spriteSize.cy = sprite.bmHeight;

    return spriteSize;
}

COORD createNewSpritePosition(HWND hwnd,short xSTEP, short yStep) {
    SIZE windowSize = GetClientWindowSize(hwnd);

    COORD newSpritePosition;

    newSpritePosition.X = (spritePosition.X + xSTEP);
    if (newSpritePosition.X < 0)
    {
        newSpritePosition.X = 0 + 4*step;
    }
    else if (newSpritePosition.X + spriteSize.cx > windowSize.cx)
    {
        newSpritePosition.X = windowSize.cx - spriteSize.cx - 4 * step;
    }
    
    newSpritePosition.Y = (spritePosition.Y + yStep);
    if (newSpritePosition.Y < 0)
    {
        newSpritePosition.Y = 0 + 4 * step;
    }
    else if (newSpritePosition.Y + spriteSize.cy > windowSize.cy)
    {
        newSpritePosition.Y = windowSize.cy - spriteSize.cy - 4 * step;
    }
    
    return newSpritePosition;
}

void changeSpritePosition(HWND hwnd, short x, short y) {
    spritePosition = createNewSpritePosition(hwnd, x, y);
    FillWindowWithColor(hwnd, GetSysColor(COLOR_WINDOW));
    showSprite(hwnd, spritePosition);
}

int FillWindowWithColor(HWND hWnd, COLORREF color)
{
    RECT windowRectangle;
    GetClientRect(hWnd, &windowRectangle);

    HDC windowDC = GetDC(hWnd);
    HBRUSH hBrush = CreateSolidBrush(color);

    int result = FillRect(windowDC, &windowRectangle, hBrush);

    DeleteObject(hBrush);
    ReleaseDC(hWnd, windowDC);

    return result;
}

void setTimer(HWND hwnd, UINT_PTR timerId) {
    if (timerId == TIMER_START_MOVEMENTS) {
        SetTimer(hwnd, timerId, 10000, NULL);
        KillTimer(hwnd, TIMER_CONTINUE_MOVEMENTS);
    }
    else {
        SetTimer(hwnd, timerId, 1000, NULL);
        KillTimer(hwnd, TIMER_START_MOVEMENTS);
    }
}

void createAutomaticMovements(HWND hwnd) {
    SIZE windowSize = GetClientWindowSize(hwnd);

    COORD newSpritePosition;
    int xStep = reversedx ? -step : step;
    newSpritePosition.X = (spritePosition.X + xStep);
    if (newSpritePosition.X < 0)
    {
        reversedx = false;
        newSpritePosition.X = 0 + step;
    }
    else if (newSpritePosition.X + spriteSize.cx > windowSize.cx)
    {
        reversedx = true;
        newSpritePosition.X = (SHORT)(windowSize.cx - spriteSize.cx - step);
    }
    int yStep = reversedy ? -step : step;
    newSpritePosition.Y = (spritePosition.Y + yStep);
    if (newSpritePosition.Y < 0)
    {
        reversedy = false;
        newSpritePosition.Y = 0 +  step;
    }
    else if (newSpritePosition.Y + spriteSize.cy > windowSize.cy)
    {
        reversedy = true;
        newSpritePosition.Y = (SHORT)(windowSize.cy - spriteSize.cy - step);
    }

    spritePosition = newSpritePosition;
    FillWindowWithColor(hwnd, GetSysColor(COLOR_WINDOW));
    showSprite(hwnd, spritePosition);
}
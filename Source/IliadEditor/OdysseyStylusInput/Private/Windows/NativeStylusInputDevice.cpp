// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "NativeStylusInputDevice.h"

void
GetSubpixelLocation(POINTER_PEN_INFO& iPenInfo, float* oX, float* oY )
{
    RECT deviceRect;
    RECT displayRect;
    GetPointerDeviceRects(iPenInfo.pointerInfo.sourceDevice, &deviceRect, &displayRect);

    float hX = iPenInfo.pointerInfo.ptHimetricLocationRaw.x;
    float hY = iPenInfo.pointerInfo.ptHimetricLocationRaw.y;

    LONG deviceW = deviceRect.right - deviceRect.left;
    LONG deviceH = deviceRect.bottom - deviceRect.top;

    LONG displayW = displayRect.right - displayRect.left;
    LONG displayH = displayRect.bottom - displayRect.top;

    *oX = (((hX - deviceRect.left) / deviceW) * displayW) + displayRect.left;
    *oY = (((hY - deviceRect.top) / deviceH) * displayH) + displayRect.top;
}

void LogPenInfo(POINTER_PEN_INFO& iPenInfo)
{
    FString pointerType = TEXT("");
    switch(iPenInfo.pointerInfo.pointerType)
    {
        case PT_POINTER : pointerType = TEXT("POINTER"); break;
        case PT_TOUCH : pointerType = TEXT("TOUCH"); break;
        case PT_PEN : pointerType = TEXT("PEN"); break;
        case PT_MOUSE : pointerType = TEXT("MOUSE"); break;
        case PT_TOUCHPAD : pointerType = TEXT("TOUCHPAD"); break;
    }

    FString pointerFlags = TEXT("");
    switch(iPenInfo.pointerInfo.pointerFlags)
    {
        case POINTER_FLAG_NEW : pointerFlags.Append(TEXT("NEW | ")); break;
        case POINTER_FLAG_INRANGE : pointerFlags.Append(TEXT("INRANGE | ")); break;
        case POINTER_FLAG_INCONTACT : pointerFlags.Append(TEXT("INCONTACT | ")); break;
        case POINTER_FLAG_FIRSTBUTTON : pointerFlags.Append(TEXT("FIRSTBUTTON | ")); break;
        case POINTER_FLAG_SECONDBUTTON : pointerFlags.Append(TEXT("SECONDBUTTON | ")); break;
        case POINTER_FLAG_THIRDBUTTON : pointerFlags.Append(TEXT("THIRDBUTTON | ")); break;
        case POINTER_FLAG_FOURTHBUTTON : pointerFlags.Append(TEXT("FOURTHBUTTON | ")); break;
        case POINTER_FLAG_FIFTHBUTTON : pointerFlags.Append(TEXT("FIFTHBUTTON | ")); break;
        case POINTER_FLAG_PRIMARY : pointerFlags.Append(TEXT("PRIMARY | ")); break;
        case POINTER_FLAG_CONFIDENCE : pointerFlags.Append(TEXT("CONFIDENCE | ")); break;
        case POINTER_FLAG_CANCELED : pointerFlags.Append(TEXT("CANCELED | ")); break;
        case POINTER_FLAG_DOWN : pointerFlags.Append(TEXT("DOWN | ")); break;
        case POINTER_FLAG_UPDATE : pointerFlags.Append(TEXT("UPDATE | ")); break;
        case POINTER_FLAG_UP : pointerFlags.Append(TEXT("UP | ")); break;
        case POINTER_FLAG_WHEEL : pointerFlags.Append(TEXT("WHEEL | ")); break;
        case POINTER_FLAG_HWHEEL : pointerFlags.Append(TEXT("HWHEEL | ")); break;
        case POINTER_FLAG_CAPTURECHANGED : pointerFlags.Append(TEXT("CAPTURECHANGED | ")); break;
        case POINTER_FLAG_HASTRANSFORM : pointerFlags.Append(TEXT("HASTRANSFORM | ")); break;
    }

    FString buttonChangeType = TEXT("");
    switch(iPenInfo.pointerInfo.ButtonChangeType)
    {
        case POINTER_CHANGE_NONE : buttonChangeType = TEXT("NONE"); break;
        case POINTER_CHANGE_FIRSTBUTTON_DOWN : buttonChangeType = TEXT("FIRSTBUTTON_DOWN"); break;
        case POINTER_CHANGE_FIRSTBUTTON_UP : buttonChangeType = TEXT("FIRSTBUTTON_UP"); break;
        case POINTER_CHANGE_SECONDBUTTON_DOWN : buttonChangeType = TEXT("SECONDBUTTON_DOWN"); break;
        case POINTER_CHANGE_SECONDBUTTON_UP : buttonChangeType = TEXT("SECONDBUTTON_UP"); break;
        case POINTER_CHANGE_THIRDBUTTON_DOWN : buttonChangeType = TEXT("THIRDBUTTON_DOWN"); break;
        case POINTER_CHANGE_THIRDBUTTON_UP : buttonChangeType = TEXT("THIRDBUTTON_UP"); break;
        case POINTER_CHANGE_FOURTHBUTTON_DOWN : buttonChangeType = TEXT("FOURTHBUTTON_DOWN"); break;
        case POINTER_CHANGE_FOURTHBUTTON_UP : buttonChangeType = TEXT("FOURTHBUTTON_UP"); break;
        case POINTER_CHANGE_FIFTHBUTTON_DOWN : buttonChangeType = TEXT("FIFTHBUTTON_DOWN"); break;
        case POINTER_CHANGE_FIFTHBUTTON_UP : buttonChangeType = TEXT("FIFTHBUTTON_UP"); break;
    }

    float subX, subY;
    GetSubpixelLocation(iPenInfo, &subX, &subY);

    RECT deviceRect;
    RECT displayRect;
    GetPointerDeviceRects(iPenInfo.pointerInfo.sourceDevice, &deviceRect, &displayRect);

    UE_LOG(LogTemp, Warning, TEXT("------------------PEN_INFO------------------"));
    UE_LOG(LogTemp, Warning, TEXT("HasPressure : %s"), iPenInfo.penMask & PEN_MASK_PRESSURE ? TEXT("TRUE") : TEXT("FALSE"));
    UE_LOG(LogTemp, Warning, TEXT("HasRotation : %s"), iPenInfo.penMask & PEN_MASK_ROTATION ? TEXT("TRUE") : TEXT("FALSE"));
    UE_LOG(LogTemp, Warning, TEXT("HasTiltX : %s"), iPenInfo.penMask & PEN_MASK_TILT_X ? TEXT("TRUE") : TEXT("FALSE"));
    UE_LOG(LogTemp, Warning, TEXT("HasTiltY : %s"), iPenInfo.penMask & PEN_MASK_TILT_Y ? TEXT("TRUE") : TEXT("FALSE"));
    UE_LOG(LogTemp, Warning, TEXT("Pressure : %d"), iPenInfo.pressure);
    UE_LOG(LogTemp, Warning, TEXT("Rotation : %d"), iPenInfo.rotation);
    UE_LOG(LogTemp, Warning, TEXT("TiltX : %d"), iPenInfo.tiltX);
    UE_LOG(LogTemp, Warning, TEXT("TiltY : %d"), iPenInfo.tiltY);
    UE_LOG(LogTemp, Warning, TEXT("IsBarrelButtonDown : %s"), iPenInfo.penFlags & PEN_FLAG_BARREL ? TEXT("TRUE") : TEXT("FALSE"));
    UE_LOG(LogTemp, Warning, TEXT("IsInverted : %s"), iPenInfo.penFlags & PEN_FLAG_INVERTED ? TEXT("TRUE") : TEXT("FALSE"));
    UE_LOG(LogTemp, Warning, TEXT("IsEraserButtonDown : %s"), iPenInfo.penFlags & PEN_FLAG_ERASER ? TEXT("TRUE") : TEXT("FALSE"));
    UE_LOG(LogTemp, Warning, TEXT("----------------POINTER_INFO----------------"));
    UE_LOG(LogTemp, Warning, TEXT("Pointer Type : %s"), *pointerType);
    UE_LOG(LogTemp, Warning, TEXT("Pointer ID : %d"), iPenInfo.pointerInfo.pointerId);
    UE_LOG(LogTemp, Warning, TEXT("Frame ID : %d"), iPenInfo.pointerInfo.frameId);
    UE_LOG(LogTemp, Warning, TEXT("Pointer Flags : %s"), *pointerFlags);
    UE_LOG(LogTemp, Warning, TEXT("Source Device : %d"), iPenInfo.pointerInfo.sourceDevice);
    UE_LOG(LogTemp, Warning, TEXT("HWND Target : %d"), iPenInfo.pointerInfo.hwndTarget);
    UE_LOG(LogTemp, Warning, TEXT("Pixel Location : x(%d) y(%d)"), iPenInfo.pointerInfo.ptPixelLocation.x, iPenInfo.pointerInfo.ptPixelLocation.y);
    UE_LOG(LogTemp, Warning, TEXT("Himetric Location : x(%d) y(%d)"), iPenInfo.pointerInfo.ptHimetricLocation.x, iPenInfo.pointerInfo.ptHimetricLocation.y);
    UE_LOG(LogTemp, Warning, TEXT("Pixel Location Raw : x(%d) y(%d)"), iPenInfo.pointerInfo.ptPixelLocationRaw.x, iPenInfo.pointerInfo.ptPixelLocationRaw.y);
    UE_LOG(LogTemp, Warning, TEXT("Himetric Location Raw : x(%d) y(%d)"), iPenInfo.pointerInfo.ptHimetricLocationRaw.x, iPenInfo.pointerInfo.ptHimetricLocationRaw.y);
    UE_LOG(LogTemp, Warning, TEXT("SubPixel Location : x(%.2f) y(%.2f)"), subX, subY);
    UE_LOG(LogTemp, Warning, TEXT("Device Rect : left(%d) top(%d) right(%d) bottom(%d)"), deviceRect.left, deviceRect.top, deviceRect.right, deviceRect.bottom);
    UE_LOG(LogTemp, Warning, TEXT("Display Rect : left(%d) top(%d) right(%d) bottom(%d)"), displayRect.left, displayRect.top, displayRect.right, displayRect.bottom);
    UE_LOG(LogTemp, Warning, TEXT("Time : %d"), iPenInfo.pointerInfo.dwTime);
    UE_LOG(LogTemp, Warning, TEXT("History Count : %d"), iPenInfo.pointerInfo.historyCount);
    UE_LOG(LogTemp, Warning, TEXT("Input Data : %d"), iPenInfo.pointerInfo.InputData);
    UE_LOG(LogTemp, Warning, TEXT("Key States : %d"), iPenInfo.pointerInfo.dwKeyStates);
    UE_LOG(LogTemp, Warning, TEXT("Performance Count : %d"), iPenInfo.pointerInfo.PerformanceCount);
    UE_LOG(LogTemp, Warning, TEXT("ButtonChangeType : %s"), *buttonChangeType);
}

void
FNativeStylusInputDevice::Tick()
{
    PreviousState = CurrentState;
    CurrentState.Empty();

    CurrentState = mNativeState;
    mNativeState.Empty();
}

void
FNativeStylusInputDevice::SetPenMask(const PEN_MASK& iMask)
{
    SupportedInputs.Empty();

    bool hasPressure = iMask & PEN_MASK_PRESSURE;
    bool hasRotation = iMask & PEN_MASK_ROTATION;
    bool hasTiltX = iMask & PEN_MASK_TILT_X;
    bool hasTiltY = iMask & PEN_MASK_TILT_Y;

    SupportedInputs.Add(EStylusInputType::Position);
    if (hasPressure)
        SupportedInputs.Add(EStylusInputType::Pressure);

    if (hasTiltX && hasTiltY)
    {
        SupportedInputs.Add(EStylusInputType::Tilt);
    }

    /* if (hasPressure && hasTiltX && hasTiltY)
        SupportedInputs.Add(EStylusInputType::TangentPressure);    */

    if (hasRotation)
        SupportedInputs.Add(EStylusInputType::Twist);

    //SupportedInputs.Add(Z);    EStylusInputType::Z
    //SupportedInputs.Add(Timer);    EStylusInputType::Timer
    //SupportedInputs.Add(ButtonPressure);    EStylusInputType::ButtonPressure
    //SupportedInputs.Add(Size);    EStylusInputType::Size
}

void
FNativeStylusInputDevice::OnPointerUpdate(POINTER_PEN_INFO& iPenInfo)
{
    //UE_LOG(LogTemp, Warning, TEXT("=============OnPointerUpdate==============="));
    //LogPenInfo(iPenInfo);

    bool hasPressure = iPenInfo.penMask & PEN_MASK_PRESSURE;
    bool hasRotation = iPenInfo.penMask & PEN_MASK_ROTATION;
    bool hasTiltX = iPenInfo.penMask & PEN_MASK_TILT_X;
    bool hasTiltY = iPenInfo.penMask & PEN_MASK_TILT_Y;


    float subX, subY;
    GetSubpixelLocation(iPenInfo, &subX, &subY);
    FVector2D position(subX, subY);
    double tiltX = 0;
    double tiltY = 0;
    float pressure = 0;
    float twist = 0;


    if (hasPressure)
        pressure = iPenInfo.pressure / 1024.f;

    if (hasTiltX && hasTiltY)
    {
        tiltX = iPenInfo.tiltX;
        tiltY = iPenInfo.tiltY;
    }

    /* if (hasPressure && hasTiltX && hasTiltY)
        SupportedInputs.Add(EStylusInputType::TangentPressure);     */

    if (hasRotation)
        twist = iPenInfo.rotation;

    bool isInverted = iPenInfo.penFlags & PEN_FLAG_INVERTED;

    FStylusState state
    (
        position,
        0,
        0,
        FVector2D(tiltX, tiltY),
        twist,
        pressure,
        0, //float InTanPressure,
        FVector2D(0), //FVector2D InSize,
        pressure > 0,
        isInverted
    );

    Dirty = true;
    mNativeState.Add(state);
}

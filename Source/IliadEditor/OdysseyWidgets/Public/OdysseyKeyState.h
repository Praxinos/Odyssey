// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"
#include "Framework/Application/IInputProcessor.h"

class ODYSSEYWIDGETS_API FOdysseyKeyState
    : public IInputProcessor
{
public:
    static TSharedPtr<FOdysseyKeyState> Get();
    static const FKey& GetLastKey();

private:
    //needed to compile against IInputProcessor
    virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override;

    /** Key down input */
    virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override;

    /** Key up input */
    virtual bool HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override;

private:
    FKey mLastKey;
};

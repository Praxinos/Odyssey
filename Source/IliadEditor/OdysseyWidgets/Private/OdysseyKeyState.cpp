// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyKeyState.h"

const FKey&
FOdysseyKeyState::GetLastKey()
{
    return Get()->mLastKey;
}

TSharedPtr<FOdysseyKeyState>
FOdysseyKeyState::Get()
{
    static TSharedPtr<FOdysseyKeyState> keyState = nullptr;
    if (!keyState)
    {
        keyState = MakeShareable(new FOdysseyKeyState());
        FSlateApplication::Get().RegisterInputPreProcessor(keyState);
    }
    return keyState;
}

void
FOdysseyKeyState::Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor)
{
}

bool
FOdysseyKeyState::HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
    if (InKeyEvent.GetKey().IsModifierKey())
        return false;

    mLastKey = InKeyEvent.GetKey();
    return false; //false means Unreal will continue as if we did nothing
}

/** Key up input */
bool
FOdysseyKeyState::HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
    if (InKeyEvent.GetKey() == mLastKey)
    {
        mLastKey = FKey();
    }
    return false; //false means Unreal will continue as if we did nothing
}

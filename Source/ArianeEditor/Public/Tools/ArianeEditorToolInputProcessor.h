// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Framework/Application/IInputProcessor.h"

class FArianeEditor;
class UArianeEditorTool;

class ARIANEEDITOR_API FArianeEditorToolInputProcessor : public IInputProcessor
{
    public:
        // Destructor
        ~FArianeEditorToolInputProcessor();
        //Constructor
        FArianeEditorToolInputProcessor( UArianeEditorTool* InTool );

    public:
        // IInputProcessor Interface overrides
        //needed to compile against IInputProcessor
        virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override;
        /** Key down input */
        virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override;
        /** Key up input */
        virtual bool HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override;

    protected:
        UArianeEditorTool* Tool;
};

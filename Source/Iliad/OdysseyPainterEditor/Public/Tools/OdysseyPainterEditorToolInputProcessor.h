// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"
#include "Framework/Application/IInputProcessor.h"

class FOdysseyPainterEditor;
class UOdysseyPainterEditorTool;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorToolInputProcessor : public IInputProcessor
{
    public:
        // Destructor
        ~FOdysseyPainterEditorToolInputProcessor();
        //Constructor
        FOdysseyPainterEditorToolInputProcessor( UOdysseyPainterEditorTool* iPainterEditorTool );

    public:
        // IInputProcessor Interface overrides
        //needed to compile against IInputProcessor
        virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override;
        /** Key down input */
        virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override;
        /** Key up input */
        virtual bool HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override;

    protected:
        UOdysseyPainterEditorTool* mPainterEditorTool;
};

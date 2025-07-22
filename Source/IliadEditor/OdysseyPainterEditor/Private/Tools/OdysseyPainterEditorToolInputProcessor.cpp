// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorToolInputProcessor.h"
#include "OdysseyPainterEditorTool.h"
#include "OdysseyPainterEditor.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorToolInputProcessor::~FOdysseyPainterEditorToolInputProcessor()
{

}

FOdysseyPainterEditorToolInputProcessor::FOdysseyPainterEditorToolInputProcessor( UOdysseyPainterEditorTool* iPainterEditorTool )
    : mPainterEditorTool (iPainterEditorTool)
{

}

void
FOdysseyPainterEditorToolInputProcessor::Tick( const float DeltaTime
                                             , FSlateApplication& SlateApp
                                             , TSharedRef<ICursor> Cursor )
{
}

bool
FOdysseyPainterEditorToolInputProcessor::HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
    //false means Unreal will continue as if we did nothing

    // note: we pass the whole key event (and not only the key itself)
    // in order to be able to query the Repeat status and other methods
    return mPainterEditorTool->OnKeyDownGlobal( InKeyEvent );
}

bool
FOdysseyPainterEditorToolInputProcessor::HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
    //false means Unreal will continue as if we did nothing

    return mPainterEditorTool->OnKeyUpGlobal( InKeyEvent );
}

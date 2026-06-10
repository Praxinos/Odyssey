// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "ArianeEditorToolInputProcessor.h"
#include "ArianeEditorTool.h"
#include "ArianeEditor.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FArianeEditorToolInputProcessor::~FArianeEditorToolInputProcessor()
{

}

FArianeEditorToolInputProcessor::FArianeEditorToolInputProcessor( UArianeEditorTool* InTool )
    : Tool (InTool)
{

}

void
FArianeEditorToolInputProcessor::Tick( const float DeltaTime
                                     , FSlateApplication& SlateApp
                                     , TSharedRef<ICursor> Cursor )
{
}

bool
FArianeEditorToolInputProcessor::HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
    //false means Unreal will continue as if we did nothing

    // note: we pass the whole key event (and not only the key itself)
    // in order to be able to query the Repeat status and other methods
    return Tool->ProcessKeyDownGlobal( InKeyEvent );
}

bool
FArianeEditorToolInputProcessor::HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
    //false means Unreal will continue as if we did nothing

    return Tool->ProcessKeyUpGlobal( InKeyEvent );
}

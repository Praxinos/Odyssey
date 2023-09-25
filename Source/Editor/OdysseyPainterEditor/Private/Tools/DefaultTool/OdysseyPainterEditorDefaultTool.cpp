// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/DefaultTool/OdysseyPainterEditorDefaultTool.h"

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorDefaultTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorDefaultTool::~UOdysseyPainterEditorDefaultTool()
{
}

UOdysseyPainterEditorDefaultTool::UOdysseyPainterEditorDefaultTool()
{
}

//static
bool
UOdysseyPainterEditorDefaultTool::DoubleClicked()
{
    uint64 clickTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    static uint64 previousClickTime = 0;
    bool doubleClicked = ( ( clickTime - previousClickTime ) < 200 ) ? true : false;

    previousClickTime = clickTime;

    return doubleClicked;
}

bool
UOdysseyPainterEditorDefaultTool::OnKeyDown( const FKey& iKey )
{
    bool ret = false;

    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (hasVector)
    {
        //Should be done in OnKeyDownVector directly
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
        if (mediaVectors.Num() > 0)
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
            ret = UOdysseyPainterEditorDefaultTool::OnKeyDownVector( vectorScene, iKey );
        }
    }

    return ret;
}

bool
UOdysseyPainterEditorDefaultTool::OnKeyDownVector( FOdysseyVectorScene* iScene
                                                 , const FKey& iKey )
{
    if( FSlateApplication::Get().GetModifierKeys().IsControlDown() )
    {
        if( iKey == EKeys::C )
        {
            FOdysseyPainterEditor::CopyObjectSelection( iScene->GetEngine(), iScene );
        }

        if( iKey == EKeys::V )
        {
            FOdysseyPainterEditor::PasteObjectSelection( iScene->GetEngine(), iScene );
        }

        if( iKey == EKeys::A )
        {
            GetEditor()->SelectAll( iScene->GetEngine(), iScene );
        }
    }

    if( iKey == EKeys::Delete )
    {
        switch( GetEditor()->GetVectorEditionMode() )
        {
            case eVectorEditionMode::Object :
                GetEditor()->DeleteObjectSelection( iScene->GetEngine(), iScene );
            break;

            case eVectorEditionMode::Vertex :
                GetEditor()->DeletePointSelection( iScene->GetEngine(), iScene );
            break;

            default:
            break;
        }
    }

    return false;
}

bool
UOdysseyPainterEditorDefaultTool::OnKeyUp( const FKey& iKey )
{
    bool ret = false;
    
    
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (hasVector)
    {
        //Should be done in OnKeyUpVector directly
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
        if (mediaVectors.Num() > 0)
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
            ret = UOdysseyPainterEditorDefaultTool::OnKeyUpVector( vectorScene, iKey );
        }
    }

    return ret;
}

bool
UOdysseyPainterEditorDefaultTool::OnKeyUpVector( FOdysseyVectorScene* iScene
                                               , const FKey& iKey )
{
    return false;
}

#undef LOCTEXT_NAMESPACE

// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorObjectPickTool/OdysseyPainterEditorVectorObjectPickTool.h"
#include <chrono>

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorVectorObjectPickTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorObjectPickTool::~UOdysseyPainterEditorVectorObjectPickTool()
{
    delete mSelectionHUD;
}

UOdysseyPainterEditorVectorObjectPickTool::UOdysseyPainterEditorVectorObjectPickTool()
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Lasso64");

    mSelectionHUD = new FOdysseyVectorHUDSelection( );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorObjectPickTool::ActivateVector( FOdysseyVectorEngine* iEngine
                                                         , FOdysseyVectorScene* iScene
                                                         , int32 iSizeX
                                                         , int32 iSizeY )
{
    mSelectionHUD->Init( iSizeX, iSizeY );
    mSelectionHUD->UpdateSelectionBox( iScene );

    iEngine->ClearHUD( );
    iEngine->AddHUD( mSelectionHUD );

    iScene->Update( 0 ); // refresh vector scene and GUI widgets via delegates.
}

bool
UOdysseyPainterEditorVectorObjectPickTool::OnKeyDownVector( FOdysseyVectorEngine* iEngine
                                                          , FOdysseyVectorScene* iScene
                                                          , const FKey& iKey )
{
    // Note: this also calls iScene->Update(0)
    UOdysseyPainterEditorDefaultTool::OnKeyDownVector( iEngine, iScene, iKey );

    return false;
}

bool
UOdysseyPainterEditorVectorObjectPickTool::OnKeyUpVector( FOdysseyVectorEngine* iEngine
                                                        , FOdysseyVectorScene* iScene
                                                        , const FKey& iKey )
{
    // Note: this also calls iScene->Update(0)
    UOdysseyPainterEditorDefaultTool::OnKeyUpVector( iEngine, iScene, iKey );

    return false;
}

bool
UOdysseyPainterEditorVectorObjectPickTool::OnMouseDownVector( FOdysseyVectorEngine* iEngine
                                                            , FOdysseyVectorScene* iScene
                                                            , const FOdysseyPoint& iPointInTexture
                                                            , const FKey& iKey )
{
    ::ULIS::FVec2D point = { iPointInTexture.x, iPointInTexture.y };

    mPointArray.clear();

    mSelectionHUD->SetSelecting( true, &mPointArray );

    mPointArray.push_back( point );

    iScene->Update( 0 ); // refresh vector scene and GUI widgets via delegates.

    return true;
}

void
UOdysseyPainterEditorVectorObjectPickTool::OnMouseDragVector( FOdysseyVectorEngine* iEngine
                                                            , FOdysseyVectorScene* iScene
                                                            , const FOdysseyPoint& iPointInTexture )
{
    ::ULIS::FVec2D point = { iPointInTexture.x, iPointInTexture.y };

    mPointArray.push_back( point );

    iScene->Update( FOdysseyVectorObject::FREQUENTUPDATES ); // refresh vector scene and GUI widgets via delegates.
}

//static
bool
UOdysseyPainterEditorVectorObjectPickTool::DoubleClicked()
{
    uint64 clickTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    static uint64 previousClickTime = 0;
    bool doubleClicked = ( ( clickTime - previousClickTime ) < 200 ) ? true : false;

    previousClickTime = clickTime;

    return doubleClicked;
}

static void
SetSelectionSpace( FOdysseyVectorEngine* iVectorEngine, FOdysseyVectorObject* iSelectedObject )
{
    if( UOdysseyPainterEditorVectorObjectPickTool::DoubleClicked() == true )
    {
        FOdysseyVectorGroup* selectedGroup = nullptr;

        if( iSelectedObject )
        {
            if( iSelectedObject->HasBaseClass( FOdysseyVectorGroup::StaticClass() ) )
            {
                selectedGroup = static_cast<FOdysseyVectorGroup*>(iSelectedObject);
            }
        }

        iVectorEngine->SetSelectionSpace( selectedGroup );
    }
}

bool
UOdysseyPainterEditorVectorObjectPickTool::OnMouseUpVector( FOdysseyVectorEngine* iEngine
                                                          , FOdysseyVectorScene* iScene
                                                          , const FOdysseyPoint& iPointInTexture
                                                          , const FKey& iKey )
{
    FOdysseyVectorUndoSelect* undoSelect = nullptr;

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("VectorObjectPickTool","Vector Object Pick Tool"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoSelect( iScene );

        GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();

    if ( mPointArray.size() == 1 )
    {
        iEngine->Pick( iScene, mPointArray, FOdysseyVectorObject::PICK_MATH_BASED );
    }

    if ( mPointArray.size() > 1 )
    {
        iEngine->Pick( iScene, mPointArray, FOdysseyVectorObject::PICK_MASK_BASED );
    }

    SetSelectionSpace( iEngine, iScene->GetLastSelected() );

    mSelectionHUD->SetSelecting( false, nullptr );
    mSelectionHUD->UpdateSelectionBox( iScene );

    iScene->Update( 0 ); // refresh vector scene and GUI widgets via delegates.

    return true;
}

void
UOdysseyPainterEditorVectorObjectPickTool::Commit()
{

}

#undef LOCTEXT_NAMESPACE

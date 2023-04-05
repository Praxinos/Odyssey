// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorObjectPickTool/OdysseyPainterEditorVectorObjectPickTool.h"
#include <chrono>

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
UOdysseyPainterEditorVectorObjectPickTool::Activate( FOdysseyVectorEngine* iEngine
                                                   , FOdysseyVectorScene* iScene
                                                   , int32 iSizeX
                                                   , int32 iSizeY )
{
    mSelectionHUD->Init( iSizeX, iSizeY );
    mSelectionHUD->UpdateSelectionBox( iScene );

    iEngine->ClearHUD( );
    iEngine->AddHUD( mSelectionHUD );
}

bool
UOdysseyPainterEditorVectorObjectPickTool::OnMouseDown( FOdysseyVectorEngine* iEngine
                                                      , FOdysseyVectorScene* iScene
                                                      , const FOdysseyPoint& iPointInTexture
                                                      , const FKey& iKey )
{
    ::ULIS::FVec2D point = { iPointInTexture.x, iPointInTexture.y };

    mPointArray.clear();

    mSelectionHUD->SetSelecting( true, &mPointArray );

    mPointArray.push_back( point );

    return true;
}

void
UOdysseyPainterEditorVectorObjectPickTool::OnMouseDrag( FOdysseyVectorEngine* iEngine
                                                      , FOdysseyVectorScene* iScene
                                                      , const FOdysseyPoint& iPointInTexture )
{
    ::ULIS::FVec2D point = { iPointInTexture.x, iPointInTexture.y };

    mPointArray.push_back( point );
}

static bool
DoubleClicked()
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
    if( DoubleClicked() == true )
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
UOdysseyPainterEditorVectorObjectPickTool::OnMouseUp( FOdysseyVectorEngine* iEngine
                                                    , FOdysseyVectorScene* iScene
                                                    , FOdysseyVectorUndo** iUndo
                                                    , const FOdysseyPoint& iPointInTexture
                                                    , const FKey& iKey )
{
    FOdysseyVectorUndoSelect* undoSelect = nullptr;

    // BeginTransaction() must be called for GUndo to have a value. Please do it in the caller function.
    if( iUndo && GUndo )
    {
        undoSelect = new FOdysseyVectorUndoSelect( iScene );

        (*iUndo) = undoSelect;

        GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(*iUndo) );
    }

    if( undoSelect )
        undoSelect->RecordBefore();

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

    mSelectionChanged.Broadcast(iScene);

    if( undoSelect )
        undoSelect->RecordAfter();

    return true;
}

void
UOdysseyPainterEditorVectorObjectPickTool::Commit()
{

}

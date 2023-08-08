// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/DefaultTool/OdysseyPainterEditorDefaultTool.h"
#include "Undo/OdysseyVectorUndoObjectAdd.h"

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorDefaultTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorDefaultTool::~UOdysseyPainterEditorDefaultTool()
{
}

UOdysseyPainterEditorDefaultTool::UOdysseyPainterEditorDefaultTool()
{
}

std::list<FOdysseyVectorObject*>&
UOdysseyPainterEditorDefaultTool::GetCopiedObjectList()
{
    static std::list<FOdysseyVectorObject*> copiedObjectList;

    return copiedObjectList;
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

void
UOdysseyPainterEditorDefaultTool::Copy( FOdysseyVectorEngine* iEngine
                                      , FOdysseyVectorScene* iScene )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetSelectedObjectList();

    if( selectedObjectList.size() )
    {
        // First step : clear previously copied objects
        GetCopiedObjectList().remove_if( []( FOdysseyVectorObject* iCopiedObject ){ delete iCopiedObject; return true; } );

        // second step : copy selection.
        for( std::list<FOdysseyVectorObject*>::iterator it = selectedObjectList.begin(); it != selectedObjectList.end(); ++it )
        {
            FOdysseyVectorObject* selectedObject = (*it);

            if( selectedObject->HasSelectedAncestor() == false )
            {
                GetCopiedObjectList().push_back( selectedObject->Copy() );
            }
        }
    }
}

void
UOdysseyPainterEditorDefaultTool::Paste( FOdysseyVectorEngine* iEngine
                                       , FOdysseyVectorScene* iScene )
{
    std::list<FOdysseyVectorObject*> pastedObjectList;

    // First copy all objects. This is needed to record their state-before-addition for the UNDO operation.
    for( std::list<FOdysseyVectorObject*>::iterator it = GetCopiedObjectList().begin(); it != GetCopiedObjectList().end(); ++it )
    {
        FOdysseyVectorObject* copiedObject = (*it);

        pastedObjectList.push_back( copiedObject->Copy() );
    }

    iScene->ClearSelection();

    // This undo must be set before association with the new parent object
    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("DefaultTool","Paste"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = static_cast<FOdysseyVectorUndo*>( new FOdysseyVectorUndoObjectAdd( iScene, pastedObjectList ) );

        GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();

    for( std::list<FOdysseyVectorObject*>::iterator it = pastedObjectList.begin(); it != pastedObjectList.end(); ++it )
    {
        FOdysseyVectorObject* pastedObject = (*it);
        //BLPoint shifting;

        iScene->AppendChild( pastedObject );

        //shifting = iScene->GetInverseWorldMatrix().mapVector( 10.0f, 10.0f ); // shift object by 10 pixels

        pastedObject->Invalidate();
        //pastedObject->Translate( newObject->GetTranslationX() + shifting.x, newObject->GetTranslationY() + shifting.y );
        pastedObject->UpdateMatrix();

        iScene->Select( pastedObject );
    }

    iScene->Update( 0 );

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                   | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED
                   | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED
                   | FOdysseyVectorEngine::SIGNAL_OBJECT_TRANSFORMED );
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
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
            ret = UOdysseyPainterEditorDefaultTool::OnKeyDownVector( vectorEngine, vectorScene, iKey );
        }
    }

    return ret;
}

bool
UOdysseyPainterEditorDefaultTool::OnKeyDownVector( FOdysseyVectorEngine* iEngine
                                                 , FOdysseyVectorScene* iScene
                                                 , const FKey& iKey )
{
    if( FSlateApplication::Get().GetModifierKeys().IsControlDown() )
    {
        if( iKey == EKeys::C )
        {
            Copy( iEngine, iScene );
        }

        if( iKey == EKeys::V )
        {
            Paste( iEngine, iScene );
        }

        if( iKey == EKeys::A )
        {
            GetEditor()->SelectAll( iEngine, iScene );
        }
    }

    if( iKey == EKeys::Delete )
    {
        switch( GetEditor()->GetVectorEditionMode() )
        {
            case eVectorEditionMode::Object :
                GetEditor()->DeleteObjectSelection( iEngine, iScene );
            break;

            case eVectorEditionMode::Vertex :
                GetEditor()->DeletePointSelection( iEngine, iScene );
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
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
            ret = UOdysseyPainterEditorDefaultTool::OnKeyUpVector( vectorEngine, vectorScene, iKey );
        }
    }

    return ret;
}

bool
UOdysseyPainterEditorDefaultTool::OnKeyUpVector( FOdysseyVectorEngine* iEngine
                                               , FOdysseyVectorScene* iScene
                                               , const FKey& iKey )
{
    return false;
}

#undef LOCTEXT_NAMESPACE

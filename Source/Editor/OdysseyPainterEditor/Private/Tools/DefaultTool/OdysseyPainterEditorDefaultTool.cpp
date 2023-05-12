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

void
UOdysseyPainterEditorDefaultTool::Copy( FOdysseyVectorEngine* iEngine
                                      , FOdysseyVectorScene* iScene )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetSelectedObjectList();

    if( selectedObjectList.size() )
    {
        // First step : clear previously copied objects
        mCopiedObjectList.remove_if( []( FOdysseyVectorObject* iCopiedObject ){ delete iCopiedObject; return true; } );

        // second step : copy selection.
        for( std::list<FOdysseyVectorObject*>::iterator it = selectedObjectList.begin(); it != selectedObjectList.end(); ++it )
        {
            FOdysseyVectorObject* selectedObject = (*it);

            if( selectedObject->HasSelectedAncestor() == false )
            {
                mCopiedObjectList.push_back( selectedObject->Copy() );
            }
        }
    }
}

void
UOdysseyPainterEditorDefaultTool::Paste( FOdysseyVectorEngine* iEngine
                                       , FOdysseyVectorScene* iScene )
{
    iScene->ClearSelection();

    for( std::list<FOdysseyVectorObject*>::iterator it = mCopiedObjectList.begin(); it != mCopiedObjectList.end(); ++it )
    {
        FOdysseyVectorObject* copiedObject = (*it);
        FOdysseyVectorObject* newObject = copiedObject->Copy();
        BLPoint shifting;

        iScene->AppendChild( newObject );

        shifting = iScene->GetInverseWorldMatrix().mapVector( 10.0f, 10.0f ); // shift object by 10 pixels

        newObject->Invalidate();
        newObject->Translate( newObject->GetTranslationX() + shifting.x, newObject->GetTranslationY() + shifting.y );
        newObject->UpdateMatrix();

        iScene->Select( newObject );
    }

    iScene->Update( 0 );
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
    }

    iScene->Update( 0 ); // refresh vector scene and GUI widgets via delegates.

    return false;
}

bool
UOdysseyPainterEditorDefaultTool::OnKeyUpVector( FOdysseyVectorEngine* iEngine
                                               , FOdysseyVectorScene* iScene
                                               , const FKey& iKey )
{
    iScene->Update( 0 ); // refresh vector scene and GUI widgets via delegates.

    return false;
}

#undef LOCTEXT_NAMESPACE

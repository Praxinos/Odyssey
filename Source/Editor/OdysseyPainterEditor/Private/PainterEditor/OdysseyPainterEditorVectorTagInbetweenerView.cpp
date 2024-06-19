#include "OdysseyPainterEditorVectorTagInbetweenerView.h"
#include "Undo/OdysseyVectorUndoTagInbetweenerParam.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorSource.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

UOdysseyPainterEditorVectorTagInbetweenerView::~UOdysseyPainterEditorVectorTagInbetweenerView()
{
}

UOdysseyPainterEditorVectorTagInbetweenerView::UOdysseyPainterEditorVectorTagInbetweenerView()
    : mEditor( nullptr )
    , mScene( nullptr )
{
}

UOdysseyPainterEditorVectorTagInbetweenerView::UOdysseyPainterEditorVectorTagInbetweenerView( FOdysseyPainterEditor* iEditor
                                                                                            , FOdysseyVectorGroupPaint* iScene )
{
    std::list<FOdysseyVectorTagInbetweener*> emptyList;

    Update( iEditor, iScene, emptyList );
}

void
UOdysseyPainterEditorVectorTagInbetweenerView::ImportParam()
{
    if( mSelectedInbetweenerTagArray.size() )
    {
        FOdysseyVectorTagInbetweener* selectedInbetweenerTag = mSelectedInbetweenerTagArray[0];

        InterpolationType = selectedInbetweenerTag->GetInterpolationType();
        GridType = selectedInbetweenerTag->GetGridType();
        DivisionX = selectedInbetweenerTag->GetGridNumQuadX();
        DivisionY = selectedInbetweenerTag->GetGridNumQuadY();
        InbetweenCount = selectedInbetweenerTag->GetInbetweenCount();

        if( GridType == eInbetweenerGridType::ARAP )
        {
            FInbetweenerGridARAP* arapGrid = static_cast<FInbetweenerGridARAP*>(selectedInbetweenerTag->GetGrid());

            Rigidity = arapGrid->GetRigidity();
        }
    }
}

void 
UOdysseyPainterEditorVectorTagInbetweenerView::Update( FOdysseyPainterEditor* iEditor
                                                     , FOdysseyVectorGroupPaint* iScene
                                                     , const std::list<FOdysseyVectorTagInbetweener*>& iSelectedInbetweenerTagList )
{
    mEditor = iEditor;
    mScene = iScene;

    mSelectedInbetweenerTagArray.clear();
    mSelectedInbetweenerTagArray.reserve( iSelectedInbetweenerTagList.size() );

    for( FOdysseyVectorTagInbetweener* selectedInbetweenerTag : iSelectedInbetweenerTagList )
    {
        mSelectedInbetweenerTagArray.push_back( selectedInbetweenerTag );
    }

    ImportParam();
}

void
UOdysseyPainterEditorVectorTagInbetweenerView::PropertyChanged( const FName& iPropertyName
                                                              , const FName& iMemberPropertyName
                                                              , const FName& iCategory)
{
    for( FOdysseyVectorTagInbetweener* selectedInbetweenerTag : mSelectedInbetweenerTagArray )
    {
        //////////
        if( iPropertyName == "InbetweenCount" )
            selectedInbetweenerTag->SetInbetweenCount( InbetweenCount );

        if( iPropertyName == "InterpolationType" )
            selectedInbetweenerTag->SetInterpolationType( InterpolationType );

        if( iPropertyName == "DivisionX" )
            selectedInbetweenerTag->SetGridNumQuadX( DivisionX );

        if( iPropertyName == "DivisionY" )
            selectedInbetweenerTag->SetGridNumQuadY( DivisionY );

        if( iPropertyName == "Rigidity" )
        {
            FInbetweenerGridARAP* arapGrid = static_cast<FInbetweenerGridARAP*>(selectedInbetweenerTag->GetGrid());

            arapGrid->SetRigidity( Rigidity );
        }

        // must be last to be able to update correctly grid type-dependent fields
        if( iPropertyName == "GridType" )
        {
            selectedInbetweenerTag->SetGridType( GridType );

            // updates grid type-dependent fields
            ImportParam();
        }
    }
}

void
UOdysseyPainterEditorVectorTagInbetweenerView::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive )
        return;

    if( mSelectedInbetweenerTagArray.size() )
    {
        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("vector-tag.transaction.property-changed","Property Changed"));
        if( GUndo )
        {
            FOdysseyVectorUndo *undo = new FOdysseyVectorUndoTagInbetweenerParam( mScene
                                                                                , mSelectedInbetweenerTagArray );

            // We use GEditor as the UObject, otherwise if we use "this", at each UNDO, PostEditChangeProperty() will be called
            // which will again call StoreUndo + this will lead to a crash. I don't know however what will be the consequences
            // of a call to GEditor::PostEditChangeProperty()
            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
                
            TSharedPtr<FOdysseyPainterEditorSource> source = mEditor->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();

        PropertyChanged( PropertyChangedEvent.GetPropertyName()
                       , PropertyChangedEvent.MemberProperty->GetFName()
                       , FName(PropertyChangedEvent.Property->GetMetaData(TEXT("Category"))) );

        mScene->Update( 0 );
        // calls delegates
        mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
    }
}

#undef LOCTEXT_NAMESPACE

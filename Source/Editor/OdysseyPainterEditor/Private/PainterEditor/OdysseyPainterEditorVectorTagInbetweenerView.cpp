#include "OdysseyPainterEditorVectorTagInbetweenerView.h"
#include "Undo/OdysseyVectorUndoChartAlter.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorSource.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

UOdysseyPainterEditorVectorTagInbetweenerView::~UOdysseyPainterEditorVectorTagInbetweenerView()
{
}

UOdysseyPainterEditorVectorTagInbetweenerView::UOdysseyPainterEditorVectorTagInbetweenerView()
    : mEditor( nullptr )
    , mInbetweenerTag( nullptr )
{
}

UOdysseyPainterEditorVectorTagInbetweenerView::UOdysseyPainterEditorVectorTagInbetweenerView( FOdysseyPainterEditor* iEditor
                                                                                            , FOdysseyVectorTagInbetweener* iBucket )
{
    Update( iEditor, iBucket );
}

void
UOdysseyPainterEditorVectorTagInbetweenerView::ImportParam()
{
    InterpolationType = mInbetweenerTag->GetInterpolationType();
    GridType = mInbetweenerTag->GetGridType();
    DivisionX = mInbetweenerTag->GetGridNumQuadX();
    DivisionY  = mInbetweenerTag->GetGridNumQuadY();
    InbetweenCount = mInbetweenerTag->GetInbetweenCount();

    if( GridType == eInbetweenerGridType::ARAP )
    {
        FInbetweenerGridARAP* arapGrid = static_cast<FInbetweenerGridARAP*>(mInbetweenerTag->GetGrid());

        arapGrid->SetRigidity( Rigidity );
    }
}

void 
UOdysseyPainterEditorVectorTagInbetweenerView::Update( FOdysseyPainterEditor* iEditor
                                                     , FOdysseyVectorTagInbetweener* iInbetweenerTag )
{
    mEditor = iEditor;
    mInbetweenerTag = iInbetweenerTag;

    ImportParam();
}

void
UOdysseyPainterEditorVectorTagInbetweenerView::PropertyChanged( const FName& iPropertyName
                                                              , const FName& iMemberPropertyName
                                                              , const FName& iCategory)
{
    if( mInbetweenerTag )
    {
        FOdysseyVectorGroupPaint* vectorScene = mInbetweenerTag->GetOwner()->GetScene();

        //////////
        if( iPropertyName == "InbetweenCount" )
        {
            // needed for valid GUndo pointer
            GEditor->BeginTransaction(LOCTEXT("vector-tag.transaction.property-changed","Property Changed"));
            if( GUndo )
            {
                FOdysseyVectorUndo *undo = new FOdysseyVectorUndoChartAlter( vectorScene
                                                                           , mInbetweenerTag );
                // We use GEditor as the UObject, otherwise if we use "this", at each UNDO, PostEditChangeProperty() will be called
                // which will again call StoreUndo + this will lead to a crash. I don't know however what will be the consequences
                // of a call to GEditor::PostEditChangeProperty()
                GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
                
                TSharedPtr<FOdysseyPainterEditorSource> source = mEditor->GetSource();
                if (source)
                    source->RecordCurrentFrameUndo();
            }
            GEditor->EndTransaction();

            mInbetweenerTag->SetInbetweenCount( InbetweenCount );
        }

        if( iPropertyName == "InterpolationType" )
            mInbetweenerTag->SetInterpolationType( InterpolationType );

        if( iPropertyName == "DivisionX" )
            mInbetweenerTag->SetGridNumQuadX( DivisionX );

        if( iPropertyName == "DivisionY" )
            mInbetweenerTag->SetGridNumQuadY( DivisionY );

        if( iPropertyName == "Rigidity" )
        {
            FInbetweenerGridARAP* arapGrid = static_cast<FInbetweenerGridARAP*>(mInbetweenerTag->GetGrid());

            arapGrid->SetRigidity( Rigidity );
        }

        // must be last to be able to update correctly grid type-dependent fields
        if( iPropertyName == "GridType" )
        {
            mInbetweenerTag->SetGridType( GridType );

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

    if( mInbetweenerTag )
    {
        FOdysseyVectorGroupPaint* vectorScene = mInbetweenerTag->GetOwner()->GetScene();

        PropertyChanged( PropertyChangedEvent.GetPropertyName()
                       , PropertyChangedEvent.MemberProperty->GetFName()
                       , FName(PropertyChangedEvent.Property->GetMetaData(TEXT("Category"))) );

        vectorScene->Update( 0 );
        // calls delegates
        vectorScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
    }
}

#undef LOCTEXT_NAMESPACE

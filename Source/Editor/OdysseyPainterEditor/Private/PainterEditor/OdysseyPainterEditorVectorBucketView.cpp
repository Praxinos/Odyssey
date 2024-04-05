#include "OdysseyPainterEditorVectorBucketView.h"
#include "Undo/OdysseyVectorUndoBucketParam.h"
#include "OdysseyVectorEngine.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

UOdysseyPainterEditorVectorBucketView::~UOdysseyPainterEditorVectorBucketView()
{
}

UOdysseyPainterEditorVectorBucketView::UOdysseyPainterEditorVectorBucketView()
    : mBucket( nullptr )
{
}

UOdysseyPainterEditorVectorBucketView::UOdysseyPainterEditorVectorBucketView( FOdysseyVectorBucket* iBucket )
{
    Update( iBucket );
}

void
UOdysseyPainterEditorVectorBucketView::ImportParam()
{
    ColorMode = mBucket->GetColorMode();
    SpreadingPolicy = mBucket->GetSpreadingPolicy();
    SolidColor  = mBucket->GetSolidColor();
    Rotation = mBucket->GetRotation();
    Propagated = mBucket->IsPropagated();
    GradientColor0 = mBucket->GetGradientColor0();
    GradientColor1 = mBucket->GetGradientColor1();
    RadialRadius = mBucket->GetRadialRadius();
}

void 
UOdysseyPainterEditorVectorBucketView::Update( FOdysseyVectorBucket* iBucket )
{
    mBucket = iBucket;

    ImportParam();
}

void
UOdysseyPainterEditorVectorBucketView::PropertyChanged( const FName& iPropertyName
                                                      , const FName& iMemberPropertyName
                                                      , const FName& iCategory )
{
    if( mBucket )
    {
        if( iPropertyName == "ColorMode" )
            mBucket->SetColorMode( ColorMode );

        if( iPropertyName == "SpreadingPolicy" )
            mBucket->SetSpreadingPolicy( SpreadingPolicy );

        // note: iMemberPropertyName because FColor is a struct 
        // and we can edit individual struct members RGBA
        if( ( iPropertyName == "SolidColor" ) || ( iMemberPropertyName == "SolidColor" ) )
            mBucket->SetSolidColor( SolidColor );

        if( iPropertyName == "Rotation" )
            mBucket->SetRotation( Rotation );

        if( iPropertyName == "Propagated" )
            mBucket->SetPropagated( Propagated );

        // note: iMemberPropertyName because FColor is a struct 
        // and we can edit individual struct members RGBA
        if( ( iPropertyName == "GradientColor0" ) || ( iMemberPropertyName == "GradientColor0" ) )
            mBucket->SetGradientColor0( GradientColor0 );

        // note: iMemberPropertyName because FColor is a struct 
        // and we can edit individual struct members RGBA
        if( ( iPropertyName == "GradientColor1" ) || ( iMemberPropertyName == "GradientColor1" ) )
            mBucket->SetGradientColor1( GradientColor1 );

        if( iPropertyName == "RadialRadius" )
            mBucket->SetRadialRadius( RadialRadius );

        mBucket->Invalidate();
    }
}

void
UOdysseyPainterEditorVectorBucketView::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive )
        return;

    if( mBucket )
    {
        FOdysseyVectorGroupPaint* vectorScene = mBucket->GetOwner()->GetScene();

        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("vector-bucket.transaction.property-changed","Property Changed"));
        if( GUndo )
        {
            FOdysseyVectorUndo *undo = new FOdysseyVectorUndoBucketParam( vectorScene, mBucket );
            // We use GEditor as the UObject, otherwise if we use "this", at each UNDO, PostEditChangeProperty() will be called
            // which will again call StoreUndo + this will lead to a crash. I don't know however what will be the consequences
            // of a call to GEditor::PostEditChangeProperty()
            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
        }
        GEditor->EndTransaction();

        PropertyChanged( PropertyChangedEvent.GetPropertyName()
                       , PropertyChangedEvent.MemberProperty->GetFName()
                       , FName(PropertyChangedEvent.Property->GetMetaData(TEXT("Category"))) );

        vectorScene->Update( 0 );
        // calls delegates
        vectorScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
    }
}

#undef LOCTEXT_NAMESPACE

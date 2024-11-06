#include "OdysseyPainterEditorVectorBucketView.h"
#include "Undo/OdysseyVectorUndoBucketParam.h"
#include "OdysseyVectorEngine.h"

#include "OdysseyPainterEditor.h"
#include "PainterEditor/OdysseyPainterEditorSource.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

UOdysseyPainterEditorVectorBucketView::~UOdysseyPainterEditorVectorBucketView()
{
}

UOdysseyPainterEditorVectorBucketView::UOdysseyPainterEditorVectorBucketView()
    : mEditor( nullptr )
    , mBucket( nullptr )
{
}

UOdysseyPainterEditorVectorBucketView::UOdysseyPainterEditorVectorBucketView( FOdysseyPainterEditor* iEditor, FOdysseyVectorBucket* iBucket )
{
    Update( iEditor, iBucket );
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
UOdysseyPainterEditorVectorBucketView::Update( FOdysseyPainterEditor* iEditor, FOdysseyVectorBucket* iBucket )
{
    mEditor = iEditor;
    mBucket = iBucket;

    ImportParam();
}

void
UOdysseyPainterEditorVectorBucketView::PropertyChanged( const FName& iPropertyName
                                                      , const FName& iMemberPropertyName
                                                      , const FName& iCategory)
{
    if( mBucket )
    {
        if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorBucketView, ColorMode) )
            mBucket->SetColorMode( ColorMode );

        if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorBucketView, SpreadingPolicy) )
            mBucket->SetSpreadingPolicy( SpreadingPolicy );

        // note: iMemberPropertyName because FColor is a struct
        // and we can edit individual struct members RGBA
        if( ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorBucketView, SolidColor) ) || ( iMemberPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorBucketView, SolidColor) ) )
            mBucket->SetSolidColor( SolidColor );

        if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorBucketView, Rotation) )
            mBucket->SetRotation( Rotation );

        if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorBucketView, Propagated) )
            mBucket->SetPropagated( Propagated );

        // note: iMemberPropertyName because FColor is a struct
        // and we can edit individual struct members RGBA
        if( ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorBucketView, GradientColor0) ) || ( iMemberPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorBucketView, GradientColor0) ) )
            mBucket->SetGradientColor0( GradientColor0 );

        // note: iMemberPropertyName because FColor is a struct
        // and we can edit individual struct members RGBA
        if( ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorBucketView, GradientColor1) ) || ( iMemberPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorBucketView, GradientColor1) ) )
            mBucket->SetGradientColor1( GradientColor1 );

        if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorBucketView, RadialRadius) )
            mBucket->SetRadialRadius( RadialRadius );

        mBucket->Invalidate();
    }
}

void
UOdysseyPainterEditorVectorBucketView::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
    uint64 retFlags = 0;

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
            FOdysseyVectorUndo *undo = new FOdysseyVectorUndoBucketParam( vectorScene, mBucket, retFlags );
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

        vectorScene->Update( 0 );
        // request refresh
        vectorScene->GetEngine()->Invalidate( 0 );
    }
}

#undef LOCTEXT_NAMESPACE

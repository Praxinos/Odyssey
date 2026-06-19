// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorVectorBucketView.h"

#include "Editor.h"

#include "OdysseyLayerStack.h"
#include "OdysseyAnimationLayerImageVector.h"
#include "OdysseyTextureLayerImageVector.h"
#include "Undo/OdysseyVectorUndoBucketParam.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVectorLayer.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorSource.h"

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
    SolidColor  = mBucket->GetSolidColor();
    Propagated = mBucket->IsPropagated();
    GradientColor0 = mBucket->GetGradientColor0();
    GradientColor1 = mBucket->GetGradientColor1();
    RadialRadius = mBucket->GetRadialRadius();

    if (mBucket->GetPaletteEntry())
    {
        PaletteSelection.OdysseyPalette = mBucket->GetPaletteEntry()->GetPalette();
        PaletteSelection.OdysseyPaletteEntryColor = Cast<UOdysseyPaletteEntryColor>(mBucket->GetPaletteEntry());
    }
    else
    {
        PaletteSelection.OdysseyPalette = nullptr;
        PaletteSelection.OdysseyPaletteEntryColor = nullptr;
    }
}

TSharedPtr<FOdysseyVectorLayer>
UOdysseyPainterEditorVectorBucketView::GetVectorLayer()
{
    UOdysseyLayer* currentLayer = mEditor->LayerStack()->GetCurrentLayer();
    UOdysseyAnimationLayerImageVector* animationLayer = Cast<UOdysseyAnimationLayerImageVector>(currentLayer);
    UOdysseyTextureLayerImageVector* textureLayer = Cast<UOdysseyTextureLayerImageVector>(currentLayer);

    if( animationLayer )
    {
        return animationLayer->GetVectorLayer();
    }

    if( textureLayer )
    {
        return textureLayer->GetVectorLayer();
    }

    return nullptr;
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

        // note: iMemberPropertyName because FColor is a struct
        // and we can edit individual struct members RGBA
        if( ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorBucketView, SolidColor) ) || ( iMemberPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorBucketView, SolidColor) ) )
            mBucket->SetSolidColor( SolidColor );

        if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorBucketView, Propagated) )
            mBucket->SetPropagated( Propagated );

        // note: iMemberPropertyName because FColor is a struct
        // and we can edit individual struct members RGBA
        if( ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorBucketView, GradientColor0) ) || ( iMemberPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorBucketView, GradientColor0) ) )
            mBucket->SetGradientColor0( GradientColor0 );

        if ( (iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorBucketView, PaletteSelection)) || ( iMemberPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorBucketView, PaletteSelection) ) )
        {
            mBucket->SetPaletteEntry( PaletteSelection.OdysseyPaletteEntryColor );
        }

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

            TSharedPtr<FOdysseyPainterEditorSource> source = mEditor->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();

        PropertyChanged( PropertyChangedEvent.GetPropertyName()
                       , PropertyChangedEvent.MemberProperty->GetFName()
                       , FName(PropertyChangedEvent.Property->GetMetaData(TEXT("Category"))) );

        vectorScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
        vectorScene->GetLayer()->RequestRedraw( vectorScene->GetCell(), 0 );
    }
}

FOdysseyPainterEditor* UOdysseyPainterEditorVectorBucketView::GetEditor()
{
    return mEditor;
}

#undef LOCTEXT_NAMESPACE

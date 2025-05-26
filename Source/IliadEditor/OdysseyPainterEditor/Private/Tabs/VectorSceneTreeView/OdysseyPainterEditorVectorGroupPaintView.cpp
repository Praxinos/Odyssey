// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorVectorGroupPaintView.h"

UOdysseyPainterEditorVectorGroupPaintView::~UOdysseyPainterEditorVectorGroupPaintView()
{
}

UOdysseyPainterEditorVectorGroupPaintView::UOdysseyPainterEditorVectorGroupPaintView()
    : UOdysseyPainterEditorVectorGroupView()
    , Painted( true )
    , Monochrome( false )
    , MonochromeColor( 127, 127, 127, 255 )
    , GapTolerance( 12.0f )
    //, Wireframe
    , WireframeColor( 255, 255, 255, 255 )
    , IntersectsCanvas( false )
    , GapDetectionScheme(  eGapDetectionScheme::SegmentExtension )
    , SegmentExtensionScheme( eSegmentExtensionScheme::Single )
    , SegmentExtensionSimplified ( true )
    , Realtime( false )
    , Multithreaded( true )
{
    bDisplayBackgroundProperties = true;
}

void
UOdysseyPainterEditorVectorGroupPaintView::ImportParam()
{
    UOdysseyPainterEditorVectorGroupView::ImportParam();

    for( FOdysseyVectorObject* selectedObject : mFocusedObjectList )
    {
        if( selectedObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
        {
            FOdysseyVectorGroupPaint* selectedPaintGroup = static_cast<FOdysseyVectorGroupPaint*>(selectedObject);

            // Category "PaintGroup"
            Painted                    = selectedPaintGroup->IsPainted();
            Monochrome                 = selectedPaintGroup->IsMonochrome();
            MonochromeColor            = selectedPaintGroup->GetMonochromeColor();
            Realtime                   = selectedPaintGroup->IsRealtime();
            GapTolerance               = selectedPaintGroup->GetGapTolerance();
            //Wireframe                = selectedPaintGroup->IsWireframe();
            WireframeColor             = selectedPaintGroup->GetWireframeColor();
            Multithreaded              = selectedPaintGroup->IsMultithreaded();
            IntersectsCanvas           = selectedPaintGroup->IntersectsCanvas();
            GapDetectionScheme         = selectedPaintGroup->GetGapDetectionScheme();
            SegmentExtensionScheme     = selectedPaintGroup->GetSegmentExtensionScheme();
            SegmentExtensionSimplified = selectedPaintGroup->IsSegmentExtensionSimplified();

            break; // only one for now
        }
    }
}

void
UOdysseyPainterEditorVectorGroupPaintView::PropertyChanged( const FName& iPropertyName
                                                          , const FName& iMemberPropertyName
                                                          , const FName& iCategory)
{
    UOdysseyPainterEditorVectorGroupView::PropertyChanged( iPropertyName
                                                         , iMemberPropertyName
                                                         , iCategory );

    for( FOdysseyVectorObject* selectedObject : mFocusedObjectList )
    {
        if( selectedObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
        {
            FOdysseyVectorGroupPaint* selectedPaintGroup = static_cast<FOdysseyVectorGroupPaint*>(selectedObject);

            if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, Painted) )
                selectedPaintGroup->SetPainted( Painted );

            if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, Monochrome) )
                selectedPaintGroup->SetMonochrome( Monochrome );

            // Note: iMemberPropertyName because FColor is a struct
            // and we can edit individual struct members RGBA
            if( ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, MonochromeColor) ) || ( iMemberPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, MonochromeColor) ) )
                selectedPaintGroup->SetMonochromeColor( MonochromeColor );

            if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, Realtime) )
                selectedPaintGroup->SetRealtime( Realtime );

            if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, GapTolerance) )
                selectedPaintGroup->SetGapTolerance( GapTolerance );

            if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, IntersectsCanvas) )
                selectedPaintGroup->SetIntersectsCanvas( IntersectsCanvas );

            //if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, Wireframe) )
            //    selectedPaintGroup->SetWireframe( Wireframe );

            // Note: iMemberPropertyName because FColor is a struct
            // and we can edit individual struct members RGBA
            if( ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, WireframeColor) ) || ( iMemberPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, WireframeColor) ) )
                selectedPaintGroup->SetWireframeColor( WireframeColor );

            if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, Multithreaded) )
                selectedPaintGroup->SetMultithreaded( Multithreaded );

            if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, GapDetectionScheme) )
                selectedPaintGroup->SetGapDetectionScheme( GapDetectionScheme );

            if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, SegmentExtensionScheme) )
                selectedPaintGroup->SetSegmentExtensionScheme( SegmentExtensionScheme );

            if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, SegmentExtensionSimplified) )
                selectedPaintGroup->SetSegmentExtensionSimplified( SegmentExtensionSimplified );
        }
    }
}

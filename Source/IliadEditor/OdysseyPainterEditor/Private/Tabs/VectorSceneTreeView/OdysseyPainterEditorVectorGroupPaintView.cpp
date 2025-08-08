// IDDN.FR.001.060015.014.S.X.2019.000.00000
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
    bDisplayForegroundProperties = false;
}

void
UOdysseyPainterEditorVectorGroupPaintView::ImportParamFromOtherView( UOdysseyPainterEditorVectorObjectView* iOtherView )
{
    UOdysseyPainterEditorVectorGroupPaintView* otherGroupPaintView = Cast<UOdysseyPainterEditorVectorGroupPaintView>(iOtherView);

    if( otherGroupPaintView )
    {
        mGroupPaintPropertyBits = otherGroupPaintView->mGroupPaintPropertyBits;

        Painted                    = otherGroupPaintView->Painted;
        Monochrome                 = otherGroupPaintView->Monochrome;
        MonochromeColor            = otherGroupPaintView->MonochromeColor;
        Realtime                   = otherGroupPaintView->Realtime;
        GapTolerance               = otherGroupPaintView->GapTolerance;
        WireframeColor             = otherGroupPaintView->WireframeColor;
        Multithreaded              = otherGroupPaintView->Multithreaded;
        IntersectsCanvas           = otherGroupPaintView->IntersectsCanvas;
        GapDetectionScheme         = otherGroupPaintView->GapDetectionScheme;
        SegmentExtensionScheme     = otherGroupPaintView->SegmentExtensionScheme;
        SegmentExtensionSimplified = otherGroupPaintView->SegmentExtensionSimplified;
    }

    UOdysseyPainterEditorVectorGroupView::ImportParamFromOtherView( iOtherView );
}

void
UOdysseyPainterEditorVectorGroupPaintView::ImportParam( const std::list<FOdysseyVectorObject*>& iFocusedObjectList )
{
    UOdysseyPainterEditorVectorGroupView::ImportParam( iFocusedObjectList );

    for( FOdysseyVectorObject* selectedObject : iFocusedObjectList )
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
UOdysseyPainterEditorVectorGroupPaintView::ClearPropertyBits()
{
    UOdysseyPainterEditorVectorObjectView::ClearPropertyBits();

    memset( &mGroupPaintPropertyBits, 0, sizeof( mGroupPaintPropertyBits ) );
}

bool
UOdysseyPainterEditorVectorGroupPaintView::HasAnyPropertyBit()
{
    // we use a loop so that we don't forget any flags, even the ones that will be added later
    for( uint32 i = 0; i < sizeof( mGroupPaintPropertyBits ); i++  )
    {
        if( mGroupPaintPropertyBits.raw[i] )
        {
            return true;
        }
    }

    return UOdysseyPainterEditorVectorObjectView::HasAnyPropertyBit();
}

bool
UOdysseyPainterEditorVectorGroupPaintView::GetPropertyBit( const FName& iPropertyName )
{
    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, Painted) )
        return mGroupPaintPropertyBits.Painted;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, Monochrome) )
        return mGroupPaintPropertyBits.Monochrome;

    // Note: iMemberPropertyName because FColor is a struct
    // and we can edit individual struct members RGBA
    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, MonochromeColor) )
        return mGroupPaintPropertyBits.MonochromeColor;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, Realtime) )
        return mGroupPaintPropertyBits.Realtime;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, GapTolerance) )
        return mGroupPaintPropertyBits.GapTolerance;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, IntersectsCanvas) )
        return mGroupPaintPropertyBits.IntersectsCanvas;

    //if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, Wireframe) )
    //    selectedPaintGroup->SetWireframe( Wireframe );

    // Note: iMemberPropertyName because FColor is a struct
    // and we can edit individual struct members RGBA
    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, WireframeColor) )
        return mGroupPaintPropertyBits.WireframeColor;

//    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, Multithreaded) )
//        mGroupPaintPropertyBits.Multithreaded = iState;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, GapDetectionScheme) )
        return mGroupPaintPropertyBits.GapDetectionScheme;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, SegmentExtensionScheme) )
        return mGroupPaintPropertyBits.SegmentExtensionScheme;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, SegmentExtensionSimplified) )
        return mGroupPaintPropertyBits.SegmentExtensionSimplified;


    return UOdysseyPainterEditorVectorGroupView::GetPropertyBit( iPropertyName  );
}

void
UOdysseyPainterEditorVectorGroupPaintView::ApplyPropertyBits( FOdysseyVectorObject* iObject )
{
    UOdysseyPainterEditorVectorGroupView::ApplyPropertyBits( iObject );

    if( iObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
    {
        FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(iObject);

        if( mGroupPaintPropertyBits.Painted )
            paintGroup->SetPainted( Painted );

        if( mGroupPaintPropertyBits.Monochrome )
            paintGroup->SetMonochrome( Monochrome );

        // Note: iMemberPropertyName because FColor is a struct
        // and we can edit individual struct members RGBA
        if( mGroupPaintPropertyBits.MonochromeColor )
            paintGroup->SetMonochromeColor( MonochromeColor );

        if( mGroupPaintPropertyBits.Realtime )
            paintGroup->SetRealtime( Realtime );

        if( mGroupPaintPropertyBits.GapTolerance )
            paintGroup->SetGapTolerance( GapTolerance );

        if( mGroupPaintPropertyBits.IntersectsCanvas )
            paintGroup->SetIntersectsCanvas( IntersectsCanvas );

        //if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, Wireframe) )
        //    selectedPaintGroup->SetWireframe( Wireframe );

        // Note: iMemberPropertyName because FColor is a struct
        // and we can edit individual struct members RGBA
        if( mGroupPaintPropertyBits.WireframeColor )
            paintGroup->SetWireframeColor( WireframeColor );

//        if( mGroupPaintPropertyBits.Multithreaded )
//            paintGroup->SetMultithreaded( Multithreaded );

        if( mGroupPaintPropertyBits.GapDetectionScheme )
            paintGroup->SetGapDetectionScheme( GapDetectionScheme );

        if( mGroupPaintPropertyBits.SegmentExtensionScheme )
            paintGroup->SetSegmentExtensionScheme( SegmentExtensionScheme );

        if( mGroupPaintPropertyBits.SegmentExtensionSimplified )
            paintGroup->SetSegmentExtensionSimplified( SegmentExtensionSimplified );
    }
}

void
UOdysseyPainterEditorVectorGroupPaintView::SetPropertyBit( const FName& iPropertyName
                                                         , const FName& iMemberPropertyName
                                                         , const FName& iCategory
                                                         , bool iState )
{
    UOdysseyPainterEditorVectorGroupView::SetPropertyBit( iPropertyName
                                                        , iMemberPropertyName
                                                        , iCategory
                                                        , iState );

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, Painted) )
        mGroupPaintPropertyBits.Painted = iState;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, Monochrome) )
        mGroupPaintPropertyBits.Monochrome = iState;

    // Note: iMemberPropertyName because FColor is a struct
    // and we can edit individual struct members RGBA
    if( ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, MonochromeColor) )
     || ( iMemberPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, MonochromeColor) ) )
        mGroupPaintPropertyBits.MonochromeColor = iState;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, Realtime) )
        mGroupPaintPropertyBits.Realtime = iState;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, GapTolerance) )
        mGroupPaintPropertyBits.GapTolerance = iState;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, IntersectsCanvas) )
        mGroupPaintPropertyBits.IntersectsCanvas = iState;

    //if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, Wireframe) )
    //    selectedPaintGroup->SetWireframe( Wireframe );

    // Note: iMemberPropertyName because FColor is a struct
    // and we can edit individual struct members RGBA
    if( ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, WireframeColor) )
     || ( iMemberPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, WireframeColor) ) )
        mGroupPaintPropertyBits.WireframeColor = iState;

//    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, Multithreaded) )
//        mGroupPaintPropertyBits.Multithreaded = iState;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, GapDetectionScheme) )
        mGroupPaintPropertyBits.GapDetectionScheme = iState;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, SegmentExtensionScheme) )
        mGroupPaintPropertyBits.SegmentExtensionScheme = iState;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, SegmentExtensionSimplified) )
        mGroupPaintPropertyBits.SegmentExtensionSimplified = iState;
}

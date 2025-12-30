// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include <blend2d.h>
#include <ULIS>
#include "HUD/OdysseyVectorHUD.h"
#include "OdysseyHUDElement.h"
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseTool.h"

class FOdysseyVectorGroupPaint;
class FOdysseyVectorCycle;
class FOdysseyVectorObject;
class FOdysseyVectorPath;
class FOdysseyVectorSegment;
class FOdysseyVectorSegmentCubic;
class FOdysseyVectorSection;

typedef struct _FSelectionBox
{
    bool inited;
    bool aabb;
    ::ULIS::FRectD rect;
    BLMatrix2D worldMatrix;
    BLMatrix2D inverseWorldMatrix;
} FSelectionBox;

typedef struct _FPointQuadTreeEntry
{
    FOdysseyVectorPoint* point;
    ::ULIS::FVec2D worldCoords;

    _FPointQuadTreeEntry( FOdysseyVectorPoint* iPoint, ::ULIS::FVec2D iWorldCoords )
    {
        point = iPoint;
        worldCoords = iWorldCoords;
    }
} FPointQuadTreeEntry;

class ODYSSEYPAINTEREDITOR_API FPointQuadTree
{
    public:
       ~FPointQuadTree();
       FPointQuadTree( const ::ULIS::FRectD& iRect
                     , uint32 iMaxPointsPerQuad
                     , std::vector<FPointQuadTreeEntry>& iPointQuadTreeEntryArray
                     , uint32 iDepth
                     , uint32 iMaxDepth );

        void Build( uint32 iMaxPointsPerQuad
                     , std::vector<FPointQuadTreeEntry>& iParentPointQuadTreeEntryArray
                     , uint32 iDepth
                     , uint32 iMaxDepth );
        void Draw( BLContext* iBLContext
                 , FOdysseyVectorGroupPaint* iScene
                 , uint64 iFlags );
        void PickPoints( double iWorldX
                       , double iWorldY
                       , double iSelectionRadius
                       , std::vector<FOdysseyVectorPoint*>& oPickedPointArray );
    private:
        std::vector<FPointQuadTreeEntry> mPointQuadTreeEntryArray;
        FPointQuadTree* mChildren[4];
        ::ULIS::FRectD mRect;
};

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorBaseToolHUD : public FOdysseyHUDElement, public IOdysseyVectorHUD
{
    public:
        static const uint64 PICK_HANDLE_VERTEX  = 1;
        static const uint64 PICK_HANDLE_SEGMENT = 1 << 1;
        static const uint64 PICK_VERTEX         = 1 << 2;

    protected:
        struct BatchedLine
        {
            BatchedLine( FVector2D* iP0, FVector2D* iP1 )
                : p0 ( iP0 )
                , p1 ( iP1 )
                , perpendicular( 0.0f, 0.0f )
            {
                FVector2D vec = (*iP1) - (*iP0);

                if( vec.SizeSquared() )
                {
                    vec.Normalize();

                    perpendicular.X = -vec.Y;
                    perpendicular.Y =  vec.X;
                }
            }

            FVector2D* p0;
            FVector2D* p1;
            FVector2D perpendicular;
        };

    public:
        virtual ~FOdysseyPainterEditorVectorBaseToolHUD();
        FOdysseyPainterEditorVectorBaseToolHUD(  UOdysseyPainterEditorVectorBaseTool* iBaseTool );

        virtual bool OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
//        virtual bool OnMouseClick(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
//        virtual bool OnMouseDoubleClick(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
        virtual bool OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
//        virtual void OnMouseEnter() override;
        virtual void OnMouseHover(const FOdysseyPoint& iPointInTexture) override;
//        virtual void OnMouseLeave() override;
        virtual void OnMouseDrag(const FOdysseyPoint& iPointInTexture) override;
        virtual bool OnMouseClick( const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;

        virtual void Load( );
        virtual void Unload( );
        virtual void Reset( );
        // old HUD system
        virtual void Draw( BLContext* iBLContext ) override;

        virtual void DrawHUD( const FOdysseyHUDElement::FDrawHUDParams& iParams ) override;

        FSelectionBox& GetSelectionBox();

        std::list<FInbetweenerBreakdown*>& GetSelectedBreakdownList();
        std::list<FOdysseyVectorTagInbetweener*>& GetSelectedInbetweenerTagList();

        void PickPoints( double iWorldX
                       , double iWorldY
                       , double iSelectionRadius
                       , std::vector<FOdysseyVectorPoint*>& oPickedPointArray );

        void MakePointQuadTree( bool iFocusedObjectsOnly
                              , uint64 iHUDFlags );

        virtual void SetCursorPosition( double iX, double iY );
        bool PickPathPoints( FOdysseyVectorPath* iPath
                           , double iWorldX
                           , double iWorldY
                           , double iSelectionRadius
                           , std::vector<FOdysseyVectorVertex*>& oPickedVertexArray
                           , std::vector<FOdysseyVectorHandleSegment*>& oPickedHandleArray
                           , uint64 iSelectionFlags );
        FVector2D TextureToHUD( const FVector2D& iPosition );
        FVector2D TextureToHUD( double iX, double iY );
        FVector2D TextureToHUD( const ::ULIS::FVec2D& iPosition );

    protected:
        void DrawInbetweens( const FOdysseyHUDElement::FDrawHUDParams& iParams
                           , FInbetweenerBreakdown* iBreakdown
                           , eShowInbetweens ShowInbetweens
                           , uint64 iSourceExtraHUDFlags
                           , uint64 iTargetExtraHUDFlags );

        void DrawSectionArray( const FOdysseyHUDElement::FDrawHUDParams& iParams
                             , std::vector<FOdysseyVectorSection*>& isectionArray
                             , const FLinearColor& fgColor
                             , const FLinearColor& bgColor
                             , uint64 iHUDFlags );

        void DrawHierarchy( const FOdysseyHUDElement::FDrawHUDParams& iParams
                          , FOdysseyVectorObject* iTopObject
                          , const FLinearColor& iForegroundColor
                          , const FLinearColor& iBackgroundColor
                          , const FLinearColor& iHighlightColor
                          , uint64 iHUDFlags );

        void DrawVertex( const FOdysseyHUDElement::FDrawHUDParams& iParams
                       , FOdysseyVectorVertex* iVertex
                       , const FLinearColor& fgColor
                       , const FLinearColor& bgColor
                       , const FLinearColor& hcColor
                       , uint64 iHUDFlags );

        void DrawBreakdown( const FOdysseyHUDElement::FDrawHUDParams& iParams
                          , FInbetweenerBreakdown* iBreakdown
                          , const FLinearColor& iSourceDrawingColor
                          , const FLinearColor& iTargetDrawingColor
                          , uint64 iHUDFlags );

        void DrawGrid( const FOdysseyHUDElement::FDrawHUDParams& iParams
                     , FInbetweenerGrid* iGrid
                     , eInbetweenerPointPositionType iPositionType
                     , const FLinearColor& iColor
                     , uint64 iHUDFlags );

        void DrawCycle( const FOdysseyHUDElement::FDrawHUDParams& iParams
                      , FOdysseyVectorCycle* iCycle
                      , const FLinearColor& fgColor
                      , const FLinearColor& bgColor
                      , uint64 iHUDFlags );

        void DrawInbetweenerInterpolatedPathAt( const FOdysseyHUDElement::FDrawHUDParams& iParams
                                              , FOdysseyVectorTagInbetweener* iInbetweenerTag
                                              , FInterpolatedPath* iInterpolatedPath
                                              , FInbetweenerChart::Inbetween* iInbetween
                                              , const FLinearColor& iColor );

        void DrawPrimitivePlus( const FOdysseyHUDElement::FDrawHUDParams& iParams
                              , const FVector2D& iHUDCoords
                              , uint32 iSize
                              , const FLinearColor& iColor
                              , float iThickness );

        void DrawPrimitiveHandle( const FOdysseyHUDElement::FDrawHUDParams& iParams
                                , const FVector2D& iHUDCoords
                                , double iRadius
                                , const FLinearColor& fgColor
                                , const FLinearColor& bgColor );

        void DrawPrimitiveVertex( const FOdysseyHUDElement::FDrawHUDParams& iParams
                                , const FVector2D& iHUDCoords
                                , double iRadius
                                , const FLinearColor& fgColor
                                , const FLinearColor& bgColor );

        void DrawPrimitiveBucket( const FOdysseyHUDElement::FDrawHUDParams& iParams
                                , const FVector2D& iHUDCoords
                                , double iRadius
                                , const FLinearColor& iFillColor
                                , const FLinearColor& iOuterCountourColor
                                , const FLinearColor& iInnerContourColor );

        void DrawPrimitiveBezierCubic( const FOdysseyHUDElement::FDrawHUDParams& iParams
                                     , const FVector2D& iHUDCoordsP0
                                     , const FVector2D& iHUDCoordsP1
                                     , const FVector2D& iHUDCoordsP2
                                     , const FVector2D& iHUDCoordsP3
                                     , uint32 iFractionCount
                                     , const FLinearColor& iColor
                                     , float iThickness );

        void DrawPrimitiveBezierQuadratic( const FOdysseyHUDElement::FDrawHUDParams& iParams
                                         , const FVector2D& iHUDCoordsP0
                                         , const FVector2D& iHUDCoordsP1
                                         , const FVector2D& iHUDCoordsP2
                                         , uint32 iFractionCount
                                         , const FLinearColor& iColor
                                         , float iThickness );

        void DrawPrimitiveLine( const FOdysseyHUDElement::FDrawHUDParams& iParams
                              , const FVector2D& iHUDCoordsP0
                              , const FVector2D& iHUDCoordsP1
                              , const FLinearColor& iColor
                              , float iThickness );
        void DrawPrimitiveLineOutlined( const FOdysseyHUDElement::FDrawHUDParams& iParams
                                      , const FVector2D& iHUDCoordsP0
                                      , const FVector2D& iHUDCoordsP1
                                      , const FLinearColor& iColor
                                      , float iThickness );

        void DrawPrimitiveCircle( const FOdysseyHUDElement::FDrawHUDParams& iParams
                                , const FVector2D& iHUDCoords
                                , double iRadius
                                , const FLinearColor& iColor
                                , float iThickness );

        void DrawPath( const FOdysseyHUDElement::FDrawHUDParams& iParams
                     , FOdysseyVectorPath* iPath
                     , const FLinearColor& fgColor
                     , const FLinearColor& bgColor
                     , const FLinearColor& hcColor
                     , uint64 iHUDFlags );

        void DrawCubicSegment( const FOdysseyHUDElement::FDrawHUDParams& iParams
                             , FOdysseyVectorSegmentCubic* iCubicSegment
                             , const FLinearColor& fgColor
                             , const FLinearColor& bgColor
                             , const FLinearColor& hcColor
                             , uint64 iHUDFlags );

        void DrawGroupPaint( const FOdysseyHUDElement::FDrawHUDParams& iParams
                           , FOdysseyVectorGroupPaint* iPaintGroup
                           , const FLinearColor& fgColor
                           , const FLinearColor& bgColor
                           , const FLinearColor& hcColor
                           , uint64 iHUDFlags );

        void DrawBucket( const FOdysseyHUDElement::FDrawHUDParams& iParams
                       , FOdysseyVectorBucket* iBucket
                       , const FLinearColor& fgColor
                       , const FLinearColor& bgColor
                       , const FLinearColor& hcColor
                       , uint64 iHUDFlags );

        void DrawInfo( const FOdysseyHUDElement::FDrawHUDParams& iParams
                     , const FText& iText
                     , const FLinearColor& iColor );

        void DrawBatchedLines( const FOdysseyHUDElement::FDrawHUDParams& iParams
                             , const std::vector<BatchedLine>& iLineBuffer
                             , const std::vector<FVector2D>& iPointBuffer
                             , const FLinearColor& iColor
                             , float iThickness );
        ::ULIS::FRectD BBoxToHUD( const FOdysseyHUDElement::FDrawHUDParams& iParams
                                , const ::ULIS::FRectD& iBBox );

        void DrawDummyPlane( const FOdysseyHUDElement::FDrawHUDParams& iParams );
        FOdysseyVectorObject* PickPath( FOdysseyVectorGroup* iSelectionSpace
                                      , FOdysseyVectorPath* iPath
                                      , const BLImage& iHUDMaskImage
                                      , const ::ULIS::FRectD& iHUDRoi );
        FOdysseyVectorObject* PickObject( FOdysseyVectorGroup* iSelectionSpace
                                        , FOdysseyVectorObject* iObj
                                        , const BLImage& iHUDMaskImage
                                        , const ::ULIS::FRectD& iHUDRoi );

        void RecursivePick( FOdysseyVectorGroup* iSelectionSpace
                          , FOdysseyVectorObject* iObj
                          , const BLImage& iHUDMaskImage
                          , const ::ULIS::FRectD& iHUDRoi
                          , std::vector<FOdysseyVectorObject*>& oSelectedObjectArray );
        void Pick( FOdysseyVectorGroupPaint* iScene
                 , const BLImage& iHUDMaskImage
                 , const ::ULIS::FRectD& iHUDRoi
                 , std::vector<FOdysseyVectorObject*>& oPickedObjectArray );

    protected:
        void UpdateSelectionBoxVertexMode();
        void UpdateSelectionBoxObjectMode( bool iForceWorld );
        void UpdateSelectionBoxInbetweenMode( bool iForceWorld );
        void UpdateSelectionBox( bool iForceWorld
                               , uint64 iHUDFlags );

        void DrawSelectionBox( const FOdysseyHUDElement::FDrawHUDParams& iParams
                             , const FLinearColor& iForegroundColor
                             , const FLinearColor& iBackgroundColor
                             , const FLinearColor& iHighlightColor
                             , uint64 iHUDFlags );
        void UpdateSelectionInbetweenMode( bool iOnTargetCellOnly );
        void DrawText( BLContext* iBLContext
                     , const BLFont& iBLFont
                     , const BLRgba32& iForegroundColor
                     , const BLRgba32& iBackgroundColor
                     , const BLRgba32& iHighlightColor
                     , char* iText
                     , uint32 iX
                     , uint32 iY );
        void FormatModifierInfo( const FText* iCtrlText
                               , const FText* iShiftText
                               , const FText* iAltText );
        void DrawModifierInfo( const FOdysseyHUDElement::FDrawHUDParams& iParams );
        FVector2D WorldPointToHUD( const FVector2D& iWorldCoords );
        FVector2D WorldVectorToHUD( const FVector2D& iWorldOriginCoords
                                  , const FVector2D& iWorldVectorCoords );
        ::ULIS::FVec2D WorldPointToHUD( const ::ULIS::FVec2D& iWorldCoords );
        ::ULIS::FVec2D WorldVectorToHUD( const ::ULIS::FVec2D& iWorldOriginCoords
                                       , const ::ULIS::FVec2D& iWorldVectorCoords );
        ::ULIS::FRectD WorldRectToHUD( const ::ULIS::FRectD& iWorldRect );
        // checks if we are tryign to Pan / Rotate / Zoom the viewport or Pick
        bool CanReceiveEvents();

    protected:
        UOdysseyPainterEditorVectorBaseTool* mBaseTool;
        FSelectionBox mSelectionBox;
        std::list<FInbetweenerBreakdown*> mSelectedBreakdownList;
        std::list<FOdysseyVectorTagInbetweener*> mSelectedInbetweenerTagList;
        TObjectPtr<UTexture> mVertexTexture;
        TObjectPtr<UTexture> mVertexContourTexture;
        TObjectPtr<UTexture> mHandleTexture;
        TObjectPtr<UTexture> mBucketInnerTexture;
        TObjectPtr<UTexture> mBucketOuterTexture;
        TObjectPtr<UTexture> mBucketPropagateTexture;
        TObjectPtr<UTexture> mInfoBorderLeftTexture;
        TObjectPtr<UTexture> mInfoBorderTexture;
        TObjectPtr<UTexture> mInfoBorderRightTexture;
        TObjectPtr<UTexture> mLineOutlinedTexture;
        double mX;
        double mY;
        FOdysseyHUDElement::FDrawHUDParams mCurrentHUDParams;

    protected:
        FPointQuadTree* mPointQuadTree;
        FSlateFontInfo mFontInfo;
        FText mModifierInfoText;
};

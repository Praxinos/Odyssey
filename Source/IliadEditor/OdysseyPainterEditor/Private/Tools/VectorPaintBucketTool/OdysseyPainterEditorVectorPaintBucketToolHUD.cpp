// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Tools/VectorPaintBucketTool/OdysseyPainterEditorVectorPaintBucketToolHUD.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVectorBucket.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorCycle.h"
#include "OdysseyVector.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

FOdysseyPainterEditorVectorPaintBucketToolHUD::~FOdysseyPainterEditorVectorPaintBucketToolHUD()
{
}

FOdysseyPainterEditorVectorPaintBucketToolHUD::FOdysseyPainterEditorVectorPaintBucketToolHUD( UOdysseyPainterEditorVectorPaintBucketTool* iPaintBucketTool )
    : FOdysseyPainterEditorVectorBaseToolHUD( iPaintBucketTool )
    , mPaintBucketTool( iPaintBucketTool )
{
}

void
FOdysseyPainterEditorVectorPaintBucketToolHUD::Reset()
{
    UpdateSelectionBox( false, mPaintBucketTool->GetEditor()->GetVectorHUDFlags() );

    UpdateWorkingPaintgroupList( );
}

void
FOdysseyPainterEditorVectorPaintBucketToolHUD::Load()
{
    Reset();

    FOdysseyPainterEditorVectorBaseToolHUD::Load( );
}

void
FOdysseyPainterEditorVectorPaintBucketToolHUD::Unload()
{
}

std::list<FOdysseyVectorGroupPaint*>&
FOdysseyPainterEditorVectorPaintBucketToolHUD::GetWorkingPaintgroupList()
{
    return mWorkingPaintgroupList;
}

void
FOdysseyPainterEditorVectorPaintBucketToolHUD::UpdateWorkingPaintgroupList()
{
    mWorkingPaintgroupList.clear();

    FOdysseyVectorObject::Traverse
    ( mScene
    , 0
    , [ this ]( FOdysseyVectorObject* object, uint64 traversalFlags ) -> uint64
        {
            if( mScene->GetCell()->ObjectHasFocus( object, traversalFlags ) )
            {
                if( object->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
                {
                    FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(object);

                    mWorkingPaintgroupList.push_back( paintGroup );
                }
                else
                {
                    FOdysseyVectorGroupPaint* ancestor = static_cast<FOdysseyVectorGroupPaint*>(object->GetAncestorByClass( FOdysseyVectorGroupPaint::StaticClass(), false ));

                    if( ancestor )
                    {
                        if( std::find( mWorkingPaintgroupList.begin()
                                     , mWorkingPaintgroupList.end()
                                     , ancestor ) == mWorkingPaintgroupList.end() )
                        {
                            mWorkingPaintgroupList.push_back( ancestor );
                        }
                    }
                }

                return FOdysseyVectorObject::TRAVERSE_OBJECT_ACCEPTED;
            }

            return 0;
        } );
}

uint32
FOdysseyPainterEditorVectorPaintBucketToolHUD::PickBucketArea( FOdysseyVectorBucket* iBucket
                                                             , double iWorldX
                                                             , double iWorldY )
{
    ::ULIS::FVec2D bucketWorldCoords = FOdysseyVectorHUD::GetBucketPosition( iBucket, true );
    FVector2D bucketHUDCoords = TextureToHUD( FVector2D( bucketWorldCoords.x, bucketWorldCoords.y ) );
    FVector2D mouseHUDCoords = TextureToHUD( FVector2D( iWorldX, iWorldY ) );
    FVector2D pickHUDDif = mouseHUDCoords - bucketHUDCoords;

    if( mPaintBucketTool->GetEditionMode() == eVectorPaintBucketEditionMode::Control )
    {
        if( iBucket->GetColorMode() == eBucketColorMode::LinearGradient )
        {
            BLMatrix2D& worldMatrix = iBucket->GetOwner()->GetWorldMatrix();
            ::ULIS::FVec2D localP0 = iBucket->GetLinearP0();
            ::ULIS::FVec2D localP1 = iBucket->GetLinearP1();
            ::ULIS::FVec2D handleWorldCoords[2] = { FOdysseyVector::MapPoint( worldMatrix, localP0 )
                                                  , FOdysseyVector::MapPoint( worldMatrix, localP1 ) };
            FVector2D handleHUDCoords[2] = { TextureToHUD( FVector2D( handleWorldCoords[0].x
                                                                    , handleWorldCoords[0].y ) )
                                           , TextureToHUD( FVector2D( handleWorldCoords[1].x
                                                                    , handleWorldCoords[1].y ) ) };

            if( ( mouseHUDCoords - handleHUDCoords[0] ).Size() < mPaintBucketTool->PickingRadius )
            {
                return PICK_LINEAR_HANDLE0;
            }

            if( ( mouseHUDCoords - handleHUDCoords[1] ).Size() < mPaintBucketTool->PickingRadius )
            {
                return PICK_LINEAR_HANDLE1;
            }

        }

        if( iBucket->GetColorMode() == eBucketColorMode::RadialGradient )
        {
            ::ULIS::FVec2D handleWorldCoords = FOdysseyVectorHUD::GetBucketRadialHandlePosition( iBucket, true );
            FVector2D handleHUDCoords = TextureToHUD( FVector2D( handleWorldCoords.x, handleWorldCoords.y ) );
            ::ULIS::FVec2D centerWorldCoords = FOdysseyVectorHUD::GetBucketRadialPosition( iBucket, true );
            FVector2D centerHUDCoords = TextureToHUD( FVector2D( centerWorldCoords.x, centerWorldCoords.y ) );

            if ( ( mouseHUDCoords - handleHUDCoords ).Size() < mPaintBucketTool->PickingRadius )
            {
                return PICK_RADIAL_HANDLE;
            }

            if ( ( mouseHUDCoords - centerHUDCoords ).Size() < FOdysseyVectorHUD::RADIAL_AREA_RADIUS )
            {
                return PICK_RADIAL_AREA;
            }
        }

        if ( pickHUDDif.Size() < mPaintBucketTool->PickingRadius )
        {
            return PICK_PROPAGATE;
        }
    }
    else
    {
        if ( pickHUDDif.Size() < mPaintBucketTool->PickingRadius )
        {
            return PICK_BUCKET;
        }
    }

    return PICK_NONE;
}

void
FOdysseyPainterEditorVectorPaintBucketToolHUD::PickCycles( double iWorldX
                                                         , double iWorldY
                                                         , std::vector<FOdysseyVectorCycle*>& oPickedCycleArray )
{
    oPickedCycleArray.clear();

    for( FOdysseyVectorGroupPaint* paintgroup : mWorkingPaintgroupList )
    {
        FOdysseyVectorCycle* pickedCycle = paintgroup->PickCycle( iWorldX, iWorldY );

        if( pickedCycle )
        {
            oPickedCycleArray.push_back( pickedCycle );
        }
    }
}

FOdysseyVectorBucket*
FOdysseyPainterEditorVectorPaintBucketToolHUD::PickBucket( double iWorldX
                                                         , double iWorldY )
{
    FOdysseyVectorBucket* pickedBucket = nullptr;

    for( FOdysseyVectorGroupPaint* paintgroup : mWorkingPaintgroupList )
    {
        std::list<FOdysseyVectorBucket*>& bucketList = paintgroup->GetBucketList();

        for( FOdysseyVectorBucket *bucket : bucketList )
        {
            if( PickBucketArea( bucket, iWorldX, iWorldY ) )
            {
                pickedBucket = bucket;

                return pickedBucket; // return now;
            }
        }
    }

    return nullptr;
}

void
FOdysseyPainterEditorVectorPaintBucketToolHUD::DrawHUD( const FOdysseyHUDElement::FDrawHUDParams& iParams )
{
    mCurrentHUDParams = iParams;

    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    FColor& hc = FOdysseyVectorHUD::GetHighlightColor();
    FLinearColor fgColor = FLinearColor( fg );
    FLinearColor bgColor = FLinearColor( bg );
    FLinearColor hcColor = FLinearColor( hc );
    uint64 viewBucketHandleFlag = ( mPaintBucketTool->GetEditionMode() == eVectorPaintBucketEditionMode::Control ) ? FOdysseyVectorHUD::HUD_GROUPPAINT_BUCKET_HANDLE : 0;
    uint64 hudFlags = mPaintBucketTool->GetEditor()->GetVectorHUDFlags();
    std::vector<FOdysseyVectorCycle*> pickedCycleArray;

    if( hudFlags & FOdysseyVectorHUD::HUD_MODE_VERTEX )
    {
        DrawHierarchy( iParams
                     , mScene
                     , fgColor
                     , bgColor
                     , hcColor
                     , hudFlags
                     | FOdysseyVectorHUD::HUD_PATH_VERTEX
                     | FOdysseyVectorHUD::HUD_PATH_SEGMENT
                     | FOdysseyVectorHUD::HUD_GROUPPAINT_BUCKET
                     | viewBucketHandleFlag );
    }

    // Draw object details only in vertex mode
    if( hudFlags & FOdysseyVectorHUD::HUD_MODE_OBJECT )
    {
        DrawHierarchy( iParams
                     , mScene
                     , fgColor
                     , bgColor
                     , hcColor
                     , hudFlags
                     | FOdysseyVectorHUD::HUD_GROUPPAINT_BUCKET
                     | viewBucketHandleFlag );
    }

    if( mPaintBucketTool->GetEditionMode() == eVectorPaintBucketEditionMode::Default )
    {
        PickCycles( mX, mY, pickedCycleArray );

        for( FOdysseyVectorCycle* cycle : pickedCycleArray)
        {
            FOdysseyVectorObject* owner = cycle->GetOwner();
            BLMatrix2D& worldMatrix = owner->GetWorldMatrix();

            DrawCycle( iParams, cycle, hcColor, bgColor, hudFlags );
        }
    }

    // invisible plane will get mouse events
    DrawDummyPlane( iParams );
}

void
FOdysseyPainterEditorVectorPaintBucketToolHUD::Draw( BLContext* iBLContext )
{

}

#undef LOCTEXT_NAMESPACE

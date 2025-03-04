// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
                    FOdysseyVectorGroupPaint* ancestor = static_cast<FOdysseyVectorGroupPaint*>(object->GetAncestorByClass( FOdysseyVectorGroupPaint::StaticClass() ));

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
    ::ULIS::FVec2D pickDif = ::ULIS::FVec2D( iWorldX - bucketWorldCoords.x
                                           , iWorldY - bucketWorldCoords.y );

    if( mPaintBucketTool->GetEditionMode() == eVectorPaintBucketEditionMode::Control )
    {
        if( iBucket->GetColorMode() == eBucketColorMode::LinearGradient )
        {
            BLMatrix2D& worldMatrix = iBucket->GetOwner()->GetWorldMatrix();
            ::ULIS::FVec2D localP0 = iBucket->GetLinearP0();
            ::ULIS::FVec2D localP1 = iBucket->GetLinearP1();
            ::ULIS::FVec2D worldHandle[2] = { FOdysseyVector::MapPoint( worldMatrix, localP0 )
                                            , FOdysseyVector::MapPoint( worldMatrix, localP1 ) };

            if( ::ULIS::FVec2D( iWorldX - worldHandle[0].x
                              , iWorldY - worldHandle[0].y ).Distance() < mPaintBucketTool->PickingRadius )
            {
                return PICK_LINEAR_HANDLE0;
            }

            if( ::ULIS::FVec2D( iWorldX - worldHandle[1].x
                              , iWorldY - worldHandle[1].y ).Distance() < mPaintBucketTool->PickingRadius )
            {
                return PICK_LINEAR_HANDLE1;
            }

        }

        if( iBucket->GetColorMode() == eBucketColorMode::RadialGradient )
        {
            ::ULIS::FVec2D radialHandleWorldCoords = FOdysseyVectorHUD::GetBucketRadialHandlePosition( iBucket, true );
            ::ULIS::FVec2D radialHandleDif = ::ULIS::FVec2D( iWorldX - radialHandleWorldCoords.x
                                                           , iWorldY - radialHandleWorldCoords.y );
            ::ULIS::FVec2D radialWorldCoords = FOdysseyVectorHUD::GetBucketRadialPosition( iBucket, true );
            ::ULIS::FVec2D radialDif = ::ULIS::FVec2D( iWorldX - radialWorldCoords.x
                                                     , iWorldY - radialWorldCoords.y );

            if ( radialHandleDif.Distance() < mPaintBucketTool->PickingRadius )
            {
                return PICK_RADIAL_HANDLE;
            }

            if ( radialDif.Distance() < FOdysseyVectorHUD::RADIAL_AREA_RADIUS )
            {
                return PICK_RADIAL_AREA;
            }
        }

        if ( pickDif.Distance() < mPaintBucketTool->PickingRadius )
        {
            return PICK_PROPAGATE;
        }
    }
    else
    {
        if ( pickDif.Distance() < mPaintBucketTool->PickingRadius )
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
FOdysseyPainterEditorVectorPaintBucketToolHUD::DrawHUD( const FOdysseyHUD::FDrawHUDParams& iParams )
{
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

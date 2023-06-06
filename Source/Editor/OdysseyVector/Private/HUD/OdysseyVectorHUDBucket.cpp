#include "HUD/OdysseyVectorHUDBucket.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorGroupPaint.h"

FOdysseyVectorHUDBucket::~FOdysseyVectorHUDBucket()
{
}

FOdysseyVectorHUDBucket::FOdysseyVectorHUDBucket()
    : mCycle ( nullptr )
{
}

void
FOdysseyVectorHUDBucket::SetCycle( FOdysseyVectorCycle* iCycle )
{
    mCycle = iCycle;
}

void
FOdysseyVectorHUDBucket::Draw( FOdysseyVectorScene* iScene, uint64 iFlags )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetSelectedObjectList();
    BLContext* blctx = iScene->GetEngine()->GetBLContext();
    ::ULIS::FRectD bbox = { 0, 0, 0, 0 };
    BLPath path;

    // matrix might get altered for displaying the selection rectangle of a single object. Save it.
    blctx->save();

    if( selectedObjectList.size() == 1 )
    {
        FOdysseyVectorObject* selectedObject = iScene->GetLastSelected();

        if( selectedObject->GetClass() == FOdysseyVectorGroupPaint::StaticClass() )
        {
            FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(selectedObject);
            BLMatrix2D& worldMatrix = paintGroup->GetWorldMatrix();

            blctx->setMatrix( worldMatrix );

            if ( mCycle )
            {
                blctx->setCompOp( BL_COMP_OP_SRC_OVER );
                blctx->setStrokeStyle( BLRgba32( 0x800000FF ) );
                blctx->setStrokeWidth( 4.0f );
                mCycle->StrokePath( true );

                mCycle = nullptr; // reset after each draw, for safety. The tool has to set the cycle at each hovering.
            }

            paintGroup->DrawBuckets( FBucketDrawingFlags::PELLET );
        }

        bbox = selectedObject->GetBBox( false );
    }

    blctx->restore();
}

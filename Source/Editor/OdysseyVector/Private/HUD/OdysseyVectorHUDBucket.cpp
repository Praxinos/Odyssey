#include "HUD/OdysseyVectorHUDBucket.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorGroupPaint.h"

FOdysseyVectorHUDBucket::~FOdysseyVectorHUDBucket()
{
}

FOdysseyVectorHUDBucket::FOdysseyVectorHUDBucket()
{
}

void
FOdysseyVectorHUDBucket::Draw( UOdysseyVectorRoot& iScene, ::ULIS::FRectD& iRoi, uint64 iFlags )
{
    std::list<UOdysseyVectorObject*>& selectedObjectList = iScene.GetSelectedObjectList();
    BLContext* blctx = iScene.GetEngine()->GetBLContext();
    ::ULIS::FRectD bbox = { 0, 0, 0, 0 };
    BLPath path;

    // matrix might get altered for displaying the selection rectangle of a single object. Save it.
    blctx->save();

    if( selectedObjectList.size() == 1 )
    {
        UOdysseyVectorObject* selectedObject = iScene.GetLastSelected();

        if( selectedObject->GetClass() == UOdysseyVectorGroupPaint::StaticClass() )
        {
            UOdysseyVectorGroupPaint* paintGroup = Cast<UOdysseyVectorGroupPaint>(selectedObject);
            BLMatrix2D& worldMatrix = paintGroup->GetWorldMatrix();

            blctx->setMatrix( worldMatrix ); 
            paintGroup->DrawBuckets( iRoi, iFlags );
        }



        bbox = selectedObject->GetBBox( false );
    }

    blctx->restore();
}

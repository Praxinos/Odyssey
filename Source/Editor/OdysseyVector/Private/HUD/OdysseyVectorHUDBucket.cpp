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
FOdysseyVectorHUDBucket::Draw( FOdysseyVectorScene& iScene, ::ULIS::FRectD& iRoi, uint64 iFlags )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = iScene.GetSelectedObjectList();
    BLContext* blctx = iScene.GetEngine()->GetBLContext();
    ::ULIS::FRectD bbox = { 0, 0, 0, 0 };
    BLPath path;

    // matrix might get altered for displaying the selection rectangle of a single object. Save it.
    blctx->save();

    if( selectedObjectList.size() == 1 )
    {
        FOdysseyVectorObject* selectedObject = iScene.GetLastSelected();

        if( selectedObject->GetClass() == FOdysseyVectorGroupPaint::StaticClass() )
        {
            FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(selectedObject);
            BLMatrix2D& worldMatrix = paintGroup->GetWorldMatrix();

            blctx->setMatrix( worldMatrix ); 
            paintGroup->DrawBuckets( iRoi, iFlags );
        }



        bbox = selectedObject->GetBBox( false );
    }

    blctx->restore();
}

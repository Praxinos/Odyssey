#include "View/OdysseyVectorViewGroupPaint.h"

UOdysseyVectorViewGroupPaint::~UOdysseyVectorViewGroupPaint()
{
}

UOdysseyVectorViewGroupPaint::UOdysseyVectorViewGroupPaint()
    : UOdysseyVectorViewObject()
{
}

void
UOdysseyVectorViewGroupPaint::ImportParam( FOdysseyVectorObject* iObject )
{
    UOdysseyVectorViewObject::ImportParam( iObject );

    GroupPaintParam = static_cast<FOdysseyVectorGroupPaint*>(iObject)->mGroupPaintParam;
}

void
UOdysseyVectorViewGroupPaint::ExportParam( FOdysseyVectorObject* iObject )
{
    UOdysseyVectorViewObject::ExportParam( iObject );

    static_cast<FOdysseyVectorGroupPaint*>(iObject)->mGroupPaintParam = GroupPaintParam;
}

void
UOdysseyVectorViewGroupPaint::PropertyChanged( const FName& iPropertyName, const FName& iCategory )
{
    FOdysseyVectorScene* scene = mObject->GetScene();

    UOdysseyVectorViewObject::PropertyChanged( iPropertyName, iCategory );

    /*if( iCategory == "Transform" )
    {
        mObject->UpdateMatrix();
    }*/

    mObject->Invalidate();

    scene->Update( 0 );
}

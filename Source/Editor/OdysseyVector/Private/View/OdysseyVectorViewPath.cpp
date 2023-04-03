#include "View/OdysseyVectorViewPath.h"

UOdysseyVectorViewPath::~UOdysseyVectorViewPath()
{
}

UOdysseyVectorViewPath::UOdysseyVectorViewPath()
    : UOdysseyVectorViewObject()
{
}

void
UOdysseyVectorViewPath::ImportParam( FOdysseyVectorObject* iObject )
{
    UOdysseyVectorViewObject::ImportParam( iObject );

     PathParam = static_cast<FOdysseyVectorPath*>(iObject)->mPathParam;
}

void
UOdysseyVectorViewPath::ExportParam( FOdysseyVectorObject* iObject )
{
    UOdysseyVectorViewObject::ExportParam( iObject );

    static_cast<FOdysseyVectorPath*>(iObject)->mPathParam = PathParam;
}

void
UOdysseyVectorViewPath::PropertyChanged( const FName& iPropertyName, const FName& iCategory )
{
    UOdysseyVectorViewObject::PropertyChanged( iPropertyName, iCategory );

    /*if( iCategory == "Transform" )
    {
        mObject->UpdateMatrix();
    }*/
}

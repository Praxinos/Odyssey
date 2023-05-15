#include "View/OdysseyVectorViewEllipse.h"

UOdysseyVectorViewEllipse::~UOdysseyVectorViewEllipse()
{
}

UOdysseyVectorViewEllipse::UOdysseyVectorViewEllipse()
    : UOdysseyVectorViewPath()
{
}

void
UOdysseyVectorViewEllipse::ImportParam( FOdysseyVectorObject* iObject )
{
    UOdysseyVectorViewPath::ImportParam( iObject );

     EllipseParam = static_cast<FOdysseyVectorEllipse*>(iObject)->mEllipseParam;
}

void
UOdysseyVectorViewEllipse::ExportParam( FOdysseyVectorObject* iObject )
{
    UOdysseyVectorViewPath::ExportParam( iObject );

    static_cast<FOdysseyVectorEllipse*>(iObject)->mEllipseParam = EllipseParam;
}

void
UOdysseyVectorViewEllipse::PropertyChanged( const FName& iPropertyName, const FName& iCategory )
{
    FOdysseyVectorScene* scene = mObject->GetScene();

    UOdysseyVectorViewPath::PropertyChanged( iPropertyName, iCategory );

    if( iCategory == "Geometry" )
    {
        mObject->Update( 0 );
    }

    mObject->Invalidate();

    scene->Update(0);
}

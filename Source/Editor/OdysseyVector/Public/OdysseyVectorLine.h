#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include "OdysseyVectorObject.h"
#include "OdysseyVectorSegmentCubic.h"
#include "OdysseyVectorPrimitive.h"

#include "OdysseyVectorLine.generated.h"

USTRUCT()
struct FLineParam
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category="Geometry")
    double StrokeWidth;

    UPROPERTY(EditAnywhere, Category="Geometry")
    double Width;

    UPROPERTY(EditAnywhere, Category="Geometry")
    double Height;
};

class ODYSSEYVECTOR_API FOdysseyVectorLine : public FOdysseyVectorPrimitive
{
    private:
        static const uint32 mStaticClass = 0x9ee34077; // value is crc32 FOdysseyVectorRectangle

    public:
        static uint32 StaticClass() { return mStaticClass; };
        virtual uint32 GetClass() { return mStaticClass; };

        bool HasBaseClass( uint32 iBaseClassID );

    private:
        void DrawShape( ::ULIS::FRectD &iRoi, uint64 iFlags );
        bool PickShape( ::ULIS::FRectD &iRoi, uint32 iSelectionFlags );
        FOdysseyVectorObject* CopyShape();
        void UpdateShape( uint32 iUpdateFlags );

    protected :
        FOdysseyVectorVertex* mCubicVertex[2];
        FOdysseyVectorSegmentCubic* mCubicSegment;

    public:
        virtual ~FOdysseyVectorLine();
        FOdysseyVectorLine( FString iName, double iWidth, double iHeight, double iStrokeWidth );
        void Init( const FString& iName, double iWidth, double iHeight );
        void SetSize( double iWidth, double iHeight );
        double GetWidth();
        double GetHeight();
        virtual FOdysseyVectorPathCubic* Convert() override;

    public:
        FLineParam mLineParam;
};

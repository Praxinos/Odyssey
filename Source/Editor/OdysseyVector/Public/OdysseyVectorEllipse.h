#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <ULIS>
#include "OdysseyVectorVertex.h"
#include "OdysseyVectorSegmentCubic.h"
#include "OdysseyVectorPathCubic.h"

#include "OdysseyVectorEllipse.generated.h"

USTRUCT()
struct FEllipseParam
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category="Geometry")
    double Width;

    UPROPERTY(EditAnywhere, Category="Geometry")
    double RadiusX;

    UPROPERTY(EditAnywhere, Category="Geometry")
    double RadiusY;
};

class ODYSSEYVECTOR_API FOdysseyVectorEllipse : public FOdysseyVectorPathCubic
{
    private:
        static const uint32 mStaticClass = 0x1147fdfe; // value is crc32 FOdysseyVectorEllipse

    public:
        static uint32 StaticClass() { return mStaticClass; };
        virtual uint32 GetClass() { return mStaticClass; };

        bool HasBaseClass( uint32 iBaseClassID );

       /**
         * @brief Static function to allocate a new ellipse. Note: this is the proper way to allocate a new ellipse as we don't
         * use the constructor to set parameters because UOBJECTs must have empty constructors.
         * @param iName object's name
         * @param iRadiusX radius on X axis
         * @param iRadiusY radius on Y axis
         */
        static FOdysseyVectorEllipse* New( std::string iName, double iRadiusX, double iRadiusY );

       /**
         * @brief destructor.
         */
        virtual ~FOdysseyVectorEllipse();

       /**
         * @brief constructor.
         */
         FOdysseyVectorEllipse();

       /**
         * @brief Init an ellipse.
         * @param iName object's name
         * @param iRadiusX radius on X axis
         * @param iRadiusY radius on Y axis
         */
        void Init( std::string iName, double iRadiusX, double iRadiusY );

       /**
         * @brief Set ellipse radius on both X and Y axis.
         * @param iRadius the radius.
         */
        void SetRadius( double iRadius );

       /**
         * @brief Set ellipse radius.
         * @param iRadiusX radius on X axis
         * @param iRadiusY radius on Y axis
         */
        void SetRadius( double iRadiusX, double iRadiusY );

       /**
         * @brief Get ellipse's radius on X axis.
         * @return radius on X axis
         */
        double GetRadiusX();

       /**
         * @brief Get ellipse's radius on Y axis.
         * @return radius on Y axis
         */
        double GetRadiusY();

       /**
         * @brief Convert this ellipse to cubic path.
         * @return a newly allocated cubic path that looks the same as this ellipse.
         */
        FOdysseyVectorPathCubic* Convert();

       /**
         * @brief Get object type
         * @return VECTORELLIPSETYPE.
         */
        virtual uint32 GetType() override;

    protected:
       /**
         * @brief Copy this ellipse (for copy-paste operations).
         * @return a newly allocated ellipse that looks the same as this ellipse.
         */
        virtual FOdysseyVectorObject* CopyShape() override;

        virtual void DrawShape( ::ULIS::FRectD &iRoi, uint64 iFlags ) override;
        virtual bool PickShape( ::ULIS::FRectD& iRoi, uint32 iSelectionFlags ) override;
        virtual void UpdateShape( uint32 iUpdateFlags ) override;

    protected :
        FOdysseyVectorVertex* mCubicVertex[4];
        FOdysseyVectorSegmentCubic* mCubicSegment[4];
        double mStrokeWidth;

    public:
        FEllipseParam mEllipseParam;
};

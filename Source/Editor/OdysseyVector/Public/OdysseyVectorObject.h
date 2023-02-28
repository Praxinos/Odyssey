#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <ULIS>

#include "OdysseyVectorObject.generated.h"

class UOdysseyVectorRoot;
class UOdysseyVectorGroup;

UCLASS()
class ODYSSEYVECTOR_API UOdysseyVectorObject : public UObject
{
    public:
        GENERATED_BODY()

        DECLARE_MULTICAST_DELEGATE(FOnPropertyChanged);

        static const uint32 PICK_MATH_BASED = ( 1 << 0 );
        static const uint32 PICK_MASK_BASED = ( 1 << 3 );

        // DO NOT CHANGE !
        static const uint32 VECTORROOTTYPE       = 0;
        static const uint32 VECTOROBJECTTYPE     = 1;
        static const uint32 VECTORGROUPTYPE      = 2;
        static const uint32 VECTORCIRCLETYPE     = 3;
        static const uint32 VECTORRECTANGLETYPE  = 4;
        static const uint32 VECTORPATHCUBICTYPE  = 5;
        static const uint32 VECTORGROUPPAINTTYPE = 6;

        // update mask
        static const uint32 FREQUENTUPDATES = ( 1 << 0 );
        static const uint32 KEEPINVALIDATED = ( 1 << 1 );

        static constexpr float BBOX_POINT_RADIUS = 4.0f;

    protected:
        std::string Name;

        UPROPERTY(EditAnywhere, Category="Transform")
        double TranslationX;

        UPROPERTY(EditAnywhere, Category="Transform")
        double TranslationY;

        UPROPERTY(EditAnywhere, Category="Transform")
        double Rotation;

        UPROPERTY(EditAnywhere, Category="Transform")
        double ScalingX;

        UPROPERTY(EditAnywhere, Category="Transform")
        double ScalingY;

        UPROPERTY(EditAnywhere, Category="Coloring")
        FColor Foreground;

        UPROPERTY(EditAnywhere,Category="Coloring")
        FColor Background;

    protected:
        BLMatrix2D mLocalMatrix;
        BLMatrix2D mInverseLocalMatrix;
        BLMatrix2D mWorldMatrix;
        BLMatrix2D mInverseWorldMatrix;
        std::list<UOdysseyVectorObject*> mChildrenList;
        UOdysseyVectorObject* mParent;
        bool mIsFilled;
        bool mIsSelected;
        bool mIsInvalidated;
        bool mDependsOnChildren;
        ::ULIS::FRectD mBBox;

        /*uint32 mStrokeColor;*/
        /*uint32 mFillColor;*/

        // used when saving
        uint32 mID;

    public:
        static uint32 TreeToList( UOdysseyVectorObject* iObject, std::list<UOdysseyVectorObject*>& iOutList );
        static uint32 TreeToArray( UOdysseyVectorObject* iObject, std::vector<UOdysseyVectorObject*>& iOutArray );
        static ::ULIS::FRectD GetBoundingBoxFromList( std::list<UOdysseyVectorObject*>& iObjectList );

        ~UOdysseyVectorObject();
        UOdysseyVectorObject();
        void SetName( std::string iName );
        void CopySettings( UOdysseyVectorObject& iDestinationObject );

        virtual bool Erase( ::ULIS::FRectD &iRoi ){ return false; };

        void Update( uint32 iUpdateFlags );
        virtual void UpdateShape( uint32 iUpdateFlags ) {};

        UOdysseyVectorObject* Copy();
        virtual UOdysseyVectorObject* CopyShape(){ return nullptr; };

        void Draw( ::ULIS::FRectD& iRoi, uint64 iFlags );
        virtual void DrawShape ( ::ULIS::FRectD &roi, uint64 iFlags ){};

        virtual void DrawStructure ( ::ULIS::FRectD &roi, uint64 iFlags ){};

        virtual uint32 GetType();

        UOdysseyVectorObject* Pick( UOdysseyVectorGroup* iSelectionSpace, ::ULIS::FRectD& iRoi, uint32 iSelectionFlags );
        virtual bool PickShape( ::ULIS::FRectD& iRoi, uint32 iSelectionFlags ){ return false; };

        /*virtual void UpdateBoundingBox() = 0;*/
        void DrawChildren( ::ULIS::FRectD& iRoi, uint64 iFlags );
        void UpdateMatrix( );
        void Translate( double iX, double iY );
        void Rotate( double iAngle );
        void Scale( double iX, double iY );
        void PrependChild( UOdysseyVectorObject* iChild );
        void AppendChild( UOdysseyVectorObject* iChild );
        void AddChild( UOdysseyVectorObject* iChild, bool iPrepend );
        void RemoveChild( UOdysseyVectorObject* iChild );
        void ImportChild( UOdysseyVectorObject* iChild, BLMatrix2D& iInverseWorldMatrix );
        double GetScalingX();
        double GetScalingY();
        double GetTranslationX();
        double GetTranslationY();
        double GetRotation();
        void ResetTransform();
        void SetID( uint32 iID );
        uint32 GetID();
        UOdysseyVectorObject* GetParent();
        void SetParent( UOdysseyVectorObject* iObject );
        void CopyTransformation( UOdysseyVectorObject& iObject );
        BLMatrix2D& GetLocalMatrix();
        BLMatrix2D& GetWorldMatrix();
        BLMatrix2D& GetInverseWorldMatrix();
        BLMatrix2D& GetInverseLocalMatrix();
        std::list<UOdysseyVectorObject*>& GetChildrenList();
        void SetForegroundColor( uint8 iR, uint8 iG, uint8 iB, uint8 iA );
        void SetBackgroundColor( uint8 iR, uint8 iG, uint8 iB, uint8 iA );
        void SetFilled(bool iIsFilled);
        void SetStrokeWidth( double iWidth );
        double GetStrokeWidth( );
        ::ULIS::FVec2D WorldCoordinatesToLocal( double iX, double iY );
        void SetIsSelected( bool iIsSelected );
        ::ULIS::FRectD GetBBox( bool iWorld );
        bool IsFilled();
        void MoveBack();
        void MoveFront();
        void Invalidate();
        UOdysseyVectorRoot* GetRoot();
        bool IsInvalidated();
        bool IsSelected();
        void DrawBBox( ::ULIS::FRectD& iRoi,uint64 iFlags );
        int32 PickBBox( double iX, double iY );
        bool HasSelectedParent();
        FColor GetForegroundColor();
        FColor GetBackgroundColor();
        virtual void SwitchSpace( UOdysseyVectorObject& iNewSpace ){};
        // UObject overrides
        virtual void PropertyChanged(const FName& iPropertyName);
        virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent) override;
        //virtual void PostTransacted(const FTransactionObjectEvent& iTransactionEvent) override;
};

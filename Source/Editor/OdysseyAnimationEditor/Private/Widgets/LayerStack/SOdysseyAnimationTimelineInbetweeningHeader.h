// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

//#include "LayerStack/LightTable/OdysseyAnimationLightTable.h"

class FInbetweeningListViewItem;
class FOdysseyVectorSharedEnv;

//////////////////////////////////////////////////////////////////////////
// SOdysseyAnimationTimelineInbetweeningHeader
class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationTimelineInbetweeningHeader
    : public SListView<TSharedPtr<FInbetweeningListViewItem>>
{
    public:
        SOdysseyAnimationTimelineInbetweeningHeader();

    public:
	    SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineInbetweeningHeader)
            {}
        SLATE_END_ARGS()

	    void Construct( const FArguments& InArgs
                      , FOdysseyVectorSharedEnv* iVectorSharedEnv );

        void Update();

    protected:
        TSharedRef<ITableRow> OnGenerateRow( TSharedPtr<FInbetweeningListViewItem> iItem
                                           , const TSharedRef<STableViewBase>& iOwnerTable );
    private :
        virtual bool Private_IsItemSelected( const TSharedPtr<FInbetweeningListViewItem>& iItem )  const override;

    protected:
        TArray<TSharedPtr<FInbetweeningListViewItem>> mItemsSource;
        FOdysseyVectorSharedEnv* mVectorSharedEnv;
};

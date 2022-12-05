// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#define UI_CMD( CommandId, Category, FriendlyName, InDescription, CommandType, InDefaultChord )                           \
    CommandId = FUICommandInfoDecl(                                                                                       \
                this->AsShared(),                                                                                         \
                FName( TEXT( #CommandId ) ),                                                                              \
                LOCTEXT( #CommandId "Label", FriendlyName ),                                                              \
                LOCTEXT( #CommandId "ToolTip", InDescription ),                                                           \
                Category)                                                                                                 \
            .UserInterfaceType( CommandType )                                                                             \
            .DefaultChord( InDefaultChord );

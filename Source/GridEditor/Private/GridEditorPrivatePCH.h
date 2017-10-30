// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

// You should place include statements to your module's private header files here.  You only need to
// add includes for headers that are used in most of your module's source files though.
#include "IGridEditor.h"

#include "Grid.h"
#include "GridManager.h"

#include "Hexagon/HexagonGrid.h"
#include "Hexagon/HexagonGridManager.h"

#include "Square/SquareGrid.h"
#include "Square/SquareGridManager.h"
#include "Square/SquarePathFinder.h"

#define LOG_WARNING(FMT, ...) UE_LOG(GridEditor, Warning, (FMT), ##__VA_ARGS__)



#include "GridPainter/GridOutlinePrimitiveComponent.h"
#include "GridPainter/GridOutlinePainter.h"
#include "Square/SquareGridManager.h"
#include "Hexagon/HexagonGridManager.h"

const static FName NAME_GridOutlineResourceNameForDebugging(TEXT("GridOutline"));

class FGridOutlinePrimitiveSceneProxy;

namespace GridOutlineNS
{
	enum EGridType
	{
		Square,
		Hexagon
	};

	enum ESquareEdgeDirection
	{
		Left = 0,
		Right,
		Top,
		Bottom,
		Max
	};
}

struct FGridOutlinePrimitiveCompUpdateParams
{
	FGridOutlinePrimitiveSceneProxy* SceneProxy;

	GridOutlineNS::EGridType GridType;

	float GridSize;

	FLinearColor Color;

	float Thickness;

	float ZDelta;

	TMap<FIntVector, FVector> Coord2Center;
};

class FGridOutlinePrimitiveSceneProxy : public FPrimitiveSceneProxy
{
public:
	FGridOutlinePrimitiveSceneProxy(UGridOutlinePrimitiveComponent* InComponent)
		:FPrimitiveSceneProxy(InComponent, NAME_GridOutlineResourceNameForDebugging)
	{
	}

	~FGridOutlinePrimitiveSceneProxy() {}

	virtual uint32 GetMemoryFootprint(void) const override { return (sizeof(*this) + GetAllocatedSize()); }

	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_FGridOutlinePrimitiveSceneProxy_GetDynamicMeshElements);

		for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
		{
			if (VisibilityMap & (1 << ViewIndex))
			{
				const FSceneView* View = Views[ViewIndex];

				FPrimitiveDrawInterface* PDI = Collector.GetPDI(ViewIndex);
				switch (UpdateParams.GridType)
				{
				case GridOutlineNS::Square:
					DrawSquareGridOutline(PDI);
					break;
				case GridOutlineNS::Hexagon:
					DrawHexagonGridOutline(PDI);
					break;
				default:
					break;
				}
			}
		}
	}

	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override
	{
		FPrimitiveViewRelevance Result;
		Result.bDrawRelevance = IsShown(View);
		Result.bDynamicRelevance = true;
		Result.bShadowRelevance = false;
		Result.bEditorPrimitiveRelevance = UseEditorCompositing(View);
		return Result;
	}

	FVector GetSquareEdgeV0(GridOutlineNS::ESquareEdgeDirection Dir, const FVector& Center)
	{
		const float HalfGridSize = UpdateParams.GridSize / 2.f;
		switch (Dir)
		{
		case GridOutlineNS::Left:
			return Center + FVector(-HalfGridSize, -HalfGridSize, UpdateParams.ZDelta);
		case GridOutlineNS::Right:
			return Center + FVector(-HalfGridSize, HalfGridSize, UpdateParams.ZDelta);
		case GridOutlineNS::Top:
			return Center + FVector(HalfGridSize, -HalfGridSize, UpdateParams.ZDelta);
		case GridOutlineNS::Bottom:
			return Center + FVector(-HalfGridSize, -HalfGridSize, UpdateParams.ZDelta);
		default:
			LOG_ERROR(TEXT("FCompactGridPrimitiveSceneProxy::GetSquareEdgeV0 unknown direction"));
			return Center;
		}
	}

	FVector GetSquareEdgeV1(GridOutlineNS::ESquareEdgeDirection Dir, const FVector& Center)
	{
		const float HalfGridSize = UpdateParams.GridSize / 2.f;
		switch (Dir)
		{
		case GridOutlineNS::Left:
			return Center + FVector(HalfGridSize, -HalfGridSize, UpdateParams.ZDelta);
		case GridOutlineNS::Right:
			return Center + FVector(HalfGridSize, HalfGridSize, UpdateParams.ZDelta);
		case GridOutlineNS::Top:
			return Center + FVector(HalfGridSize, HalfGridSize, UpdateParams.ZDelta);
		case GridOutlineNS::Bottom:
			return Center + FVector(-HalfGridSize, HalfGridSize, UpdateParams.ZDelta);
		default:
			LOG_ERROR(TEXT("FCompactGridPrimitiveSceneProxy::GetSquareEdgeV1 unknown direction"));
			return Center;
		}
	}

	bool CanConnectSquareEdge(GridOutlineNS::ESquareEdgeDirection Dir, const FIntVector& Start, const FIntVector& End)
	{
		switch (Dir)
		{
		case GridOutlineNS::Left:
		case GridOutlineNS::Right:
			return Start.Y == End.Y && FMath::Abs(Start.X - End.X) == 1;
		case GridOutlineNS::Top:
		case GridOutlineNS::Bottom:
			return Start.X == End.X && FMath::Abs(Start.Y - End.Y) == 1;
		default:
			return false;
		}
	}

	FEdge CreateSquareEdge(GridOutlineNS::ESquareEdgeDirection Dir, const FIntVector& Start, const FIntVector& End)
	{
		FVector& StartCenter = UpdateParams.Coord2Center.FindChecked(Start);
		FVector& EndCenter = UpdateParams.Coord2Center.FindChecked(End);
		return FEdge(GetSquareEdgeV0(Dir, StartCenter), GetSquareEdgeV1(Dir, EndCenter));
	}

	void CollectSquareGridOutline()
	{
		SquareGridOutline.Reset();

		FIntVector Coord, CounterGrid;
		TArray<FIntVector> Dir2Grids[GridOutlineNS::Max];

		const GridOutlineNS::ESquareEdgeDirection CounterDirections[] = { GridOutlineNS::Right, GridOutlineNS::Left, GridOutlineNS::Bottom, GridOutlineNS::Top };
		const FIntVector CounterCoordDelta[] = { FIntVector(0, -1, 0), FIntVector(0, 1, 0), FIntVector(1, 0, 0), FIntVector(-1, 0, 0) };

		for (auto& Elem : UpdateParams.Coord2Center)
		{
			Coord = Elem.Key;

			for (int DirIndex = 0; DirIndex < 4; ++DirIndex)
			{
				auto CounterDir = CounterDirections[DirIndex];
				CounterGrid = Coord + CounterCoordDelta[DirIndex];

				if (Dir2Grids[CounterDir].Contains(CounterGrid))
				{
					Dir2Grids[CounterDir].Remove(CounterGrid);
				}
				else
				{
					Dir2Grids[DirIndex].Add(Coord);
				}
			}
		}

		FIntVector Start;
		FEdge Edge;
		for (int i = 0; i < 4; ++i)
		{
			auto Dir = (GridOutlineNS::ESquareEdgeDirection)i;
			TArray<FIntVector>& Grids = Dir2Grids[i];

			Grids.Sort([&](const FIntVector& L, const FIntVector& R)
			{
				switch (Dir)
				{
				case GridOutlineNS::Top:
				case GridOutlineNS::Bottom:
					return L.X == R.X ? L.Y < R.Y : L.X < R.X;
				case GridOutlineNS::Left:
				case GridOutlineNS::Right:
					return L.Y == R.Y ? L.X < R.X : L.Y < R.Y;
				default:
					break;
				}
				return L.X < R.X;
			});

			for (int i = 0; i < Grids.Num(); ++i)
			{
				if (i == 0)
				{
					Start = Grids[i];
				}
				else if (!CanConnectSquareEdge(Dir, Grids[i - 1], Grids[i]))
				{
					SquareGridOutline.Add(CreateSquareEdge(Dir, Start, Grids[i - 1]));
					Start = Grids[i];
				}
			}

			if (Grids.Num() != 0)
			{
				SquareGridOutline.Add(CreateSquareEdge(Dir, Start, Grids.Last()));
			}
		}
	}

	void CollectHexagonGridOutline()
	{

	}

	void DrawSquareGridOutline(FPrimitiveDrawInterface* PDI) const 
	{
		for (int i = 0; i < SquareGridOutline.Num(); ++i)
		{
			const FEdge& Edge = SquareGridOutline[i];
			PDI->DrawLine(Edge.Vertex[0], Edge.Vertex[1], UpdateParams.Color, 0, UpdateParams.Thickness);
		}

		//LOG_WARNING(TEXT("DrawSquareGridOutline edge number: %d"), SquareGridOutline.Num());
	}

	void DrawHexagonGridOutline(FPrimitiveDrawInterface* PDI) const
	{

	}

	void Update(const FGridOutlinePrimitiveCompUpdateParams& NewUpdateParams)
	{
		UpdateParams = NewUpdateParams;

		switch (UpdateParams.GridType)
		{
		case GridOutlineNS::Square:
			CollectSquareGridOutline();
			break;
		case GridOutlineNS::Hexagon:
			CollectHexagonGridOutline();
			break;
		default:
			break;
		}
	}

private:
	FGridOutlinePrimitiveCompUpdateParams UpdateParams;

	TArray<FEdge> SquareGridOutline;
};
//////////////////////////////////////////////////////////////////////////
UGridOutlinePrimitiveComponent::UGridOutlinePrimitiveComponent()
{
}

UGridOutlinePrimitiveComponent::~UGridOutlinePrimitiveComponent()
{
}

FPrimitiveSceneProxy* UGridOutlinePrimitiveComponent::CreateSceneProxy()
{
	return new FGridOutlinePrimitiveSceneProxy(this);
}

void UGridOutlinePrimitiveComponent::UpdateGridInfo()
{
	AGridManager* GridManager = GridPainter->GridManager;

	FGridOutlinePrimitiveCompUpdateParams UpdateParams;
	UpdateParams.SceneProxy = (FGridOutlinePrimitiveSceneProxy*)SceneProxy;
	UpdateParams.GridType = Cast<ASquareGridManager>(GridManager) == nullptr ? GridOutlineNS::EGridType::Hexagon : GridOutlineNS::EGridType::Square;
	UpdateParams.GridSize = GridManager->GetGridSize();
	UpdateParams.Color = GridPainter->OutlineColor;
	UpdateParams.Thickness = GridPainter->Thickness;
	UpdateParams.ZDelta = GridPainter->ZDelta;

	for (int i = 0; i < GridPainter->VisibleGrids.Num(); ++i)
	{
		UGrid* Grid = GridPainter->VisibleGrids[i];

		UpdateParams.Coord2Center.Add(Grid->GetCoord(), Grid->GetCenter());
	}

	check(UpdateParams.SceneProxy != nullptr);

	ENQUEUE_RENDER_COMMAND(GridOutlinePrimitiveCompUpdateCommand)(
		[UpdateParams](FRHICommandListImmediate& RHICmdList)
	{
		FScopeCycleCounter Context(UpdateParams.SceneProxy->GetStatId());
		UpdateParams.SceneProxy->Update(UpdateParams);
	});
}

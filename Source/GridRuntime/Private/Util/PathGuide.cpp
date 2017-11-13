#include "Util/PathGuide.h"
#include "Util/GridUtilities.h"

APathGuide::APathGuide()
{
	PrimaryActorTick.bCanEverTick = false;

	PathLineComponent = CreateDefaultSubobject<ULineComponent>("PathLineComponent");
	StartDecalComp = CreateDefaultSubobject<UDecalComponent>("StartDecalComp");
	DestDecalComp = CreateDefaultSubobject<UDecalComponent>("DestDecalComp");

	StartDecalComp->SetVisibility(false);
	StartDecalComp->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());

	DestDecalComp->SetVisibility(false);
	DestDecalComp->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());

	StartDecalMaterial = nullptr;
	DestinationDecalMaterial = nullptr;
	ZOffset = 10.f;
	PathThickness = 5.f;
	PathColor = FLinearColor::White;
	LineBreakThreshold = 50.f;

	GridType = EGridType::Unknown;
	GridSize = 100.f;
	DecalSizeScale = 0.98f;
}

APathGuide::~APathGuide()
{

}

void APathGuide::SetPath(const TArray<UGrid*>& Path)
{
	TArray<FVector> Points;

	FVector PrePoint, CurrPoint, TempPoint, Dir;
	for (int i = 0; i < Path.Num(); ++i)
	{
		const UGrid* Grid = Path[i];
		CurrPoint = Grid->GetCenter() + FVector(0.f, 0.f, ZOffset);

		if (i == 0)
		{
			Points.Add(CurrPoint);

			GridType = Grid->GridType;
			GridSize = Grid->GetGridSize();
		}
		else
		{
			if (FMath::Abs(CurrPoint.Z - PrePoint.Z) < LineBreakThreshold)
			{
				Points.Add(CurrPoint);
			}
			else
			{
				TempPoint = CurrPoint;
				TempPoint.Z = PrePoint.Z;
				Dir = TempPoint - PrePoint;
				Dir.Normalize();

				int32 Distance = FVector::Dist(PrePoint, TempPoint);

				Distance = PrePoint.Z < CurrPoint.Z ? Distance / 2 - ZOffset : Distance / 2 + ZOffset;

				TempPoint = PrePoint + Distance * Dir;
				Points.Add(TempPoint);

				TempPoint.Z = CurrPoint.Z;
				Points.Add(TempPoint);

				Points.Add(CurrPoint);
			}
		}
		PrePoint = CurrPoint;
	}

	SetCustomPoints(Points);
}

void APathGuide::SetCustomPoints(const TArray<FVector>& Points)
{
	PathLineComponent->LineColor = PathColor;
	PathLineComponent->LineThickness = PathThickness;
	PathLineComponent->SetPoints(Points);

	UpdateDecal(Points);
}

void APathGuide::SetStartDecalMaterial(UMaterialInterface* NewMaterial)
{
	StartDecalMaterial = NewMaterial;
	StartDecalComp->SetDecalMaterial(StartDecalMaterial);
	if (StartDecalMaterial == nullptr)
	{
		StartDecalComp->SetVisibility(false);
	}
}

void APathGuide::SetDestinationDecalMaterial(UMaterialInterface* NewMaterial)
{
	DestinationDecalMaterial = NewMaterial;
	DestDecalComp->SetDecalMaterial(DestinationDecalMaterial);
	if (DestinationDecalMaterial == nullptr)
	{
		DestDecalComp->SetVisibility(false);
	}
}

void APathGuide::Clear()
{
	SetPath(TArray<UGrid*>());
}

void APathGuide::UpdateDecal(const TArray<FVector>& Points)
{
	if (Points.Num() == 0)
	{
		StartDecalComp->SetVisibility(false);
		DestDecalComp->SetVisibility(false);
	}
	else
	{
		if (StartDecalMaterial != nullptr)
		{
			StartDecalComp->SetVisibility(true);
			StartDecalComp->SetDecalMaterial(StartDecalMaterial);
			StartDecalComp->SetWorldLocation(Points[0]);
			StartDecalComp->DecalSize = UGridUtilities::CalcGridDecalSize(GridType, GridSize) * DecalSizeScale;
		}
		else
		{
			StartDecalComp->SetVisibility(false);
		}
		
		if (DestinationDecalMaterial != nullptr)
		{
			DestDecalComp->SetVisibility(true);
			DestDecalComp->SetDecalMaterial(DestinationDecalMaterial);
			DestDecalComp->SetWorldLocation(Points.Last());
			DestDecalComp->DecalSize = UGridUtilities::CalcGridDecalSize(GridType, GridSize) * DecalSizeScale;
		}
		else
		{
			DestDecalComp->SetVisibility(false);
		}
	}
}

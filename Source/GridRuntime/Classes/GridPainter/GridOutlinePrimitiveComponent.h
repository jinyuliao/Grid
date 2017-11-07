#pragma once

#include "CoreMinimal.h"
#include "PrimitiveSceneProxy.h"
#include "Components/PrimitiveComponent.h"
#include "GridOutlinePrimitiveComponent.generated.h"

/**
 * 
 */
UCLASS()
class GRIDRUNTIME_API UGridOutlinePrimitiveComponent : public UPrimitiveComponent
{
	GENERATED_BODY()
	
public:
	UGridOutlinePrimitiveComponent();
	virtual ~UGridOutlinePrimitiveComponent();

	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;

	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;

	virtual void UpdateGridInfo();
};

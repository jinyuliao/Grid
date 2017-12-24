#pragma once

#include "CoreMinimal.h"
#include "GridInfo.h"
#include "GridInfo_Editor.generated.h"

UCLASS()
class UGridInfo_Editor  : public UGridInfo
{
	GENERATED_BODY()

public:
	UGridInfo_Editor();

	bool GetVisibility() const;

	bool GetShowNormal() const { return bShowNormal; }
	void SetShowNormal(bool bNewShowNormal);

	bool GetSensing() const { return bSensing; }
	void SetSensing(bool bNewSensing);

private:
	bool bShowNormal;

	bool bSensing;
};

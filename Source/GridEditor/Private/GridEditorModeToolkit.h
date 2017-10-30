#pragma once

class FEdModeGridEditor;
class SGridEditor;

class FGridEditorToolkit : public FModeToolkit
{
public:
	FGridEditorToolkit(FEdModeGridEditor* OwningMode);
	virtual ~FGridEditorToolkit();

	/** Initializes the foliage mode toolkit */
	virtual void Init(const TSharedPtr<class IToolkitHost>& InitToolkitHost) override;

	/** IToolkit interface */
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual class FEdMode* GetEditorMode() const override;
	virtual TSharedPtr<SWidget> GetInlineContent() const override;

protected:
	void OnChangeMode(FName ModeName);
	bool IsModeEnabled(FName ModeName) const;
	bool IsModeActive(FName ModeName) const;

private:
	FEdModeGridEditor* OwningMode;

	TSharedPtr<SGridEditor> GridEditorWidgets;
};


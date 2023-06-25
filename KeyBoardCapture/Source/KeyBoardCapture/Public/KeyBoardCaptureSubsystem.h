// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Application/IInputProcessor.h"
#include "Misc/OutputDevice.h"
#include "Windows/WindowsApplication.h"
#include "KeyBoardCaptureSubsystem.generated.h"


class FKeyBoardCaptureInputProcessor : public IInputProcessor
{
public:
	FKeyBoardCaptureInputProcessor() {};
	virtual ~FKeyBoardCaptureInputProcessor() {}

	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) {};

	/** Key down input */
	virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override ;

	/** Key up input */
	virtual bool HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) { return false; }
};
class FKeyBoardCaptureMessageHandler
	: public IWindowsMessageHandler
{
public:

	virtual bool ProcessMessage(HWND Hwnd, uint32 Message, WPARAM WParam, LPARAM LParam, int32& OutResult) override;
};


UCLASS()
class KEYBOARDCAPTURE_API UKeyBoardCaptureSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	/** Implement this for initialization of instances of the system */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Implement this for deinitialization of instances of the system */
	virtual void Deinitialize() override;

	void ProcessMessage(HWND Hwnd, uint32 Message, WPARAM WParam, LPARAM LParam);

	void HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent);
protected:

	FWindowsApplication* GetApplication() const
	{
		if (!FSlateApplication::IsInitialized())
		{
			return nullptr;
		}

		return (FWindowsApplication*)FSlateApplication::Get().GetPlatformApplication().Get();
	}
private:
	FKeyBoardCaptureMessageHandler Handler;

	TSharedPtr<FKeyBoardCaptureInputProcessor> BoardCaptureInputProcessor;
};

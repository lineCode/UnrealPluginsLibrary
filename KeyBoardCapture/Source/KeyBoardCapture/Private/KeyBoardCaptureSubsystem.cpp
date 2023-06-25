// Fill out your copyright notice in the Description page of Project Settings.


#include "KeyBoardCaptureSubsystem.h"



bool FKeyBoardCaptureMessageHandler::ProcessMessage(HWND Hwnd, uint32 Message, WPARAM WParam, LPARAM LParam, int32& OutResult)
{
		// log out some details for the received message
	//GLog->Logf(TEXT("WindowsMessageHandlerExampleModule: hwnd = %i, msg = %i, wParam = %i, lParam = %i"), Hwnd, Message, WParam, LParam);

	// we did not handle this message, so make sure it gets passed on to other handlers
	if (GEngine && GEngine->GameViewport)
	{
		UGameInstance* GameInstance = GEngine->GameViewport->GetGameInstance();
		if (GameInstance)
		{
			UKeyBoardCaptureSubsystem* KeyBoardSubsystem = UGameInstance::GetSubsystem<UKeyBoardCaptureSubsystem>(GameInstance);
			if (KeyBoardSubsystem)
			{
				KeyBoardSubsystem->ProcessMessage(Hwnd, Message, WParam, LParam);
			}
		}
	}
	return false;
}

bool FKeyBoardCaptureInputProcessor::HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
	if (GEngine && GEngine->GameViewport)
	{
		UGameInstance* GameInstance = GEngine->GameViewport->GetGameInstance();
		if (GameInstance)
		{
			UKeyBoardCaptureSubsystem* KeyBoardSubsystem = UGameInstance::GetSubsystem<UKeyBoardCaptureSubsystem>(GameInstance);
			if (KeyBoardSubsystem)
			{
				KeyBoardSubsystem->HandleKeyDownEvent(SlateApp,InKeyEvent);
			}
		}
	}
	return false;
}
void UKeyBoardCaptureSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	//FWindowsApplication* Application = GetApplication();

	BoardCaptureInputProcessor = MakeShareable(new FKeyBoardCaptureInputProcessor());
	/*if (Application != nullptr)
	{
		Application->AddMessageHandler(Handler);
	}*/
	FSlateApplication::Get().RegisterInputPreProcessor(BoardCaptureInputProcessor);
}

void UKeyBoardCaptureSubsystem::Deinitialize()
{
	Super::Deinitialize();
	// unregister our handler
	/*FWindowsApplication* Application = GetApplication();

	if (Application != nullptr)
	{
		Application->RemoveMessageHandler(Handler);
	}*/
	FSlateApplication::Get().UnregisterInputPreProcessor(BoardCaptureInputProcessor);
	BoardCaptureInputProcessor.Reset();
}

void UKeyBoardCaptureSubsystem::ProcessMessage(HWND Hwnd, uint32 Message, WPARAM WParam, LPARAM LParam)
{
}

void UKeyBoardCaptureSubsystem::HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
	GLog->Logf(TEXT("FKeyBoardCaptureInputProcessor: FKeyEvent KeyName= %s"), *InKeyEvent.GetKey().GetFName().ToString());
}

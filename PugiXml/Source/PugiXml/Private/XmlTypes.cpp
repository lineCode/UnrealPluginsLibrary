#include "XmlTypes.h"
#include "Misc/FileHelper.h"

#if WITH_EDITOR
#include "DirectoryWatcherModule.h"
#include "IDirectoryWatcher.h"
#endif


UXmlDocument* UXmlDocument::Make(UObject* WorldContextObject, FString XmlPath)
{
	for (TObjectIterator<UXmlDocument> It; It; ++It)
	{
		if (It->GetWorld() == WorldContextObject->GetWorld() && It->SourceFile.FilePath == XmlPath) {
			return *It;
		}
	}

	auto Ret = NewObject<UXmlDocument>(WorldContextObject);
	Ret->Document = pugi::Load(XmlPath);
	Ret->SourceFile.FilePath = XmlPath;
	Ret->bExternal = 1;

#if WITH_EDITOR
	FDirectoryWatcherModule& Module = FModuleManager::LoadModuleChecked<FDirectoryWatcherModule>(TEXT("DirectoryWatcher"));
	if (auto Watcher = Module.Get())
	{
		auto Callback = IDirectoryWatcher::FDirectoryChanged::CreateUObject(Ret, &UXmlDocument::Private_OnFileChanged);
		Watcher->RegisterDirectoryChangedCallback_Handle(FPaths::GetPath(FPaths::ConvertRelativePathToFull(XmlPath)), Callback, Ret->OnFileChangedHanle);
	}
#endif

	return Ret;
}

FXmlNode UXmlDocument::Child(FString XPath)
{
	if (XPath.IsEmpty())return FXmlNode();

	auto Result = Document.select_node(*XPath);
	return FXmlNode{ Result.node() };
}

TArray<FXmlNode> UXmlDocument::Children(FString XPath)
{
	TArray<FXmlNode> Ret;
	if (XPath.IsEmpty())return Ret;

	auto Result = Document.select_nodes(*XPath);
	for (auto& N : Result)
	{
		Ret.Add(FXmlNode{ N.node() });
	}
	return MoveTemp(Ret);
}

void UXmlDocument::ReloadContent()
{
	if (!SourceFile.FilePath.IsEmpty()) 
	{
		Document = pugi::Load(FullFilePath());
		Content = pugi::Source(Document);
	}
	else {
		Content = FString();
	}
}


void UXmlDocument::PostLoad()
{
	Super::PostLoad();
	if (bExternal) 
	{
		ReloadContent();
#if WITH_EDITOR
		RegistDirectoryChangedCallback(this);
#endif
	}
	ReloadDocument();
}

FString UXmlDocument::FullFilePath()
{
	FString FullPath = FPaths::ConvertRelativePathToFull(SourceFile.FilePath);
	if (!FPaths::FileExists(FullPath))
	{
		FullPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / SourceFile.FilePath);
	}
	return MoveTemp(FullPath);
}

void UXmlDocument::ReloadDocument()
{
	if (bExternal) 
	{
		Document = pugi::Load(FullFilePath());
	}
	else if (!Content.IsEmpty())
	{
		Document.load_string(*Content);
	}
}


#if WITH_EDITOR
void UXmlDocument::PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);
	auto Name = PropertyChangedEvent.GetPropertyName();

	if (Name == GET_MEMBER_NAME_CHECKED(FFilePath, FilePath) || Name == GET_MEMBER_NAME_CHECKED(UXmlDocument, SourceFile)) 
	{
		RegistDirectoryChangedCallback(this);
		ReloadContent();
	}
}

void UXmlDocument::RegistDirectoryChangedCallback(UXmlDocument* Dodument, bool bRegist)
{
	FDirectoryWatcherModule& Module = FModuleManager::LoadModuleChecked<FDirectoryWatcherModule>(TEXT("DirectoryWatcher"));
	if (auto Watcher = Module.Get())
	{
		if (Dodument->OnFileChangedHanle.IsValid())
		{
			Watcher->UnregisterDirectoryChangedCallback_Handle(FPaths::GetPath(Dodument->FullFilePath()), Dodument->OnFileChangedHanle);
		}

		if (bRegist)
		{
			auto Callback = IDirectoryWatcher::FDirectoryChanged::CreateUObject(this, &UXmlDocument::Private_OnFileChanged);
			Watcher->RegisterDirectoryChangedCallback_Handle(FPaths::GetPath(Dodument->FullFilePath()), Callback, Dodument->OnFileChangedHanle);
		}
	}
}

void UXmlDocument::Private_OnFileChanged(const TArray<struct FFileChangeData>& InData)
{
	bool Modify = false;
	auto FullPath = this->FullFilePath();
	for (auto& D : InData)
	{
		if (D.Filename == FullPath) {
			Modify = true;
		}
	}
	if (Modify)
	{
		if (bExternal) {
			ReloadContent();
		}
		ReloadDocument();
		OnFileChanged.Broadcast();
	}
}

void UXmlDocument::BeginDestroy()
{
	Super::BeginDestroy();
	FDirectoryWatcherModule& Module = FModuleManager::LoadModuleChecked<FDirectoryWatcherModule>(TEXT("DirectoryWatcher"));
	RegistDirectoryChangedCallback(this, false);
}
#endif
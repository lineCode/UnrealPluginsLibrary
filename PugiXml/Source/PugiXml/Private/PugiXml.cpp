// Fill out your copyright notice in the Description page of Project Settings.

#include "PugiXml.h"
#include "Misc/FileHelper.h"

#if WITH_EDITOR
#include "PropertyEditing.h"
#include "XmlDocumentDetails.h"
#endif

DEFINE_LOG_CATEGORY(PugiXml);

#define LOCTEXT_NAMESPACE "FPugiXml"

void FPugiXml::StartupModule()
{
#if WITH_EDITOR
	auto& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomClassLayout("XmlDocument", FOnGetDetailCustomizationInstance::CreateStatic(&FXmlDocumentDetails::MakeInstance));
#endif
}

void FPugiXml::ShutdownModule()
{
	
}


namespace pugi
{
	class xml_append : public xml_writer
	{
	public:
		xml_append(xml_node& Parent) : Node(Parent) {}

		~xml_append() {
			Node.append_buffer(String.c_str(), String.size());
		}

		virtual void write(const void* data, size_t size) override
		{
			String.append(reinterpret_cast<const char*>(data), size);
		}
		std::string String;
		xml_node& Node;
	};

	static void IncludeDocument(xml_node& Parent, const xml_document& Document) {
		if (auto Ele = Document.document_element()) {
			for (auto& C : Ele.children())
			{
				xml_append Append(Parent);
				C.print(Append);
			}
		}
	}

	static FString LoadFile(const FString& Path)
	{
		FString Ret;
		if (!FFileHelper::LoadFileToString(Ret, *Path)) {
			UE_LOG(PugiXml, Error, L"Load File Error : %s", *Path);
		}
		return Ret;
	}

	static xml_document LoadImpl(const FString& Path, bool bInclude, bool PrintLog = false)
	{
		auto Folder = FPaths::GetPath(Path);

		TFunction<void(xml_node&)> ForeachNodes;
		ForeachNodes = [&](xml_node& Parent) {
			for (auto& C : Parent.children())
			{
				if (C.name() == FString("include")) {
					auto Full = Folder / C.attribute(PUGIXML_TEXT("path")).as_string();
					xml_document Doc;
					if (!Doc.load_string(*LoadFile(Full))) return;
					IncludeDocument(Parent, Doc);
					Parent.remove_child(C);
				}
				else {
					ForeachNodes(C);
				}
			}
		};
		
		auto Str = LoadFile(Path);
		if (PrintLog)UE_LOG(LogTemp, Log, L"Load xml %s\n%s", *Path, *Str);
		xml_document Doc;
		if (!Doc.load_string(*Str)) return Doc;

		auto Root = Doc.document_element();
		if(bInclude)ForeachNodes(Root);

		Root.append_attribute(PUGIXML_TEXT("path")) = *Path;

		return MoveTemp(Doc);
	}


	xml_document LoadAndCombine(const FString& Folder)
	{
		xml_document Ret;
		Ret.load_string(PUGIXML_TEXT("<root></root>"));

		auto Root = Ret.document_element();

		if (FPaths::DirectoryExists(Folder)) {
			TArray<FString> OutPath;
			IFileManager::Get().FindFiles(OutPath, *Folder, L"xml");
			for (auto& P : OutPath)
			{
				auto FullPath = Folder / P;
				IncludeDocument(Root, LoadImpl(FullPath, true));
			}
		}
		Root.append_attribute(PUGIXML_TEXT("path")) = *Folder;
		return MoveTemp(Ret);
	}


	xml_document Load(const FString& Path, bool bInclude, bool PrintLog)
	{
		return LoadImpl(Path, bInclude, PrintLog);
	}

	class xml_source_reader : public xml_writer
	{
	public:
		virtual void write(const void* data, size_t size) override
		{
			auto str = std::string(static_cast<const char*>(data), size);
			Source += UTF8_TO_TCHAR(str.c_str());
		}
		FString Source;
	};

	FString Source(const xml_node& InNode) {
		
		xml_source_reader Reader;
		for (auto& Item : InNode.children())
		{
			Item.print(Reader);
		}
		return Reader.Source;
	}

	FString Source(const xml_document& Document)
	{
		xml_source_reader Reader;
		Document.save(Reader);
		return Reader.Source;
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FPugiXml, PugiXml);
#include "SubSystem\RenderManager.h"


GLib::Sprites::Sprite * CreateSprite(const char * i_pFilename)
{
	SLOW_ASSERT(i_pFilename != nullptr, ErrorType::ENullPointer);

	size_t sizeTextureFile = 0;

	// Load the source file (texture data)
	void * pTextureFile = LoadFile(i_pFilename, sizeTextureFile);

	// Ask GLib to create a texture out of the data (assuming it was loaded successfully)
	GLib::Texture * pTexture = pTextureFile ? GLib::CreateTexture(pTextureFile, sizeTextureFile) : nullptr;

	// exit if something didn't work
	// probably need some debug logging in here!!!!
	if (pTextureFile)
		delete[] pTextureFile;

	if (pTexture == nullptr)
		return NULL;

	uint32 width = 0;
	uint32 height = 0;
	uint32 depth = 0;

	// Get the dimensions of the texture. We'll use this to determine how big it is on screen
	bool result = GLib::GetDimensions(pTexture, width, height, depth);
	ASSERT(result == true);
	ASSERT((width > 0) && (height > 0));

	// Define the sprite edges
	GLib::Sprites::SpriteEdges	Edges = { -float(width / 2.0f), float(height), float(width / 2.0f), 0.0f };
	GLib::Sprites::SpriteUVs	UVs = { { 0.0f, 0.0f },{ 1.0f, 0.0f },{ 0.0f, 1.0f },{ 1.0f, 1.0f } };
	GLib::RGBA							Color = { 255, 255, 255, 255 };

	// Create the sprite
	GLib::Sprites::Sprite * pSprite = GLib::Sprites::CreateSprite(Edges, 0.1f, Color, UVs);
	if (pSprite == nullptr)
	{
		GLib::Release(pTexture);
		return nullptr;
	}

	// Bind the texture to sprite
	GLib::Sprites::SetTexture(*pSprite, *pTexture);

	return pSprite;
}

void * LoadFile(const char * i_pFilename, size_t & o_sizeFile)
{
	SLOW_ASSERT(i_pFilename != NULL, ErrorType::ENullPointer);

	FILE * pFile = NULL;

	errno_t fopenError = fopen_s(&pFile, i_pFilename, "rb");
	if (fopenError != 0)
		return NULL;

	SLOW_ASSERT(pFile != NULL, ErrorType::ENullPointer);

	int32 FileIOError = fseek(pFile, 0, SEEK_END);
	SLOW_ASSERT(FileIOError == 0, ErrorType::ENonZeroValue);

	long FileSize = ftell(pFile);
	SLOW_ASSERT(FileSize >= 0, ErrorType::ENonPostiveValue);

	FileIOError = fseek(pFile, 0, SEEK_SET);
	SLOW_ASSERT(FileIOError == 0, ErrorType::ENonZeroValue);

	uint8 * pBuffer = new uint8[FileSize];
	SLOW_ASSERT(pBuffer, ErrorType::ENullPointer);

	size_t FileRead = fread(pBuffer, 1, FileSize, pFile);
	ASSERT(FileRead == FileSize);

	fclose(pFile);

	o_sizeFile = FileSize;

	return pBuffer;
}



RenderManager *RenderManager::globalInstance_ = nullptr;

RenderManager::~RenderManager()
{
	//renderObjectList_.Clear();
	for (auto it = spriteResources_.begin();it != spriteResources_.end(); ++it)
	{
		GLib::Sprites::Release(it->second);
	}
	spriteResources_.clear();
}

void RenderManager::RenderUpdate() const
{
	GLib::BeginRendering();
	GLib::Sprites::BeginRendering();

	LinkedListNode<StrongPtr<RenderObject>> *temp = renderObjectList_.Head();
	LinkedListNode<StrongPtr<RenderObject>> *recordNext;
	while (temp != nullptr)
	{
		recordNext = temp->Next;				// temp may be delete in DoRender
		temp->Data->DoRender();                 
		temp = recordNext;
	}

	GLib::Sprites::EndRendering();
	GLib::EndRendering();
}

StrongPtr<RenderObject>& RenderManager::AddRenderObject(const StrongPtr<GameObject> &i_gameObject, const char *i_filePath)
{
	GLib::Sprites::Sprite *tempSprite;

	HashedString filePathHash = HashedString(i_filePath);
	// check if sprite exist, if not create one and store it in map
	auto it = spriteResources_.find(filePathHash.GetHash());
	if (it == spriteResources_.end())
	{
		tempSprite = CreateSprite(i_filePath);
		spriteResources_.insert(std::pair<uint32, GLib::Sprites::Sprite*>(filePathHash.GetHash(), tempSprite));
	}
	else
	{
		tempSprite = it->second;
	}
	StrongPtr<RenderObject> newRenderObject = new RenderObject(i_gameObject, tempSprite);
	renderObjectList_.PushHead(newRenderObject);
	return renderObjectList_.Head()->Data;
}

void RenderManager::RemoveFromList(RenderObject &i_renderObject)
{
	LinkedListNode<StrongPtr<RenderObject>> *temp = renderObjectList_.Head();
	if (temp == nullptr) return;

	if (temp->Data == &i_renderObject)
	{
		renderObjectList_.PopHead();         // Pop include delete
	}
	else
	{
		while (temp->Next != nullptr)
		{
			if (temp->Next->Data == &i_renderObject)
			{
				renderObjectList_.DeleteNext(temp);
				break;
			}
			temp = temp->Next;
		}
	}
}

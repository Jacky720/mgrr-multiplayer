#define SHARED_USE_EX_FUNCS

#include <Events.h>
#include <cGameUIManager.h>
//#include <ObjReadManager.h>
#include <EntitySystem.h>
#include <PlayerManagerImplement.h>
#include <shared.h>
#include "ModelItems.h"
#include <map>

extern ModelItems* modelItems;
extern unsigned int *modelSword;
std::map<int, std::vector<int>> childObjs = { { 0x10010, {0x32040} }, // Empty list needed for modelItems load
	                                          { 0x11400, {0x3D070} },
											  { 0x11500, {0x11503, 0x11504, 0x11506} } };

// cObjReadManager
struct cObjReadManager
{
	eObjID m_nObjId;
	int m_nSetType;
	int m_nTimeout;

	BOOL loadRequestedObject(eObjID objId, int setType)
	{
		return ((BOOL(__thiscall*)(cObjReadManager*, eObjID, int))(shared::base + 0x600CA0))(this, objId, setType);
	}

	BOOL requestWork(eObjID objId, int setType)
	{
		return ((BOOL(__thiscall*)(cObjReadManager*, eObjID, int))(shared::base + 0x600A60))(this, objId, setType);
	}

	BOOL endWork(eObjID objId, int setType)
	{
		return ((BOOL(__thiscall*)(cObjReadManager*, eObjID, int))(shared::base + 0x600BD0))(this, objId, setType);
	}

	void getDataAtSet(void* file, eObjID objId, int unknown)
	{
		((void(__thiscall*)(cObjReadManager*, void*, eObjID, int))(shared::base + 0x601170))(this, file, objId, unknown);
	}

	static inline cObjReadManager& Instance = *(cObjReadManager*)(shared::base + 0x0177B364);
};

// DatHolder
struct DatHolder
{
	char* m_data;

	void* getFile(const char* name)
	{
		((void* (__thiscall*)(DatHolder*, const char*))(shared::base + 0x9E4500))(this, name);
	}

	operator DataArchiveEntry* ()
	{
		return (DataArchiveEntry*)m_data;
	}

	DataArchiveEntry* asEntry()
	{
		return (DataArchiveEntry*)m_data;
	}

	size_t getAnyFileIndex(const char* filename, unsigned int matchLimit)
	{
		return ((size_t(__thiscall*)(DatHolder*, const char*, unsigned int))(shared::base + 0x9E4130))(this, filename, matchLimit);
	}

	size_t getSize(unsigned int index)
	{
		return ((int(__thiscall*)(DatHolder*, unsigned int))(shared::base + 0x9E3670))(this, index);
	}

	size_t getPosition(const char* filename)
	{
		auto index = getAnyFileIndex(filename, 0);

		if (index != -1) {
			return *(int*)(m_data + asEntry()->m_nPositionOffset + index * 4);
		}

		return 0;
	}

	void* getFile(unsigned int index)
	{
		return ((void* (__thiscall*)(DatHolder*, unsigned int))(shared::base + 0x9E3CF0))(this, index);
	}

	void* getAnyFile(const char* filename, unsigned int matchLimit)
	{
		return ((void* (__thiscall*)(DatHolder*, const char*, unsigned int))(shared::base + 0x9E4550))(this, filename, matchLimit);
	}

	const char* getNameByFileIndex(unsigned int index)
	{
		return ((const char* (__thiscall*)(DatHolder*, unsigned int))(shared::base + 0x9E38D0))(this, index);
	}

	size_t getFileIndexByExtension(int bigEndian, const char* extension, unsigned int matchLimit)
	{
		return ((size_t(__thiscall*)(DatHolder*, int, const char*, unsigned int))(shared::base + 0x9E36A0))(this, bigEndian, extension, matchLimit);
	}

	void* getFiledataByExtension(int bigEndian, const char* extension, unsigned int matchLimit)
	{
		return ((void* (__thiscall*)(DatHolder*, int, const char*, unsigned int))(shared::base + 0x9E3D30))(this, bigEndian, extension, matchLimit);
	}

	void* getFiledataByExtension(const char* suffix, int unknown)
	{
		return ((void* (__thiscall*)(DatHolder*, const char*, int))(shared::base + 0x9E44B0))(this, suffix, unknown);
	}
};

bool isObjExists(eObjID objId)
{
	char buffer[128];

	char* folder = nullptr;
	std::pair<int, char*>* pFolderNames = (std::pair<int, char*>*)(shared::base + 0x1490198);

	for (int i = 0; i < 12; i++)
	{
		if (pFolderNames[i].first == (objId & 0xF0000))
		{
			folder = pFolderNames[i].second;
			break;
		}
	}

	sprintf(buffer, "%s\\%s%04x.dat", folder, folder, objId & 0x0000FFFF);

	return ((BOOL(__cdecl*)(const char*))(shared::base + 0x9EC390))(buffer) == TRUE;
}

struct EntSpawn
{
	bool bDone = false;
	eObjID mObjId = eObjID(0);
	int iSetType = 0;
	bool bWorkFail = false;

	Entity* m_Entity = nullptr;
};

class EntSpawnQueue : public lib::StaticArray<EntSpawn, 16>
{
public:

	Entity* getLastEntity();
};

EntSpawnQueue m_EntQueue;

Entity* EntSpawnQueue::getLastEntity()
{
	EntSpawn* queue = nullptr;

	for (auto& str : *this)
	{
		if (str.bDone && !str.bWorkFail && str.m_Entity)
			queue = &str;
	}

	Entity* resultEntity = nullptr;

	if (queue)
		resultEntity = queue->m_Entity;

	if (queue)
		remove(*queue);

	return resultEntity;
}

class Spawner
{
public:

	Spawner()
	{
		Events::OnUpdateEvent.after += []()
			{
				if (m_EntQueue.m_size)
				{
					std::map<eObjID, unsigned int> loadedIDs;
					void* block = ((Hw::cHeap*)(shared::base + 0x19DA8E8))->getBlock(nullptr);
					while (block) {
						loadedIDs.emplace(((eObjID*)block)[2], ((int*)block)[1]);
						Hw::cHeap* allocator = ((Hw::cHeap**)block)[-1];
						block = allocator->getBlock(block);
					}

					for (auto& str : m_EntQueue)
					{
						if (str.bDone || str.bWorkFail)
							continue;

						str.bWorkFail = cObjReadManager::Instance.requestWork(str.mObjId, str.iSetType) == 0;

						if (str.bWorkFail)
							continue;

						std::vector<unsigned int> children2;
						if (childObjs.contains(str.mObjId)) {
							for (auto& id2 : childObjs[str.mObjId])
								children2.push_back(id2);

							if (modelSword) children2.push_back(*modelSword);
							if (modelItems) {
								children2.push_back(modelItems->m_nModel);
								children2.push_back(modelItems->m_nHair);
								children2.push_back(modelItems->m_nVisor);
								children2.push_back(modelItems->m_nSheath);
								children2.push_back(modelItems->m_nHead);
							}
						}

						// Check readiness (some submodels may not be loaded yet and still risk crashing)
						bool ready = true;
						for (auto& id2 : children2) {
							if (id2 == 0 || id2 == -1)
								continue;
							if (!loadedIDs.contains((eObjID)id2)) {
								ready = false;
								str.bWorkFail |= cObjReadManager::Instance.requestWork((eObjID)id2, 0) == 0;
							}
							else
								ready &= (loadedIDs.at((eObjID)id2) & 4) != 0 && (loadedIDs.at((eObjID)id2) & 9) != 1;
						}

						if (ready) {
							int trueSetType = (str.mObjId == (eObjID)0x10010) ? str.iSetType : 0;
							str.bDone = cObjReadManager::Instance.loadRequestedObject(str.mObjId, trueSetType);
						}

					}

					for (unsigned int i = 0; i < m_EntQueue.m_size; i++)
					{
						auto& elem = m_EntQueue[i];

						if (elem.bWorkFail)
							m_EntQueue.remove(elem);
						
						else if (elem.bDone && !elem.m_Entity) {
							if (elem.mObjId == 0x10010 || elem.mObjId == 0x11400 || elem.mObjId == 0x11500) {
								EntitySystem::ObjectInfo myObject = EntitySystem::ObjectInfo();
								EntitySystem::EntityInfo myEntity = EntitySystem::EntityInfo();
								DatHolder myFile[2] = { 0, 0 };
								static char playerNameString[15] = "SpawnedPlayer";
								myEntity.m_ModelIndex = elem.mObjId;
								myEntity.m_ObjectIndex = elem.mObjId;
								myEntity.m_ObjectInfo = &myObject;
								myEntity.m_Name = playerNameString;
								myEntity.field_10 = 0;
								cObjReadManager::Instance.getDataAtSet(myFile, (eObjID)modelItems->m_nModel, 0);
								void* wmb = myFile->getFiledataByExtension("wmb", 0);
								myEntity.m_ModelData = (void*)((DWORD(__cdecl*)(void*, int))(shared::base + 0x619920))(wmb, 0);
								myEntity.m_Param = myFile->getAnyFile("_param.bxm", 0);
								void* wtb = myFile->getFiledataByExtension("wtb", 0);
								void* wta = myFile->getFiledataByExtension("wta", 0);
								void* wtp = myFile->getFiledataByExtension("wtp", 0);
								if (wtb) {
									myEntity.m_WtbFile = wtb;
									myEntity.m_TexturesFile = 0;
								}
								else {
									myEntity.m_WtbFile = wtp;
									myEntity.m_TexturesFile = wta;
								}

								elem.m_Entity = EntitySystem::ms_Instance.createEntity(&myEntity);

								if (elem.iSetType == 2) {
									Pl0000* newPlayer = elem.m_Entity->getEntityInstance<Pl0000>();
									//newPlayer->field_13F4 = 0;
									//newPlayer->field_13F8 = 1;
									newPlayer->setSwordLost(true);
									newPlayer->m_SwordState = 1;
									*selectedCustomWeapon = &validCustomWeapons[CustomWeapons::Unarmed];
									((void(__thiscall*)(Pl0000*))(shared::base + 0x7948D0))(newPlayer); // Pl0000::RebuildCustomWeapon
									PlayerManagerImplement::ms_Instance->setCustomWeaponEquipped(5); // Unarmed
									newPlayer->setIdle(0);

									newPlayer->toggleAnyMesh("skin_in", false);
									newPlayer->toggleAnyMesh("saya_arm", false);
									Behavior* newSheath = newPlayer->m_SheathHandle.getEntity()->m_pInstance;
									newSheath->toggleAnyMesh("equip_sheath", false);
									newSheath->toggleAnyMesh("connect", false);
								}

							}
							else {
								EntitySystem::ObjectInfo objInfo;
								objInfo.m_nSetType = elem.iSetType;
								objInfo.m_vecSize = { 1.0, 1.0, 1.0 };
								elem.m_Entity = EntitySystem::ms_Instance.createEntity("SpawnedObject", elem.mObjId, &objInfo);
							}
						}
					}
				}

				if (auto entity = m_EntQueue.getLastEntity(); entity)
				{
					auto instance = entity->getEntityInstance<Behavior>();

					//cObjReadManager::Instance.requestWork((eObjID)0x11400, instance->m_nSetType);

					auto pos = cGameUIManager::Instance.m_pPlayer ? cGameUIManager::Instance.m_pPlayer->m_vecTransPos : cVec4();
					auto rot = cGameUIManager::Instance.m_pPlayer ? cGameUIManager::Instance.m_pPlayer->m_vecRotation : cVec4();

					instance->place(pos, rot);

					cObjReadManager::Instance.endWork(instance->m_ObjId, instance->m_nSetType);
				}

				//shared::ExPressKeyUpdate();
			};

		Events::OnTickEvent += []()
			{
				//if (shared::IsKeyPressedEx('H', false)) // spawn boss Sam for example
					//m_EntQueue.push_back({ .mObjId = (eObjID)0x20020, .iSetType = 0, .bWorkFail = !isObjExists(.mObjId) });
			};
	}
} __spawner;
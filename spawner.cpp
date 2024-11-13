#define SHARED_USE_EX_FUNCS

#include <Events.h>
#include <cGameUIManager.h>
#include <EntitySystem.h>
#include <shared.h>

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

	static inline cObjReadManager& Instance = *(cObjReadManager*)(shared::base + 0x0177B364);
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
				if (m_EntQueue.m_nSize)
				{
					for (auto& str : m_EntQueue)
					{
						if (!str.bDone)
						{
							if (!str.bWorkFail)
								str.bWorkFail = cObjReadManager::Instance.requestWork(str.mObjId, str.iSetType) == 0;

							str.bDone = cObjReadManager::Instance.loadRequestedObject(str.mObjId, str.iSetType);
						}
					}

					for (int i = 0; i < m_EntQueue.m_nSize; i++)
					{
						auto& elem = m_EntQueue[i];

						if (elem.bWorkFail)
							m_EntQueue.remove(elem);
						
						else if (elem.bDone && !elem.m_Entity)
							elem.m_Entity = EntitySystem::Instance.createEntity("SpawnedObject", elem.mObjId, nullptr);
					}
				}

				if (auto entity = m_EntQueue.getLastEntity(); entity)
				{
					auto instance = entity->getEntityInstance<Behavior>();

					//cObjReadManager::Instance.requestWork((eObjID)0x11400, instance->m_nSetType);

					auto pos = cGameUIManager::Instance.m_pPlayer ? cGameUIManager::Instance.m_pPlayer->m_vecTransPos : cVec4();
					auto rot = cGameUIManager::Instance.m_pPlayer ? cGameUIManager::Instance.m_pPlayer->m_vecRotation : cVec4();

					instance->place(pos, rot);

					cObjReadManager::Instance.endWork(instance->m_nObjId, instance->m_nSetType);
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
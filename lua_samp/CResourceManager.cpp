#include "CResourceManager.h"
#include "CCore.h"

CResourceManager::CResourceManager(const char* rN)
{
	r_resourceName = rN;
	r_isRunning = false;
	//r_isActive = false;
	r_isScriptsValid = false;
}

CResourceManager::~CResourceManager()
{
	s_Core->DeleteResource(r_resourceName.c_str());
}

bool CResourceManager::IsValidFile(std::string szFileScript)
{
	std::string file(LUA_RESOURCES_FOLDER + szFileScript + ".lua");
	std::ifstream fileScript(file);
	if (fileScript.good())
	{
		fileScript.close();
		return true;
	}
	else
	{
		fileScript.close();
		return false;
	}
}

int CResourceManager::LoadResource()
{	
	CLuaManager *luaScript = new CLuaManager(r_resourceName);

	bool IsValid = IsValidFile(r_resourceName);

	if (IsValid) {
		luaScript->AddFile(r_resourceName);
	}
	else
	{
		CUtility::printf("Unable to load the file: %s (The file does not exist or does not contain the extension \".lua\")", r_resourceName.c_str()
		delete luaScript;
		return 2;
	}

	r_validScript = luaScript;
	r_isScriptsValid = true;

	return 4;
}

void CResourceManager::StartResource(void)
{
	r_validScript->StartLua();
	s_Core->RegisterLuaVM(r_validScript->GetVirtualMachine(), this);

	//r_isActive = true;
	r_isRunning = true;
}

void CResourceManager::StopResource(void)
{
	std::map<lua_State *, CResourceManager *> *tempContainer = s_Core->GetLuaResourceContainer();
	for (std::map<lua_State *, CResourceManager *>::iterator it = (*tempContainer).begin(); it != (*tempContainer).end(); ++it)
	{
		if (it->second == this)
		{
			CLuaManager *luaManager = s_Core->GetLuaManager(it->first);
			//CUtility::printf("%d", luaManager->GetVirtualMachine());

			auto *tempContainer = s_Core->GetModuleContainer();
			for (auto it = (*tempContainer).begin(); it != (*tempContainer).end(); ++it)
			{
				(*it)->_ResourceStopping(luaManager->GetVirtualMachine());
			}
			lua_close(luaManager->GetVirtualMachine());

			delete luaManager;
			s_Core->DeleteLuaVM(it->first);
			s_Core->DeleteReference(it->first);
			break;
		}
	}

	delete this;
}

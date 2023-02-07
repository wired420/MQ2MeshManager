#pragma once
#pragma comment(lib, "lib/libcurl.lib")
#include <string>
#include <filesystem>
#include <mq/Plugin.h>

namespace fs = std::filesystem;
/**
 * PROTOTYPES
 *
 * Putting my code in order like a jigsaw puzzle got annoying at some point. Soooo
 * Here we go. Prototyping. Screw you code order!
 *
 */
void MeshLoadDatabase();
void MeshWriteChat(const std::string& msg, bool AntiSpam);
void MeshManager(SPAWNINFO* pChar, char* szLine);
void MeshDownloadFile(const std::string& url, const std::string& filename, const std::string& savepath, bool single);
void MeshManagerMenu(const std::string& menu);
void MeshUpdateDatabase();
void MeshManagerDisplayHashes(const char* Param2);
void MeshManagerUpdateZone(const char* Param2);
void MeshManagerUpdateAll(const char* Param2, const char* Param3);
void MeshManagerSaveSettings();
void MeshManagerLoadSettings();
void MeshManagerSaveIgnores();
void MeshManagerLoadIgnores();
bool InGameAndSpawned();
std::string tolower(const std::string& msg);
bool is_number(const std::string& s);
void Get_Hash_For_Update(const struct HashListStorage& tmp);
std::string Get_Hash(const fs::path& p, const std::string& h);
std::string MD5(const std::string& data);
std::string SHA256(const std::string& data);
struct DownloadListStorage;
struct HashListStorage;

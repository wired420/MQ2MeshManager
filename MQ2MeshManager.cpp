/**
 * TODO LIST (Overall)
 *
 * + MAKE A UI!
 *
 * + Build an ignore list. Meshes you don't want over written. Make it for all characters.
 *
 */
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
 // MQ2MeshManager.cpp : Defines the entry point for the DLL application.
 //

 // PLUGIN_API is only to be used for callbacks.  All existing callbacks at this time
 // are shown below. Remove the ones your plugin does not use.  Always use Initialize
 // and Shutdown for setup and cleanup.
#include <bitset>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <regex>
#include <string>
#include <thread>

#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <Windows.h>

// Json Parser
#include <nlohmann/json.hpp>

// Downloader
#include <curl/curl.h>

// Hashing Function
#include <cryptopp/files.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <cryptopp/md5.h>
#include <cryptopp/sha.h>

// MQ Stuff
#include <mq/Plugin.h>
#include "Prototypes.h"

using json = nlohmann::json;
namespace fs = std::filesystem;

PreSetup("MQ2MeshManager");
PLUGIN_VERSION(1.0);

/**
 * GLOBALS
 *
 * Avoid Globals if at all possible, since they persist throughout your program.
 * But if you must have them, here is the place to put them.
 */
static const std::string MeshPluginVersion = "1.0b";
bool fAgree = false;
static std::chrono::steady_clock::time_point PulseTimer = std::chrono::steady_clock::now();
static std::chrono::steady_clock::time_point DownloadThreadTimer = std::chrono::steady_clock::now();
static std::chrono::steady_clock::time_point HashThreadTimer = std::chrono::steady_clock::now();
unsigned short DownloadThreadCount = 0;
unsigned short HashThreadCount = 0;
std::string fCurrentDL = "none", fLastDL = "none", fPathDL = "none", LastMsg = "none";
unsigned short fProgressDL = 0;
bool fDownloading = false;
std::list<DownloadListStorage> DownloadList;
std::list<HashListStorage> HashpipeList;
std::vector<std::string> IgnoreList;
bool fDownloadReady = false;
bool fHashReady = false;
unsigned short int HideProgress = 1;
fs::path navPath = fs::path(gPathResources) / "MQ2Nav";
unsigned short DownloadThreads = 0, MaxDownloadThreads = 4;
unsigned short HashThreads = 0, MaxHashThreads = 4;
bool ThreadSafe = true;
int ThreadLockSeconds = 20;
bool AutoDownloadMissing = false, AutoCheckForUpdates = false;
bool _init = false;
int MaxZone = 546;
std::string Zones[546] = { "aalishai", "abysmal", "acrylia", "airplane", "akanon", "akheva", "akhevatwo", "alkabormare", "anguish", "arcstone", "arelis", "arena", "argath", "arginhiz", "arthicrex", "arxmentis", "ashengate", "atiiki", "barindu", "barren", "basilica", "bazaar", "beastdomain", "befallen", "beholder", "bertoxtemple", "bixiewarfront", "blackburrow", "blacksail", "bloodfalls", "bloodfields", "bloodmoon", "bothunder", "breedinggrounds", "brellsarena", "brellsrest", "brellstemple", "broodlands", "brotherisland", "buriedsea", "burningwood", "butcher", "cabeast", "cabwest", "cauldron", "causeway", "cazicthule", "chamberoftears", "chambersa", "chambersb", "chambersc", "chambersd", "chamberse", "chambersf", "chapterhouse", "charasisb", "charasis", "charasistwo", "chardokb", "chardok", "chardoktwo", "chelsithreborn", "citymist", "cityofbronze", "cobaltscar", "cobaltscartwo", "codecayb", "codecay", "commonlands", "convorteum", "coolingchamber", "corathusa", "corathusb", "corathus", "cosul", "crescent", "crushbone", "cryptofshade", "crystallos", "crystal", "crystalshard", "crystaltwoa", "crystaltwob", "dalnir", "darklightcaverns", "dawnshroud", "deadbone", "deadhills", "deepshade", "degmar", "delvea", "delveb", "devastationa", "devastation", "direwind", "discord", "discordtower", "drachnidhivea", "drachnidhiveb", "drachnidhivec", "drachnidhive", "dragoncrypt", "dragonscalea", "dragonscaleb", "dragonscale", "dranikcatacombsa", "dranikcatacombsb", "dranikcatacombsc", "dranikhollowsa", "dranikhollowsb", "dranikhollowsc", "dranik", "draniksewersa", "draniksewersb", "draniksewersc", "draniksscar", "dreadlands", "dreadspire", "dredge", "drogab", "droga", "dulak", "eastkarana", "eastkorlacha", "eastkorlach", "eastsepulcher", "eastwastes", "eastwastesshard", "eastwastestwo", "echo", "elddara", "elddar", "emeraldjungle", "empyr", "endlesscaverns", "erudnext", "erudnint", "erudsxing", "esianti", "ethernere", "everfrost", "eviltree", "exaltedb", "exalted", "fallen", "fearplane", "feerrott2", "feerrott", "felwithea", "felwitheb", "ferubi", "fhalls", "fieldofbone", "firefallpass", "firiona", "foundation", "freeportacademy", "freeportarena", "freeportcityhall", "freeporteast", "freeporthall", "freeportmilitia", "freeportsewers", "freeporttheater", "freeportwest", "freporte", "freportn", "freportw", "frontiermtnsb", "frontiermtns", "frostcrypt", "frozenshadow", "frozenshadowtwo", "fungalforest", "fungusgrove", "gfaydark", "gnomemtn", "gorowyn", "greatdivide", "greatdividetwo", "grelleth", "griegsend", "grimling", "grobb", "growthplane", "guardian", "guildhall3", "guildhalllrg", "guildhall", "guildhallsml", "guildlobby", "guka", "gukb", "gukbottom", "gukc", "gukd", "guke", "gukf", "gukg", "gukh", "guktop", "gunthak", "gyrospireb", "gyrospirez", "halas", "harbingers", "hateplaneb", "hateplane", "hatesfury", "heartoffearb", "heartoffearc", "heartoffear", "highkeep", "highpasshold", "hillsofshade", "hohonora", "hohonorb", "hole", "hollowshade", "hollowshadetwo", "housegarden", "iceclad", "icefall", "ikkinz", "illsalina", "illsalinb", "illsalinc", "illsalin", "inktuta", "innothuleb", "jaggedpine", "jardelshook", "kael", "kaelshard", "kaeltwo", "kaesora", "kaladima", "kaladimb", "karnor", "kattacastrumb", "kattacastrum", "katta", "kedge", "kithicor", "kodtaz", "korascian", "korshaext", "korshaint", "kurn", "lakeofillomen", "lakerathe", "lavastorm", "lceanium", "letalis", "lfaydark", "lichencreep", "lopingplains", "maidenhouseint", "maiden", "maidensgrave", "maidentwo", "mansion", "mearatas", "mechanotus", "mesa", "miragulmare", "mira", "mirb", "mirc", "mird", "mire", "mirf", "mirg", "mirh", "miri", "mirj", "mischiefplane", "mistmoore", "mistythicket", "mmca", "mmcb", "mmcc", "mmcd", "mmce", "mmcf", "mmcg", "mmch", "mmci", "mmcj", "monkeyrock", "moors", "morellcastle", "mseru", "nadox", "najena", "natimbi", "necropolis", "necropolistwo", "nedaria", "neighborhood", "nektulosa", "nektulos", "neriaka", "neriakb", "neriakc", "neriakd", "netherbian", "nexus", "nightmareb", "northkarana", "northro", "nro", "nurga", "oceangreenhills", "oceangreenvillage", "oceanoftears", "oggok", "oldblackburrow", "oldbloodfield", "oldcommons", "olddranik", "oldfieldofboneb", "oldfieldofbone", "oldkaesoraa", "oldkaesorab", "oldkithicor", "oldkurn", "overthere", "overtheretwo", "paineel", "paludal", "paludaltwo", "paw", "pellucid", "permafrost", "phinteriortree", "phylactery", "pillarsalra", "plhdkeinteriors1a1", "plhdkeinteriors1a2", "plhdkeinteriors1a3", "plhdkeinteriors3a1", "plhdkeinteriors3a2", "plhdkeinteriors3a3", "plhogrinteriors1a1", "plhogrinteriors1a2", "plhogrinteriors3a1", "plhogrinteriors3a2", "plhogrinteriors3b1", "plhogrinteriors3b2", "poair", "podisease", "poeartha", "poearthb", "pofire", "pohealth", "poinnovation", "pojustice", "poknowledge", "ponightmare", "poshadow", "postorms", "potactics", "potimea", "potimeb", "potorment", "potranquility", "povalor", "powar", "powater", "precipiceofwar", "provinggrounds", "qcat", "qey2hh1", "qeynos2", "qeynos", "qeytoqrg", "qinimi", "qrg", "qvic", "ragea", "rage", "rathechamber", "rathemtn", "redfeather", "relic", "resplendent", "riftseekers", "rivervale", "riwwi", "roost", "rubak", "ruja", "rujb", "rujc", "rujd", "ruje", "rujf", "rujg", "rujh", "ruji", "rujj", "runnyeye", "sarithcity", "scarlet", "scorchedwoods", "sebilis", "sepulcher", "shadeweaver", "shadeweavertwo", "shadowedmount", "shadowhaven", "shadowhaventwo", "shadowrest", "shadowspine", "shadowvalley", "shardslanding", "sharvahl", "sharvahltwo", "shiningcity", "shipmvm", "shipmvp", "shipmvu", "shippvu", "shipuvu", "shipworkshop", "silyssar", "sirens", "skyfire", "skyfiretwo", "skylance", "skyshrine", "skyshrinetwo", "sleeper", "sleepertwo", "sncrematory", "snlair", "snplant", "snpool", "soldunga", "soldungb", "soldungc", "solrotower", "soltemple", "solteris", "somnium", "southkarana", "southro", "sseru", "ssratemple", "steamfactory", "steamfontmts", "steppes", "stillmoona", "stillmoonb", "stonebrunt", "stonehive", "stonesnake", "stratos", "suncrest", "sunderock", "swampofnohope", "tacvi", "taka", "takb", "takc", "takd", "take", "takf", "takg", "takh", "taki", "takishruinsa", "takishruins", "takj", "tempesttemple", "templeveeshan", "templeveeshantwo", "tenebrous", "thalassius", "theatera", "theater", "thedeep", "thegrey", "thenest", "thevoida", "thevoidb", "thevoidc", "thevoidd", "thevoide", "thevoidf", "thevoidg", "thevoidh", "thuledream", "thulehouse1", "thulehouse2", "thulelibrary", "thuliasaur", "thundercrest", "thurgadina", "thurgadinb", "timorous", "tipt", "torgiran", "toskirakk", "towerofrot", "toxxulia", "trakanon", "trialsofsmoke", "tutoriala", "tutorialb", "twilight", "txevu", "umbral", "umbraltwo", "underquarry", "unrest", "uqua", "valdeholm", "veeshan", "veeshantwo", "veksar", "velketor", "velketortwo", "vergalid", "vexthal", "vexthaltwo", "vxed", "wakening", "wallofslaughter", "warrens", "warslikswood", "weddingchapeldark", "weddingchapel", "well", "westkorlacha", "westkorlachb", "westkorlachc", "westkorlach", "westsepulcher", "westwastes", "westwastestwo", "windsong", "xorbb", "yxtta", "zhisza" };
std::error_code ec;
std::stringstream jParse;
json MeshDatabase = json::array();
json SettingsDatabase = json::array();
json IgnoreDatabase = json::array();

/**
 * STRUCTURES
 *
 * For when you want to store data where it doesn't get lonely.
 *
 */
struct DownloadListStorage
{
	std::string FileName;
	std::string FileUrl;
};

struct HashListStorage
{
	std::string FileName;
	std::string FilePath;
	std::string FileUrl;
	std::string HashType;
	std::string RemoteHash;
};

struct ProgressMeter {
	std::string Thread;
	std::string FileUrl;
	std::string FileName;
};

/**
 * HELPER FUNCTIONS - vcpkg install cryptopp
 *
 * Functions created to make common tasks used quicker.
 *
 * Examples: Checking if a file exists, sending chat messages, calculating the hash of a file. Etc...
 *
 */
std::string Get_Hash(const fs::path& p, const std::string& h = "md5")
{
	if (fs::exists(p))
	{
		std::string str;
		std::string out;
		CryptoPP::FileSource file(p.string().c_str(), true, new CryptoPP::StringSink(str));

		if (h == "md5")
			out = MD5(str);
		else if (h == "sha256")
			out = SHA256(str);
		else
			MeshWriteChat("\arHasing Error. Unsupported hashing function.", false);
		return out;
	}
	else
	{
		MeshWriteChat("\arHashing Error. Invalid Path", false);
		return "0";
	}
}

void Get_Hash_For_Update(const struct HashListStorage& tmp)
{
	if (fs::exists(tmp.FilePath, ec))
	{
		DownloadListStorage tmpDownloadList;
		std::string str;
		std::string out;

		CryptoPP::FileSource file(tmp.FilePath.c_str(), true, new CryptoPP::StringSink(str));

		if (tmp.HashType == "md5")
		{
			out = MD5(str);
		}
		else if (tmp.HashType == "sha256")
		{
			out = SHA256(str);
		}
		else
		{
			MeshWriteChat("\arHasing Error. Unsupported hashing function.", false);
			HashThreads--;
		}

		if (out.compare(tmp.RemoteHash) != 0)
		{
			tmpDownloadList.FileName = tmp.FileName;
			tmpDownloadList.FileUrl = tmp.FileUrl;
			DownloadList.push_back(tmpDownloadList);
			HashThreads--;
		}
	}
	else
	{
		MeshWriteChat("\arHashing Error. Invalid Path.", false);
		HashThreads--;
	}
}

std::string MD5(const std::string& data)
{
	std::string res;
	CryptoPP::Weak::MD5 hash;
	CryptoPP::byte digest[CryptoPP::Weak::MD5::DIGESTSIZE];

	hash.CalculateDigest(digest, (CryptoPP::byte*)data.c_str(), data.size());

	CryptoPP::HexEncoder encoder;
	encoder.Attach(new CryptoPP::StringSink(res));
	encoder.Put(digest, sizeof(digest));
	encoder.MessageEnd();

	return res;
}

std::string SHA256(const std::string& data)
{
	std::string res;
	CryptoPP::SHA256 hash;
	CryptoPP::byte digest[CryptoPP::SHA256::DIGESTSIZE];

	hash.CalculateDigest(digest, (CryptoPP::byte*)data.c_str(), data.size());

	CryptoPP::HexEncoder encoder;
	encoder.Attach(new CryptoPP::StringSink(res));
	encoder.Put(digest, sizeof(digest));
	encoder.MessageEnd();

	return res;
}

bool InGameAndSpawned()
{
	return(GetGameState() == GAMESTATE_INGAME && pLocalPC != NULL);
}

bool ValidateZoneShortName(const std::string& shortname)
{
	int i = 0;
	while (!Zones[i].empty())
	{
		if (mq::ci_equals(Zones[i], shortname))
			return true;
		i++;
	}
	return false;
}

/**
 * Chat function that won't spam you to death. When calling chat,
 * use true for plugin spam, use false for failure messages and
 * when responding to user commands.
 */
void MeshWriteChat(const std::string& msg, bool AntiSpam)
{
	static const char* const prefix = "\ag[\a-tMQ2MeshManager\ag]\aw ";

	if (AntiSpam)
	{
		if (msg == LastMsg)
			return;
		else
			LastMsg = msg;
	}
	WriteChatf("%s%s", prefix, msg.c_str());
}


/**
 * MESH DATABASE FUNCTIONS - vcpkg install nlohmann-json
 *
 * Uses nlohmann/json single file json parser.
 *
 */
void MeshLoadDatabase()
{
	const fs::path f = fs::path(gPathResources) / "MQ2MeshManager" / "meshdb.json";
	if (!fs::exists(f, ec))
	{
		MeshWriteChat("\arDatabase file does not exist. Use \aw/mesh updatedb\ar first.", false);
	}
	else
	{
		FILE* jFile;

		const errno_t err = fopen_s(&jFile, f.string().c_str(), "r");
		if (err == 0)
		{
			MeshDatabase.clear();
			MeshDatabase = json::parse(jFile);
			MeshWriteChat("\agDatabase Initialized.", false);
			fclose(jFile);
		}
		else
		{
			MeshWriteChat("\arError opening database file for reading.", false);
		}
	}
}

bool MeshIsDatabaseEmpty(json object) {
	if (object.empty())
		return true;
	else
		return false;
}

/**
 * DOWNLOADER - vpkg install curl zlib
 *
 * Streams data into a size_t in chunks before writing it to disk. Includes
 * functions for tracking progress.
 *
 */
size_t Curl_Write_Data(void* ptr, size_t size, size_t nmemb, FILE* stream)
{
	size_t written = fwrite(ptr, size, nmemb, stream);
	return written;
}

int Curl_Progress_Report(void* ptr, double TotalToDownload, double NowDownloaded, double TotalToUpload, double NowUploaded)
{
	struct ProgressMeter* pm = (struct ProgressMeter*)ptr;
	if (TotalToDownload <= 0.0)
	{
		return 0;
	}
	int progress = (int)round(NowDownloaded * 100 / TotalToDownload);
	MeshWriteChat(fmt::format("{} {}{} {} {} {} {}", "\ag[\atThread\aw:", pm->Thread, "\ag] \awDownload of\ay ", pm->FileName, "\awis\at", std::to_string(progress), "%\aw complete."), true);
	fProgressDL = progress;
	return CURL_PROGRESSFUNC_CONTINUE;
}

void MeshDownloadFile(const std::string& url, const std::string& filename, const std::string& savepath, bool single)
{
	struct ProgressMeter pm;
	CURL* curl;
	FILE* fp;
	CURLcode result;
	const std::string FinalPath = fmt::format("{}{}{}", savepath, "\\", filename);

	// Update TLOs
	fCurrentDL = filename;
	fPathDL = FinalPath;

	curl = curl_easy_init();
	fDownloading = true;

	if (curl)
	{
		pm.FileName = filename;
		pm.FileUrl = url;
		pm.Thread = std::to_string(DownloadThreads);

		errno_t fw = fopen_s(&fp, FinalPath.c_str(), "wb");
		if (fw != 0)
		{
			char errstr[3]{};
			sprintf_s(errstr, "%d", fw);
			if (DownloadThreads > 0)
			{
				DownloadThreads--;
			}
			MeshWriteChat(fmt::format("{} {} {} {}[}", "\arError opening\aw", filename, "\arfor writing. [Error:", errstr, "]"), false);
			curl_easy_cleanup(curl);
		}
		else
		{
			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
			curl_easy_setopt(curl, CURLOPT_SSL_ENABLE_ALPN, 1L);
			curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, &pm);
			curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, Curl_Progress_Report);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Curl_Write_Data);
			curl_easy_setopt(curl, CURLOPT_NOPROGRESS, HideProgress);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
			result = curl_easy_perform(curl);
			if (result == CURLE_OK)
			{
				if (DownloadThreads > 0)
				{
					DownloadThreads--;
				}
				MeshWriteChat(fmt::format("{} {} {}", "\awDownload of\ag", filename, "\aw complete."), true);
			}
			else
			{
				if (DownloadThreads > 0)
				{
					DownloadThreads--;
				}
				MeshWriteChat(fmt::format("{} {} {} {}{}", "\arError downloading file\aw", filename, "\ar.\n\a-r[\arError Code:\aw", curl_easy_strerror(result), "\a-r]"), false);
			}
			curl_easy_cleanup(curl);
			fclose(fp);

			// Reload Database after we download it
			if (!filename.empty() && filename == "meshdb.json")
			{
				MeshLoadDatabase();
			}
			// Update TLOS
			fLastDL = (!filename.empty()) ? filename : "none";
			fCurrentDL = "none";
			fPathDL = "none";
			fDownloading = false;
		}
	}
}

/**
 * TOP LEVEL OBJECTS
 *
 * TLOs available to grab information about current download (filename, progress, path, is a download running currently etc..)
 *
 * Will Add checking if mesh for current zone is up to date, mesh for a picked zone is up to date etc later when the code for that becomes
 * available in the future.
 *
 */
class MQ2MeshManagerType* pMeshTypes = 0;
class MQ2MeshManagerType : public MQ2Type
{
public:
	enum Information
	{
		IsDownloading = 1,
		DownloadCurrent = 2,
		DownloadLast = 3,
		DownloadProgress = 4,
		DownloadPath = 5
	};
	MQ2MeshManagerType() :MQ2Type("MeshManager")
	{
		TypeMember(IsDownloading);
		TypeMember(DownloadCurrent);
		TypeMember(DownloadLast);
		TypeMember(DownloadProgress);
		TypeMember(DownloadPath);
	}
	virtual bool MQ2MeshManagerType::GetMember(MQVarPtr VarPtr, const char* Member, char* Index, MQTypeVar& Dest) override
	{
		auto pMember = MQ2MeshManagerType::FindMember(Member);
		if (pMember)
		{
			switch ((Information)pMember->ID)
			{
			case IsDownloading:
				Dest.DWord = fDownloading;
				Dest.Type = mq::datatypes::pBoolType;
				return true;
			case DownloadCurrent:
				Dest.Ptr = &fCurrentDL;
				Dest.Type = mq::datatypes::pStringType;
				return true;
			case DownloadLast:
				Dest.Ptr = &fLastDL;
				Dest.Type = mq::datatypes::pStringType;
				return true;
			case DownloadProgress:
				Dest.Int = fProgressDL;
				Dest.Type = mq::datatypes::pIntType;
				return true;
			case DownloadPath:
				Dest.Ptr = &fPathDL;
				Dest.Type = mq::datatypes::pStringType;
				return true;
			}
		}
	}
};

bool DataMeshManager(const char* Index, MQTypeVar& Dest)
{
	Dest.Type = pMeshTypes;
	Dest.DWord = 1;
	return true;
}

/**
 * Settings/Configuration File
 *
 * Here's where we load/save our settings.
 *
 */
void MeshManagerSaveSettings()
{
	const std::string FileName = fmt::format("{}_{}.json", pEverQuestInfo->WorldServerShortname, pLocalPC->Name);
	const fs::path f = fs::path(gPathResources) / "MQ2MeshManager" / FileName;
	FILE* jFile;
	json tmp;
	auto& settings = tmp["Settings"];
	settings["MaxDownloads"] = MaxDownloadThreads;
	settings["MaxHashes"] = MaxHashThreads;
	settings["DownloadMissing"] = AutoDownloadMissing;
	settings["AutoUpdate"] = AutoCheckForUpdates;
	settings["ProgressMeter"] = HideProgress;
	settings["ThreadSafety"] = ThreadSafe;

	const errno_t err = fopen_s(&jFile, f.string().c_str(), "wb");
	if (err == 0)
	{
		fputs(tmp.dump().c_str(), jFile);
		fclose(jFile);
	}
	else
	{
		MeshWriteChat("\arError opening settings file for writing.", false);
	}
	MeshManagerLoadSettings();
}

void MeshManagerLoadSettings()
{
	const std::string FileName = fmt::format("{}_{}.json", pEverQuestInfo->WorldServerShortname, pLocalPC->Name);
	const fs::path f = fs::path(gPathResources) / "MQ2MeshManager" / FileName;
	FILE* jFile;

	if (!fs::exists(f, ec))
	{
		MeshWriteChat("\arSettings File Doesn't Exist. Loading Default Settings.", false);
		MeshManagerSaveSettings();
	}
	else
	{
		SettingsDatabase.clear();
		const errno_t err = fopen_s(&jFile, f.string().c_str(), "rb");
		if (err == 0)
		{
			SettingsDatabase = json::parse(jFile);
			fclose(jFile);

			auto& settings = SettingsDatabase["Settings"];

			MaxDownloadThreads = settings["MaxDownloads"];
			MaxHashThreads = settings["MaxHashes"];
			AutoDownloadMissing = settings["DownloadMissing"];
			AutoCheckForUpdates = settings["AutoUpdate"];
			HideProgress = settings["ProgressMeter"];
			ThreadSafe = settings["ThreadSafety"];
		}
		else
		{
			MeshWriteChat("\arError opening settings file for reading.", false);
		}
	}
}

/**
 * ZONE IGNORE
 *
 * For when you have a custom mesh and don't want it to get nuked by the updater.
 *
 */
void MeshManagerSaveIgnores()
{
	const fs::path p = fs::path(gPathResources) / "MQ2MeshManager" / "ZoneIgnores.json";
	FILE* jFile;
	json tmp;

	for (int i = 0; i <= IgnoreList.size() - 1; i++)
	{
		tmp[IgnoreList[i]] = true;
	}

	const errno_t err = fopen_s(&jFile, p.string().c_str(), "wb");
	if (err == 0)
	{
		fputs(tmp.dump().c_str(), jFile);
		fclose(jFile);
	}
	else
	{
		MeshWriteChat("\arError opening settings file for writing.", false);
	}
	MeshManagerLoadIgnores();
}

void MeshManagerLoadIgnores()
{
	const fs::path p = fs::path(gPathResources) / "MQ2MeshManager" / "ZoneIgnores.json";
	FILE* jFile;

	if (!fs::exists(p, ec))
	{
		return;
	}
	else
	{
		IgnoreDatabase.clear();
		const errno_t err = fopen_s(&jFile, p.string().c_str(), "rb");
		if (err == 0)
		{
			IgnoreDatabase = json::parse(jFile);
			fclose(jFile);

			IgnoreList.clear();
			for (auto i = IgnoreDatabase.begin(); i != IgnoreDatabase.end(); ++i)
			{
				if (ValidateZoneShortName(i.key()))
				{
					IgnoreList.push_back(i.key());
				}
			}
		}
		else
		{
			MeshWriteChat("\arError opening ignore file for reading.", false);
		}
	}
}

/**
 * COMMANDS
 *
 * Slash commands to operate plugin in game.
 *
 */
void MeshManager(SPAWNINFO* pChar, char* szLine)
{
	char fMsg[256] = { 0 };
	bool syntaxError = false;
	char Param1[MAX_STRING];
	char Param2[MAX_STRING];
	char Param3[MAX_STRING];
	GetArg(Param1, szLine, 1);
	GetArg(Param2, szLine, 2);
	GetArg(Param3, szLine, 3);

	if (InGameAndSpawned())
	{
		if (szLine[0] == 0)
		{
			MeshManagerMenu("help");
		}
		else if (mq::ci_equals(Param1, "help"))
		{
			MeshManagerMenu("help");
		}
		else if (mq::ci_equals(Param1, "agree"))
		{
			if (!fAgree)
			{
				if (mq::ci_equals(Param2, "confirm"))
				{
					fs::path fp = fs::path(gPathResources) / "MQ2MeshManager" / "confirmed.txt";
					if (!fs::exists(fp, ec))
					{
						std::ofstream output(fp);
						MeshWriteChat("\agAgreement Confirmed!", true);
					}
					else
					{
						MeshWriteChat("\arPermissions error. Can't open agreement file for writing.", false);
						return;
					}
				}
				else
				{
					MeshManagerMenu("agreement");
				}
			}
			else
			{
				MeshWriteChat("\agAgreement already accepted!", false);
				return;
			}
		}
		else if (mq::ci_equals(Param1, "hash"))
		{
			MeshManagerDisplayHashes(Param2);
		}
		else if (mq::ci_equals(Param1, "ignore"))
		{
			std::string zonename = GetShortZone(pLocalPC->zoneId);
			if (Param2[0] == '\0' || mq::ci_equals(Param2, "list"))
			{
				MeshManagerMenu("ignorelist");
			}
			if (mq::ci_equals(Param2, "add"))
			{
				if (Param3[0] == '\0')
				{
					IgnoreList.push_back(zonename);
					MeshWriteChat(fmt::format("{} {} {}", "\a-tAdded Zone\ag", zonename, "\a-tto download ignore list."), false);
					MeshManagerSaveIgnores();
				}
				else
				{
					auto p3 = mq::to_lower_copy(Param3);
					if (ValidateZoneShortName(p3))
					{
						IgnoreList.push_back(p3);
						MeshWriteChat(fmt::format("{} {} {}", "\a-tAdded Zone\ag", p3, "\a-tto download ignore list"), false);
						MeshManagerSaveIgnores();
					}
					else
					{
						MeshWriteChat("\arInvalid zone shortname. Try again.", false);
						return;
					}
				}
			}
			if (mq::ci_equals(Param2, "del") || mq::ci_equals(Param2, "delete") ||
				mq::ci_equals(Param2, "rem") || mq::ci_equals(Param2, "remove"))
			{
				if (Param3[0] == '\0')
				{
					for (int i = 0; i <= (int)IgnoreList.size(); i++)
					{
						if (IgnoreList[i] == zonename)
						{
							IgnoreList.erase(std::next(IgnoreList.begin(), i));
							MeshWriteChat(fmt::format("{} {} {}", "\a-tRemoved zone\ar", zonename, "\a-tfrom download ignore list."), false);
							MeshManagerSaveIgnores();
							return;
						}
					}
				}
				else
				{
					auto p3 = mq::to_lower_copy(Param3);
					if (ValidateZoneShortName(p3))
					{
						auto p3a = std::find(IgnoreList.begin(), IgnoreList.end(), p3);
						IgnoreList.erase(p3a);
						MeshWriteChat(fmt::format("{} {} {}", "\a-tRemoved zone\ar", p3, "\a-tfrom download ignore list."), false);
						MeshManagerSaveIgnores();
						return;
					}
					else
					{
						MeshWriteChat("\arInvalid zone shortname. Try again.", false);
						return;
					}
				}
			}
		}
		else if (mq::ci_equals(Param1, "settings"))
		{
			MeshManagerMenu("settings");
		}
		else if (mq::ci_equals(Param1, "set"))
		{
			if (Param2[0] == '\0')
			{
				MeshManagerMenu("set");
			}
			else if (mq::ci_equals(Param2, "autoupdate"))
			{
				std::string AutoFeature;
				if (Param3[0] == '\0')
				{
					AutoFeature = (AutoCheckForUpdates) ? "\agOn" : "\arOff";
					MeshWriteChat(fmt::format("{} {}" "\a-tAutomatically Check For Updates\aw:", AutoFeature), false);
				}
				else if (Param3[0] != '\0')
				{
					if (mq::ci_equals(Param3, "on") || mq::ci_equals(Param3, "1") || mq::ci_equals(Param3, "true"))
					{
						AutoCheckForUpdates = true;
						MeshWriteChat(fmt::format("{} {}", "\a-tAutomatically Check For Updates\aw:\ag", Param3), false);
						MeshManagerSaveSettings();
					}
					else if (mq::ci_equals(Param3, "off") || mq::ci_equals(Param3, "0") || mq::ci_equals(Param3, "false"))
					{
						AutoCheckForUpdates = false;
						MeshWriteChat(fmt::format("{} {}", "\a-tAutomatically Check For Updates\aw:\ar", Param3), false);
						MeshManagerSaveSettings();
					}
					else
					{
						MeshWriteChat("\ar/mesh set autoupdaate <on/off/1/0/true/false>", false);
						return;
					}
				}
			}
			else if (mq::ci_equals(Param2, "maxdownloads"))
			{
				if (Param3[0] == '\0')
				{
					MeshWriteChat(fmt::format("{} {}", "\a-tMax Download Threads\aw:\ag", std::to_string(MaxDownloadThreads)), false);
				}
				else if (Param3[0] != '\0')
				{

					if (!mq::IsNumber(Param3))
					{
						MeshWriteChat("\ar/mesh set maxdownloads requires a number 1-10.", false);
					}
					else
					{
						short p3 = mq::GetIntFromString(Param3, 0);
						if (p3 <= 0 || p3 > 10)
						{
							MeshWriteChat("\ar/mesh set maxdownloads requires a number 1-10.", false);
						}
						else
						{
							MeshWriteChat(fmt::format("{} {}", "\a-tMax Download Threads\aw:\ag", p3), false);
							MaxDownloadThreads = p3;
							MeshManagerSaveSettings();
						}
					}
				}
			}
			else if (mq::ci_equals(Param2, "maxhashes"))
			{
				if (Param3[0] == '\0')
				{
					MeshWriteChat(fmt::format("{} {}", "\a-tMax Hash Threads\aw:\ar", std::to_string(MaxHashThreads)), false);
				}
				else if (Param3[0] != '\0')
				{
					if (!mq::IsNumber(Param3))
					{
						MeshWriteChat("\ar/mesh set maxhashes requires a number 1-10.", false);
						return;
					}
					else
					{
						short p3 = mq::GetIntFromString(Param3, 0);
						if (p3 <= 0 || p3 > 10)
						{
							MeshWriteChat("\ar/mesh set maxhashes requires a number 1-10.", false);
							return;
						}
						else
						{
							MeshWriteChat(fmt::format("{} {}", "\a-tMax Hash Threads\aw:\ag", Param3), false);
							MaxHashThreads = p3;
							MeshManagerSaveSettings();
						}
					}
				}
			}
			else if (mq::ci_equals(Param2, "missing"))
			{
				std::string AutoFeature;
				if (Param3[0] == '\0')
				{
					AutoFeature = (AutoDownloadMissing) ? "\agOn" : "\arOff";
					MeshWriteChat(fmt::format("{} {}", "\a-tAutomatically Download Missing Meshes\aw:", AutoFeature), false);
				}
				else if (Param3[0] != '\0')
				{
					if (mq::ci_equals(Param3, "on") || mq::ci_equals(Param3, "1") || mq::ci_equals(Param3, "true"))
					{
						AutoDownloadMissing = true;
						MeshWriteChat(fmt::format("{} {}", "\a-tAutomatically Download Missing Meshes\aw:\ag", Param3), false);
						MeshManagerSaveSettings();
					}
					else if (mq::ci_equals(Param3, "off") || mq::ci_equals(Param3, "0") || mq::ci_equals(Param3, "false"))
					{
						AutoDownloadMissing = false;
						MeshWriteChat(fmt::format("{} {}", "\a-tAutomatically Download Missing Meshes\aw:\ar", Param3), false);
						MeshManagerSaveSettings();
					}
					else
					{
						MeshWriteChat("\ar/mesh set missing <on/off/1/0/true/false>", false);
						return;
					}
				}
			}
			else if (mq::ci_equals(Param2, "progress"))
			{
				std::string AutoFeature;
				if (Param3[0] == '\0')
				{
					AutoFeature = (HideProgress) ? "\arOff" : "\agOn";
					MeshWriteChat(fmt::format("{} {}", "\a-tShow Download Progress\aw:", AutoFeature), false);
				}
				else if (Param3[0] != '\0')
				{
					if (mq::ci_equals(Param3, "on") || mq::ci_equals(Param3, "1") || mq::ci_equals(Param3, "true"))
					{
						HideProgress = 0;
						MeshWriteChat(fmt::format("{} {}", "\a-tShow Download Progress\aw:\ag", Param3), false);
						MeshManagerSaveSettings();
					}
					else if (mq::ci_equals(Param3, "off") || mq::ci_equals(Param3, "0") || mq::ci_equals(Param3, "false"))
					{
						HideProgress = 1;
						MeshWriteChat(fmt::format("{} {}", "\a-tShow Download Progress\aw:\ar", Param3), false);
						MeshManagerSaveSettings();
					}
					else
					{
						MeshWriteChat("\ar/mesh set progress <on/off/1/0/true/false>", false);
						return;
					}
				}
			}
			else if (mq::ci_equals(Param2, "threadsafety"))
			{
				std::string AutoFeature;
				if (Param3[0] == '\0')
				{
					AutoFeature = (ThreadSafe) ? "\agOn" : "\arOff";
					MeshWriteChat(fmt::format("{} {}", "\a-tThread Safety\aw:", AutoFeature), false);
				}
				else if (Param3 != NULL || Param3[0] != '\0')
				{
					if (mq::ci_equals(Param3, "on") || mq::ci_equals(Param3, "1") || mq::ci_equals(Param3, "true"))
					{
						ThreadSafe = true;
						MeshWriteChat(fmt::format("{} {}", "\a-tThread Safety\aw:\ag", Param3), false);
						MeshManagerSaveSettings();
					}
					else if (mq::ci_equals(Param3, "off") || mq::ci_equals(Param3, "0") || mq::ci_equals(Param3, "false"))
					{
						ThreadSafe = false;
						MeshWriteChat(fmt::format("{} {}", "\a-tThread Safety\aw:\ar", Param3), false);
						MeshManagerSaveSettings();
					}
					else
					{
						MeshWriteChat("\ar/mesh set threadsafety <on/off/1/0/true/false>", false);
						return;
					}
				}
			}
		}
		else if (mq::ci_equals(Param1, "updatedb"))
		{
			if (fAgree)
				MeshUpdateDatabase();
			else
				MeshWriteChat("\arYou have not yet accepted the user agreement. Please type \aw/mesh agree\ar for more information.", false);
			return;
		}
		else if (mq::ci_equals(Param1, "updatezone"))
		{
			if (MeshIsDatabaseEmpty(MeshDatabase))
				MeshWriteChat("\arDatabase does not exist. Use \aw/mesh updatedb\ar first.", false);

			if (fAgree)
				MeshManagerUpdateZone(Param2);
			else
				MeshWriteChat("\arYou have not yet accepted the user agreement. Please type \aw/mesh agree\ar for more information.", false);
			return;
		}
		else if (mq::ci_equals(Param1, "updateall"))
		{
			if (MeshIsDatabaseEmpty(MeshDatabase))
				MeshWriteChat("\arDatabase does not exist. Use \aw/mesh updatedb\ar first.", false);

			if (fAgree)
				MeshManagerUpdateAll(Param2, Param3);
			else
				MeshWriteChat("\arYou have not yet accepted the user agreement. Please type \aw/mesh agree\ar for more information.", false);
			return;
		}
		else if (mq::ci_equals(Param1, "tlos"))
		{
			MeshManagerMenu("tlos");
		}
		else
		{
			syntaxError = true;
		}

		if (syntaxError)
		{
			MeshWriteChat("\arUnknown command or option. Check \aw/mesh help\ar for correct syntax.", false);
		}
	}
	else
	{
		MeshWriteChat("\arPlease make sure you are logged in and spawned in game.", false);
	}
}

void MeshManagerUpdateAll(const char* Param2, const char* Param3) {
	DownloadListStorage tmp;
	HashListStorage tmp2;
	std::string mn, zn, md5, fmd5, url;

	if (!fDownloadReady)
	{
		if (mq::ci_equals(Param2, "confirm"))
		{
			if (mq::ci_equals(Param3, "overwrite"))
			{
				for (int i = 0; i <= MaxZone - 1; i++)
				{
					zn = Zones[i];
					tmp.FileName = zn + ".navmesh";
					tmp.FileUrl = MeshDatabase[zn]["link"];
					DownloadList.push_back(tmp);
				}
				MeshWriteChat("\agAll files added to download manager.", false);
				fDownloadReady = true;
				return;
			}
			for (int i = 0; i <= MaxZone - 1; i++)
			{
				zn = Zones[i];
				fs::path tp = fs::path(gPathResources) / "MQ2Nav" / mn;
				//File doesn't exist? Go ahead and add it to the download list.
				if (!fs::exists(tp, ec)) {
					tmp.FileName = zn + ".navmesh";
					tmp.FileUrl = MeshDatabase[zn]["link"];
					DownloadList.push_back(tmp);
				}
				else
				{
					// If file exists look up its hash and pass it to the hashing thread.
					tmp2.HashType = "md5";
					tmp2.FilePath = tp.string();
					tmp2.FileName = zn + ".navmesh";
					tmp2.FileUrl = MeshDatabase[zn]["link"];
					tmp2.RemoteHash = MeshDatabase[zn]["hash"];
					HashpipeList.push_back(tmp2);
					fHashReady = true;
				}
			}
		}
		else
		{
			MeshWriteChat("\awThis operation will \aroverwrite\aw custom meshes. If you are sure you wish to proceed, type \ar/mesh updataall confirm\aw to continue. If you wish to redownload all files use \ar/mesh updateall confirm overwrite", false);
			return;
		}
	}
	else
	{
		MeshWriteChat("\arDownload queue already being processed. Please wait for current operation to finish.", false);
	}
}


void MeshManagerDisplayHashes(const char* Param2)
{
	fs::path hPath;
	std::string fn;

	// /mesh hash with no argument
	if (Param2[0] == '\0')
	{
		int currentZoneID = pLocalPC->zoneId;

		if (currentZoneID != -1)
		{
			fn = std::string(GetShortZone(currentZoneID)) + ".navmesh";
			hPath = fs::path(gPathResources) / "MQ2Nav" / fn;
		}
		else
		{
			MeshWriteChat("\arError. Improper Zone ID. Please try again.", false);

		}
	}
	// /mesh hash [zoneshortname]
	else
	{
		fn = std::string(Param2) + ".navmesh";
		hPath = fs::path(gPathResources) / "MQ2Nav" / fn;
	}

	if (fs::exists(hPath, ec))
	{
		std::string md5 = Get_Hash(hPath);
		//std::string sha256 = Get_Hash(hPath, "sha256");

		MeshWriteChat(fmt::format("{} {} {}", "\a-t------------------------\aw", fn, "\a-t ------------------------"), false);
		MeshWriteChat(fmt::format("{} {}", "\a-tMD5\ag:\aw", md5), false);
		//MeshWriteChat("\a-tSHA256\ag:\aw " + sha256, false);
	}
	else
	{
		MeshWriteChat("\arError occured. Either file doesn't exist yet or an invalid shortname was supplied.", false);
	}
}


void MeshManagerUpdateZone(const char* Param2)
{
	DownloadListStorage tmp;
	std::string fn;
	fs::path fPath;
	std::string baseURL = "https://mqmesh.com/resources/meshes/";

	if (Param2 == NULL || Param2[0] == '\0')
	{
		// no zone argument
		int currentZoneID = pLocalPC->zoneId;

		if (currentZoneID != -1)
		{
			fn = std::string(GetShortZone(currentZoneID)) + ".navmesh";
			fPath = fs::path(gPathResources) / "MQ2Nav";

			if (!fs::exists(fPath, ec))
			{
				//no file exists? Just download it!
				MeshWriteChat("\awQueuing file\ag:\ar " + fn, true);
				tmp.FileName = fn;
				tmp.FileUrl = baseURL + fn;
				DownloadList.push_back(tmp);
				fDownloadReady = true;
			}
			else
			{
				std::string dHash = MeshDatabase[std::string(GetShortZone(currentZoneID))]["hash"];
				std::string fHash = Get_Hash(fPath.string() + "\\" + fn);

				MeshWriteChat(fmt::format("{} {}", "\awYou currently have a copy of\ar ", fn), false);
				int upd = dHash.compare(fHash);
				if (upd == 0)
				{
					MeshWriteChat(fmt::format("{} {} {}", "\awIt is currently up to date. If you still wish to update it type\ar /mesh updatezone", GetShortZone(currentZoneID), "\awto overwrite."), false);
				}
				else
				{
					MeshWriteChat(fmt::format("{} {} {}", "\awThere is currenntly a newer version available. If you are \arsure\aw you wish to overwrite your current mesh type\ar /mesh updatezone", GetShortZone(currentZoneID), "\awto overwrite."), false);
				}
			}
		}
		else
		{
			MeshWriteChat("\arCan only run this command without a zone shortname while fully in game.", false);
		}
	}
	else
	{
		// Param2 is a shortzone argument download it and don't argue with them. Overwrite whats there.
		fn = std::string(Param2) + ".navmesh";
		fPath = fs::path(gPathResources) / "MQ2Nav";

		MeshWriteChat(fmt::format("{} {}", "\awQueuing file\ag:\ar ", fn), true);
		tmp.FileName = fn;
		tmp.FileUrl = baseURL + fn;
		DownloadList.push_back(tmp);
		fDownloadReady = true;
	}
}

void MeshManagerMenu(const std::string& menu = "help")
{
	if (menu == "agreement")
	{
		MeshWriteChat("\a-t--------------------------------------------------", false);
		MeshWriteChat("\awI hereby acknowledge, that I have read AND accept that this", false);
		MeshWriteChat("\awplugin connects to and downloads from the internet. All", false);
		MeshWriteChat("\awconnections from this plugin are made directly to", false);
		MeshWriteChat("\a-thttps ://mqmesh.com \awover a TLS encrypted connection.", false);
		MeshWriteChat("", false);
		MeshWriteChat("\awBy typing \ar/mesh agree confirm\aw you acknowledge that you accept these terms.", false);
	}
	if (menu == "help")
	{
		MeshWriteChat(fmt::format("{} {}", "\a-tBy\aw:\ag wired420 \aw- \a-tVersion\aw:\ag", MeshPluginVersion), false);
		if (!fAgree)
		{
			MeshWriteChat("\a-r*** \arAGREEMENT NOT ACCEPTED\a-r ***", false);
		}
		MeshWriteChat("\a-t---------------------- \awHelp\a-t ----------------------", false);
		MeshWriteChat("\ar< > \aw= \arRequired \ag[ ] \aw= \agOptional", false);
		MeshWriteChat("\a-t--------------------------------------------------", false);
		MeshWriteChat("\a-t/mesh [help] - This menu.", false);
		if (!fAgree)
		{
			MeshWriteChat("\a-r/mesh agree - View user agreement.", false);
		}
		MeshWriteChat("\a-t/mesh updatedb - Checks for updates to database.", false);
		MeshWriteChat("\a-t/mesh updatezone [zone shortname] - Checks zone for updates. (Default: current)", false);
		MeshWriteChat("\a-t/mesh updateall <confirm> [overwrite]- Checks all meshes for updates.", false);
		MeshWriteChat("\a-t/mesh ignore <list|add|del> [zone shortname] - Marks Meshes to not ever download.", false);
		MeshWriteChat("\a-t/mesh tlos - Lists available TLOs available.", false);
		MeshWriteChat("\a-t/mesh hash [zone shortname] - Displays hashes for current or desinated zone.", false);
		MeshWriteChat("\a-t/mesh set - Display settings menu.", false);
		MeshWriteChat("\a-t/mesh settings - Display Current Settings", false);
		MeshWriteChat("\a-t--------------------------------------------------", false);
	}
	if (menu == "ignorelist")
	{
		int cnt = 0;
		const fs::path p = fs::path(gPathResources) / "MQ2MeshManager" / "ZoneIgnores.json";
		MeshWriteChat("\a-t-----------\aw Ignores\a-t ------------", false);
		if (!fs::exists(p, ec) || fs::is_empty(p, ec))
		{
			MeshWriteChat("\arIgnore List Is Empty!", false);
			return;
		}
		for (int i = 0; i <= IgnoreList.size() - 1; ++i)
		{
			MeshWriteChat("\aw" + IgnoreList[i], false);
		}
	}
	if (menu == "set")
	{
		MeshWriteChat("\a-t----------------------\aw Set\a-t -----------------------", false);
		MeshWriteChat("\a-t--------------------------------------------------", false);
		MeshWriteChat("\ar< > \aw= \arRequired \ag[ ] \aw= \agOptional", false);
		MeshWriteChat("\a-t--------------------------------------------------", false);
		MeshWriteChat("\a-t/mesh set maxdownloads <1-10>", false);
		MeshWriteChat("\a-t/mesh set maxhashes <1-10>", false);
		MeshWriteChat("\a-t/mesh set missing <on/off/1/0/true/false>", false);
		MeshWriteChat("\a-t/mesh set autoupdate <on/off/1/0/true/false>", false);
		MeshWriteChat("\a-t/mesh set progress <on/off/1/0/true/false>", false);
		MeshWriteChat("\a-t/mesh set threadsafety <on/off/1/0/true/false>", false);
	}
	if (menu == "settings")
	{
		std::string AutoFeatures;

		MeshWriteChat("\a-t------------------- \awSettings\a-t -----------------", false);
		MeshWriteChat("\a-t--------------------------------------------------", false);
		MeshWriteChat(fmt::format("{} {} {} {}", "\a-tMax Downloads\aw:\ar", std::to_string(MaxDownloadThreads), "\a-tMax Hashes\aw:\ar", std::to_string(MaxHashThreads)), false);
		AutoFeatures = (AutoDownloadMissing) ? "\agOn" : "\arOff";
		MeshWriteChat(fmt::format("{} {}", "\a-tAuto Download Missing\aw:", AutoFeatures), false);
		AutoFeatures = (AutoCheckForUpdates) ? "\agOn" : "\arOff";
		MeshWriteChat(fmt::format("{} {}", "\a-tCheck For Updates At Login\aw:", AutoFeatures), false);
		AutoFeatures = (HideProgress) ? "\arOff" : "\agOn";
		MeshWriteChat(fmt::format("{} {}", "\a-tShow Download Progress\aw:", AutoFeatures), false);
		AutoFeatures = (ThreadSafe) ? "\agOn" : "\arOff";
		MeshWriteChat(fmt::format("{} {}", "\a-tThread Safety\aw:", AutoFeatures), false);
	}
	if (menu == "tlos")
	{
		char buf[4] = { 0 };
		std::string tmp = (fDownloading) ? "true" : "false";
		MeshWriteChat("\a-t---------------\at TLOs \a-t---------------", false);
		MeshWriteChat(fmt::format("{} {}", "\at${\a-tMeshManager\at.\a-tisDownloading\at}\ag:\aw", tmp), false);
		MeshWriteChat(fmt::format("{} {}", "\at${\a-tMeshManager\at.\a-tDownloadCurrent\at}\ag:\aw", fCurrentDL), false);
		MeshWriteChat(fmt::format("{} {}", "\at${\a-tMeshManager\at.\a-tDownloadLast\at}\ag:\aw", fLastDL), false);
		MeshWriteChat(fmt::format("{} {}", "\at${\a-tMeshManager\at.\a-tDownloadProgress\at}\ag:\aw", std::to_string(fProgressDL)), false);
		MeshWriteChat(fmt::format("{} {}", "\at${\a-tMeshManager\at.\a-tDownloadPath\at}\ag:\aw", fPathDL), false);
	}
}

void MeshUpdateDatabase() {
	const std::string dbURL = "https://mqmesh.com/meshdb.json";
	const std::string fName = "meshdb.json";
	fs::path meshRes = fs::path(gPathResources) / "MQ2MeshManager";

	MeshWriteChat("Requesting database update:", true);
	MeshDownloadFile(dbURL, fName, meshRes.string(), true);
}

/**
 * THREADING
 *
 * To keep from blocking the client we're setting up threads for hashing and
 * downloading. Hopefully this will keep the client from glitching so much
 * during the update processes.
 *
 */



 /**
  * @fn InitializePlugin
  *
  * This is called once on plugin initialization and can be considered the startup
  * routine for the plugin.
  */
PLUGIN_API void InitializePlugin() {
	char fMsg[256] = { 0 };
	fs::path fPath;

	/* Our folder */
	fPath = fs::path(gPathResources) / "MQ2MeshManager";
	if (!fs::exists(fPath, ec))
	{
		fs::create_directory(fPath, ec);
	}

	/* Nav's Folder. If it's not there. Make it too and assume they'll load nav later. */
	fPath = fs::path(gPathResources) / "MQ2Nav";
	if (!fs::exists(fPath, ec))
	{
		fs::create_directory(fPath, ec);
	}

	fPath = fs::path(gPathResources) / "MQ2MeshManager" / "confirmed.txt";
	if (!fs::exists(fPath, ec))
	{
		MeshWriteChat("\arYou have not yet accepted the user agreement. Please type \aw/mesh agree\ar for more information.", true);
	}
	else
	{
		fAgree = true;
	}

	fPath = fs::path(gPathResources) / "MQ2MeshManager" / "meshdb.json";
	if (fs::exists(fPath, ec))
	{
		MeshLoadDatabase();
	}
	else
	{
		MeshWriteChat("\awDatabase does not exist yet. Please use\ar /mesh updatedb\aw before continuing.", false);
	}

	AddCommand("/mesh", MeshManager);
	AddMQ2Data("MeshManaager", DataMeshManager);
}

/**
 * @fn ShutdownPlugin
 *
 * This is called once when the plugin has been asked to shutdown.  The plugin has
 * not actually shut down until this completes.
 */
PLUGIN_API void ShutdownPlugin()
{
	RemoveCommand("/mesh");
	RemoveMQ2Data("MeshManaager");
	// Examples:
	// RemoveCommand("/mycommand");
	// RemoveXMLFile("MQUI_MyXMLFile.xml");
	// RemoveMQ2Data("mytlo");
}

/**
 * @fn OnCleanUI
 *
 * This is called once just before the shutdown of the UI system and each time the
 * game requests that the UI be cleaned.  Most commonly this happens when a
 * /loadskin command is issued, but it also occurs when reaching the character
 * select screen and when first entering the game.
 *
 * One purpose of this function is to allow you to destroy any custom windows that
 * you have created and cleanup any UI items that need to be removed.
 */
PLUGIN_API void OnCleanUI()
{
	// DebugSpewAlways("MQ2MeshManager::OnCleanUI()");
}

/**
 * @fn OnReloadUI
 *
 * This is called once just after the UI system is loaded. Most commonly this
 * happens when a /loadskin command is issued, but it also occurs when first
 * entering the game.
 *
 * One purpose of this function is to allow you to recreate any custom windows
 * that you have setup.
 */
PLUGIN_API void OnReloadUI()
{
	// DebugSpewAlways("MQ2MeshManager::OnReloadUI()");
}

/**
 * @fn SetGameState
 *
 * This is called when the GameState changes.  It is also called once after the
 * plugin is initialized.
 *
 * For a list of known GameState values, see the constants that begin with
 * GAMESTATE_.  The most commonly used of these is GAMESTATE_INGAME.
 *
 * When zoning, this is called once after @ref OnBeginZone @ref OnRemoveSpawn
 * and @ref OnRemoveGroundItem are all done and then called once again after
 * @ref OnEndZone and @ref OnAddSpawn are done but prior to @ref OnAddGroundItem
 * and @ref OnZoned
 *
 * @param GameState int - The value of GameState at the time of the call
 */
PLUGIN_API void SetGameState(int GameState)
{
	if (!_init)
	{
		if (InGameAndSpawned())
		{
			MeshManagerLoadSettings();
			MeshManagerLoadIgnores();
			_init = true;

			if (AutoCheckForUpdates)
			{
				DoCommand(nullptr, "/mesh updateall confirm");
			}
		}
	}
}


/**
 * @fn OnPulse
 *
 * This is called each time MQ2 goes through its heartbeat (pulse) function.
 *
 * Because this happens very frequently, it is recommended to have a timer or
 * counter at the start of this call to limit the amount of times the code in
 * this section is executed.
 */
PLUGIN_API void OnPulse() {
	if (!fAgree)
	{
		if (std::chrono::steady_clock::now() > PulseTimer)
		{
			PulseTimer = std::chrono::steady_clock::now() + std::chrono::seconds(30);
			fs::path meshRes = fs::path(gPathResources) / "MQ2MeshManager" / "confirmed.txt";
			if (fs::exists(meshRes, ec))
			{
				MeshWriteChat("\agPlugin activated!", true);
				fAgree = true;
			}
		}
	}
	else
	{
		if (fHashReady)
		{
			//Check for Stuck Hash Threads
			if (HashThreads >= MaxHashThreads || HashThreads < 0)
			{
				if (std::chrono::steady_clock::now() > HashThreadTimer)
				{
					HashThreadTimer = std::chrono::steady_clock::now() + std::chrono::seconds(5);
					HashThreadCount++;
					// First one will always trip instantly. Add 1 to account for this.
					if (HashThreadCount >= (ThreadLockSeconds / 4) + 1)
					{
						MeshWriteChat("\arNo hash threads have opened in 20 seconds. Resetting threads.", true);
						HashThreads = 0;
						HashThreadCount = 0;
					}
				}
			}
			else
			{
				HashThreadCount = 0;
			}
			if (!HashpipeList.empty())
			{
				HashListStorage tmp2;
				tmp2 = HashpipeList.front();
				HashpipeList.pop_front();
				HashThreads++;
				std::thread hash_obj(Get_Hash_For_Update, tmp2);
				hash_obj.detach();
			}
			else
			{
				MeshWriteChat(fmt::format("\ar{} {}", std::to_string(DownloadList.size()), "\awfiles added to the download manager."), true);
				fDownloadReady = true;
				fHashReady = false;
			}
		}
		if (fDownloadReady)
		{
			// Check for Stuck Download Threads
			if (DownloadThreads >= MaxDownloadThreads || DownloadThreads < 0)
			{
				if (std::chrono::steady_clock::now() > DownloadThreadTimer)
				{
					DownloadThreadTimer = std::chrono::steady_clock::now() + std::chrono::seconds(5);
					DownloadThreadCount++;
					// First one will always trip instantly. Add 1 to account for this.
					if (DownloadThreadCount >= 4 + 1)
					{
						MeshWriteChat("\arNo download threads have opened in 20 seconds. Resetting threads.", true);
						DownloadThreads = 0;
						DownloadThreadCount = 0;
					}
				}
			}
			else
			{
				DownloadThreadCount = 0;
			}
			// Check if there is anything waiting to be downloaded.
			if (!DownloadList.empty())
			{
				if (DownloadThreads < MaxDownloadThreads)
				{
					int single = (DownloadList.size() > 1) ? false : true;
					DownloadListStorage tmp;
					tmp = DownloadList.front();
					std::string fn = tmp.FileName.substr(0, tmp.FileName.length() - 8);
					bool found = (std::find(IgnoreList.begin(), IgnoreList.end(), fn) != IgnoreList.end());
					DownloadList.pop_front();
					if (found)
					{
						return;
					}
					DownloadThreads++;
					std::thread download_obj(MeshDownloadFile, tmp.FileUrl, tmp.FileName, navPath.string(), single);
					download_obj.detach();
				}
			}
			else
			{
				MeshWriteChat("\agAll downloads complete or in progress.", true);
				fDownloadReady = false;
			}
		}
	}
}


/**
 * @fn OnZoned
 *
 * This is called after entering a new zone and the zone is considered "loaded."
 *
 * It occurs after @ref OnEndZone @ref OnAddSpawn and @ref OnAddGroundItem have
 * been called.
 */
PLUGIN_API void OnZoned()
{
	if (AutoDownloadMissing)
	{
		std::string fn = std::string(GetShortZone(pLocalPC->zoneId)) + ".navmesh";
		std::string search = fn.substr(0, fn.length() - 8);
		bool found = (std::find(IgnoreList.begin(), IgnoreList.end(), search) != IgnoreList.end());
		if (!found) {
			fs::path p = fs::path(gPathResources) / "MQ2Nav" / fn;
			if (!fs::exists(p, ec))
			{
				std::string cmd = "/mesh updatezone " + std::string(GetShortZone(pLocalPC->zoneId));
				DoCommand(nullptr, cmd.c_str());
			}
		}
	}
}

/**
 * @fn OnUpdateImGui
 *
 * This is called each time that the ImGui Overlay is rendered. Use this to render
 * and update plugin specific widgets.
 *
 * Because this happens extremely frequently, it is recommended to move any actual
 * work to a separate call and use this only for updating the display.
 */
PLUGIN_API void OnUpdateImGui()
{
	/*
		if (GetGameState() == GAMESTATE_INGAME)
		{
			if (ShowMQ2MeshManagerWindow)
			{
				if (ImGui::Begin("MQ2MeshManager", &ShowMQ2MeshManagerWindow, ImGuiWindowFlags_MenuBar))
				{
					if (ImGui::BeginMenuBar())
					{
						ImGui::Text("MQ2MeshManager is loaded!");
						ImGui::EndMenuBar();
					}
				}
				ImGui::End();
			}
		}
	*/
}

/* 
* Filename:  vfae.h
* Description:  header file for VMDK forensics artifact extractor
*/

#include <windows.h>
#include <tchar.h>
#include <process.h>
#include <winbase.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <stdexcept>
#include "md5.h"
#include <direct.h>
#include <io.h>
#include <fstream>
#include <istream>
#include <atlstr.h>
#include "vixDiskLib.h"
#include "vixMntapi.h"
#include <time.h>
#include <string.h>
#pragma comment(lib,"vixDiskLib")
#pragma comment(lib,"vixMntapi")

using std::string;

#define VIXDISKLIB_VERSION_MAJOR 1
#define VIXDISKLIB_VERSION_MINOR 0
#define PROGRAM_VERSION "1.2 beta"
#define THROW_ERROR(vixError) \
	throw VixDiskLibErrWrapper((vixError), __FILE__, __LINE__)

#define CHECK_AND_THROW(vixError)                                    \
	do {                                                              \
	if (VIX_FAILED((vixError))) {                                  \
	throw VixDiskLibErrWrapper((vixError), __FILE__, __LINE__); \
	}                                                              \
	} while (0)

/* function declarations */

void usage(char *);
bool fileExists(char *);
string parseVMDK(char *, char *, char *, int);
int GetFileList(const char *searchkey, std::vector<std::string> &list);
int findDirectories(string, FILE *);
void extractFiles(FILE *, string, char *);
int getMD5(FILE *, char *);

static struct {
	int command;
	VixDiskLibAdapterType adapterType;
	char *diskPath;
	char *parentPath;
	char *metaKey;
	char *metaVal;
	int filler;
	unsigned mbSize;
	VixDiskLibSectorType numSectors;
	VixDiskLibSectorType startSector;
	uint32 openFlags;
	unsigned numThreads;
	Bool success;
	Bool isRemote;
	char *host;
	char *userName;
	char *password;
	int port;
	char *srcPath;
	VixDiskLibConnection connection;
	std::string vmxSpec;
	char *libdir;
	char *ssMoRef;
} appGlobals;

typedef struct {
	VixVolumeHandle volumeHandle;
	VixVolumeInfo* volInfo;
} MountedVolume;

BOOL FolderExists(CString strFolderName)
{   
	return GetFileAttributes(strFolderName) != INVALID_FILE_ATTRIBUTES;   
}

std::string ExtractFilename( const std::string& path )
{
	return path.substr( path.find_last_of( '\\' ) +1 );
}

std::string ExtractDirectory( const std::string& path )
{
	return path.substr( 0, path.find_last_of( '\\' ) +1 );
}
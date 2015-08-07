/*
-  Author:   JD Durick
-  Program:  VFAE (VMDK Artifact Extractor)
-  Website:  http://www.vmforensics.org
-  Email:    labgeek@gmail.com
-  Version   1.2 beta
-  Description:  Console application to copy out files from an off-line VMDK file.  The application allows
the user to conduct a quick triage of the directory structure by outputing the results to a specific output
file.  Additionally, it conducts a MD5 hash value of the VMDK itself if needed.  For specific file searching 
purposes, it searches for any filetype within the off-line VMDK based on a passed in argument via the command-line.
Furthermore, you can extract those file that were found in a hard-coded "Extracted Files" directory as well as
provide the MD5 hash of each file that was extracted.
- 
*/

/*TODO:
- MORE error checking as always
- Get OS information from Virtual machine
-  Assign directory paths based on OS
- add extracted files folder to the command line
*/


#include "vfae.h"
#include <dirent.h>
#define MAX_DATE 24 //max date size of 24
using std::cout;
using std::string;
using std::endl;
using std::vector;
using namespace std;


//Wrapper class for VixDiskLib disk objects
class VixDiskLibErrWrapper
{
public:
	explicit VixDiskLibErrWrapper(VixError errCode, const char* file, int line)
		:
	_errCode(errCode),
		_file(file),
		_line(line)
	{
		char* msg = VixDiskLib_GetErrorText(errCode, NULL);
		_desc = msg;
		VixDiskLib_FreeErrorText(msg);
	}

	VixDiskLibErrWrapper(const char* description, const char* file, int line)
		:
	_errCode(VIX_E_FAIL),
		_desc(description),
		_file(file),
		_line(line)
	{
	}

	string Description() const { return _desc; }
	VixError ErrorCode() const { return _errCode; }
	string File() const { return _file; }
	int Line() const { return _line; }

private:
	VixError _errCode;
	string _desc;
	string _file;
	int _line;
};

/* 
- Main function 
- allows for triage, searching files, and extraction of those files
- allows for MD5 hashing of actual VMDK being analyzed
*/

int main(int argc,char *argv[]){

	MD5 md5 ;
	char *cmd = argv[0];
	char *t = "triage";
	char *searchfiles = "searchfiles";
	char *hashme = "hash";
	int opt_extract = 0;
	string fn;

	//printf("argc = %d\n", argc);

	/* Command line argument handling - eventually needs rewrite */

	if (1==argc) {
		//printf("argc = %d\n", argc);
		usage(cmd);
		return 0;
	}

	/* Takes 3 arguments, i.e. - vfae.exe -d c:\<path to VMDK> */

	if(argc == 3)
	{
		/* string compares for the -d flag */
		if (
			(strcmp("-d", argv[1]) == 0) || 
			(strcmp("-disk", argv[1]) == 0) || 
			(strcmp("--d", argv[1]) == 0) || 
			(strcmp("-DISK", argv[1]) == 0)
			)
		{
			printf("Parsing %s - triage taking place\n", argv[2]);
			fn = parseVMDK(argv[2], NULL, t, opt_extract);
			printf("Please view the %s file for your results.\n", fn.c_str());
			return 0;
		}
	}

	/* matches vfae.exe -d c:\<path to VMDK> -md5 */

	if(argc == 4)
	{
		printf("MD5 hashing taking place...\n");

		if (((strcmp("-d", argv[1]) == 0) || 
			(strcmp("-disk", argv[1]) == 0) || 
			(strcmp("--d", argv[1]) == 0) || 
			(strcmp("-DISK", argv[1]) == 0)
			) 
			&& 
			(strcmp("-md5", argv[3]) == 0)){
				fn = parseVMDK(argv[2], argv[4], hashme, NULL);
				printf("Please view the %s file for your results.\n", fn.c_str());
				return 0;
		}
	}

	if(argc == 5)
	{
		if (((strcmp("-d", argv[1]) == 0) || 
			(strcmp("-disk", argv[1]) == 0) || 
			(strcmp("--d", argv[1]) == 0) || 
			(strcmp("-DISK", argv[1]) == 0)) && 
			(strcmp("-s", argv[3]) == 0) ||
			(strcmp("-search", argv[3]) == 0)||
			(strcmp("--s", argv[3]) == 0)||
			(strcmp("-SEARCH", argv[3]) == 0)
			)
		{
			printf("\nVMDK path to be analyzed = %s\n", argv[2]);
			printf("Search file path to be analyzed = %s\n", argv[4]);
			fn = parseVMDK(argv[2], argv[4], searchfiles, opt_extract);
			//printf("filename  = %s\n", fn.c_str());
			printf("Please view the %s file for your results.\n", fn.c_str());
			return 0;
		}
	}

	if(argc == 6)
	{
		opt_extract = 1;

		if (((strcmp("-d", argv[1]) == 0) || 
			(strcmp("-disk", argv[1]) == 0) || 
			(strcmp("--d", argv[1]) == 0) || 
			(strcmp("-DISK", argv[1]) == 0)) && 
			(strcmp("-s", argv[3]) == 0) ||
			(strcmp("-search", argv[3]) == 0)||
			(strcmp("--s", argv[3]) == 0)||
			(strcmp("-SEARCH", argv[3]) == 0)
			)
		{
			printf("\nPath/Files that will be searched and extracted = %s\n", argv[4]);
			fn = parseVMDK(argv[2], argv[4], searchfiles, opt_extract);
			printf("Please view the %s file for your results.\n", fn.c_str());
			return 0;
		}
	}
	if(argc == 7)
	{
		usage(cmd);
		return 0;
	}


	if ((strcmp("-h", argv[1]) == 0)|| (strcmp("--h", argv[1]) == 0) || (strcmp("-help", argv[1]) == 0))
	{
		usage(cmd);
		return 0;
	}

	if ((strcmp("-v", argv[1]) == 0)|| (strcmp("--v", argv[1]) == 0) || (strcmp("-version", argv[1]) == 0) || (strcmp("--version", argv[1]) == 0) || (strcmp("-ver", argv[1]) == 0))
	{
		printf("VFAE (VMDK Artifact Extractor)\n");
		printf("Version %s\n", PROGRAM_VERSION);
		return 0;
	}


	return 0;
}


/*
Function name:  usage(char *)
Description:    Provides meaningful information to assist the end-user
Input:          pointer to application name
Output:         not a damn thing.
*/
void usage(char *appName){
	cout << "VMDK Forensic Artifact Extractor (VFAE) - <http://www.vmforensics.org>\n"
		"\n"
		"By:  JD Durick <labgeek@gmail.com>\n"
		"Date:  01/09/2012\n"
		"Version:  1.2 Beta\n"
		"Description:  Searches for directories and files from off-line VMDK files using VMware VDDK APIs.\n"
		"Additionally, it extracts selected files from the vmdk being parsed\n"
		"\nusage:  vfae.exe [-h[elp]] [-v[ersion]] [-d[isk] <path to VMDK file>] [-s[earch] <files to search for (wildcards allowed)>]\n"
		"[-md5 <Creates a MD5 value for the .vmdk being anayzed>] [-e[xtract] <extract files from VMDK>]\n"
		"\nSee README.txt for additional help.\n"
		;
	exit(0);
}

/*
Function name:  fileExists 
Description:    Function used from md5.cpp main function to confirm whether or not the file does exist.
Reference:      Thanks to Jim Michaels
*/
bool fileExists(char * fpath) {
#if defined(__BORLANDC__) || defined(_MSC_VER)
	HANDLE h = CreateFile(
		fpath,								// pointer to name of the file
		GENERIC_READ,				// access (read-write) mode
		FILE_SHARE_READ,           // share mode
		NULL,							   // pointer to security attributes
		OPEN_EXISTING,				// how to create
		FILE_ATTRIBUTE_NORMAL,  // file attributes
		INVALID_HANDLE_VALUE         // handle to file with attributes to copy
		);

	if (INVALID_HANDLE_VALUE==h) {
		return false;
	}
	CloseHandle(h);
	return true;
	;



#elif defined(__GNUC__)
	struct stat s;
	int i=stat(fpath, &s);
	return 0==i;
#endif

}

/* 
* Function name:  parseVMDK
* Description:  does the heavy lifting, makes calls to the VDDK API's, mounts and parses volume.
* Extracts files with their MD5 values if required.
* Input:  Pointer to path of VMDK, pointer to directory/file path, pointer to codename, integer that determines
* whether or not the file will be extracted.
* output:  Nothing is passed back.
*/

string parseVMDK(char *path, char *dirfile, char *codename, int opt_extract)
{
	//printf("PATH and dirfile and codename, opt_extract passed = %s and %s and %s and %d\n", path, dirfile, codename, opt_extract);
	FILE *fptr;
	int find_ret = 0;
	time_t rawtime;
	struct tm * timeinfo;
	int  fcounter = 0;
	int successctr = 0;
	int k = 0;
	int failedctr = 0;
	string outputfn = "vfae_output";
	char the_date[MAX_DATE];
	the_date[0] = '\0';
	rawtime = time(NULL);
	strftime(the_date, MAX_DATE, "%m%d%Y_%H%M%S", localtime(&rawtime));
	outputfn += "_";
	outputfn += the_date;
	outputfn += ".txt";
	//printf("outputfn = %s\n", outputfn.c_str());

	fptr = fopen(outputfn.c_str(), "w");  /* output file, appended by time and date  */
	std::fstream fin, fout;


	VixDiskLibConnectParams vxConParams = {0};
	VixDiskLibHandle diskHandle;
	VixDiskLibHandle diskHandles[1];
	VixDiskSetHandle diskSetHandle = NULL;
	VixDiskLibInfo *info = NULL;
	VixOsInfo *osinfo;

	size_t numVolumes = 0; /* set volumes to 0 for init */
	size_t i = 0;
	VixVolumeHandle *volumeHandles = NULL;
	VixVolumeInfo *volInfo = NULL;
	std::vector<MountedVolume> mountedVolumes;
	appGlobals.openFlags = VIXDISKLIB_FLAG_OPEN_READ_ONLY;
	appGlobals.numSectors = 1;

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	appGlobals.diskPath = path;
	try{
		VixError vixError = VixDiskLib_Init(VIXDISKLIB_VERSION_MAJOR,VIXDISKLIB_VERSION_MINOR,NULL,NULL,NULL,appGlobals.libdir);
		CHECK_AND_THROW(vixError);
		vixError = VixMntapi_Init(VIXDISKLIB_VERSION_MAJOR,VIXDISKLIB_VERSION_MINOR,NULL,NULL,NULL,NULL,NULL);
		CHECK_AND_THROW(vixError);
		vixError = VixDiskLib_Connect(&vxConParams,&appGlobals.connection);
		CHECK_AND_THROW(vixError);

		//open the disks you want to mount.
		vixError = VixDiskLib_Open(appGlobals.connection,appGlobals.diskPath,appGlobals.openFlags,&diskHandle);
		CHECK_AND_THROW(vixError);

		diskHandles[0] = diskHandle;

		vixError = VixMntapi_OpenDiskSet(diskHandles,1,appGlobals.openFlags,&diskSetHandle);
		CHECK_AND_THROW(vixError);
		vixError = VixMntapi_GetVolumeHandles(diskSetHandle,&numVolumes,&volumeHandles);

		vixError = VixDiskLib_GetInfo(diskHandle, &info);
		fprintf(fptr, "VFAE Execution Summary Output file\n\n");
		fprintf(fptr, "VMDK Forensic Artifact Extractor (VFAE) - version 1.2 beta\n");
		fprintf(fptr, "By:  JD Durick <labgeek@gmail.com\n");
		fprintf(fptr, "Current local time:  %s\n", asctime(timeinfo));
		/*
		I was getting the MD5 each time it was run, changed that...
		vmdkmd5 = md5.digestFile( path );
		fprintf(fptr, "Opening VMDK %s (%s)\n", path, vmdkmd5);
		*/
		fprintf(fptr, "Total number of sectors:  %d\n", info->capacity);
		fprintf(fptr,"Physical Geometry: %d/%d/%d\n",   info->physGeo.cylinders, info->physGeo.heads, info->physGeo.sectors);

		CHECK_AND_THROW(vixError);
		fprintf(fptr, "Number of total volumes found %d\n", numVolumes);

		volInfo = NULL;
		for (i = 0; i < numVolumes; ++i) {
			MountedVolume newVolume = {0, 0};

			fprintf(fptr, "Mounting volume now.\n");
			vixError = VixMntapi_MountVolume(volumeHandles[i], TRUE);  // set TRUE for isReadOnly (mounted in read only mode)
			CHECK_AND_THROW(vixError);

			fprintf(fptr, "\nRunning VixMntpai_GetOsInfo...\n");
			fprintf(fptr, "==============================\n");
			vixError = VixMntapi_GetOsInfo(diskSetHandle, &osinfo);
			fprintf(fptr, "Major version = %d\n", osinfo->majorVersion);
			fprintf(fptr, "Minor version = %d\n", osinfo->minorVersion);
			fprintf(fptr, "Default location where OS is installed = %s\n", osinfo->osFolder);
			if(osinfo->osIs64Bit)
			{
				fprintf(fptr, "This is a 64-bit operating system\n");
			}
			else
			{
				fprintf(fptr, "This is not 64-bit operating system\n");
			}
			fprintf(fptr, "Vendor = %s\n", osinfo->vendor);


			fprintf(fptr, "\nGetting volume information now\n");
			vixError = VixMntapi_GetVolumeInfo(volumeHandles[i], &newVolume.volInfo);
			CHECK_AND_THROW(vixError);

			fprintf(fptr, "\nMounted Volume %d, Type %d, isMounted %d, symLink %s, numGuestMountPoints %d (%s)\n\n",
				i, newVolume.volInfo->type, newVolume.volInfo->isMounted,
				newVolume.volInfo->symbolicLink == NULL ? "<null>" : newVolume.volInfo->symbolicLink,
				newVolume.volInfo->numGuestMountPoints,
				(newVolume.volInfo->numGuestMountPoints == 1) ? (newVolume.volInfo->inGuestMountPoints[0]) : "<null>" );

			//fprintf(fptr, "\nConfirm whether or not newVolume was mounted\n");
			Bool mountResult = DefineDosDevice(DDD_RAW_TARGET_PATH, "M:", newVolume.volInfo->symbolicLink);

			string mp = newVolume.volInfo->symbolicLink;
			if(strcmp("triage", codename) == 0)
			{
				fprintf(fptr, "\nGet the list of directories found in dirList.txt file\n");
				find_ret = findDirectories(mp, fptr);
				if(find_ret == 1)
				{
					fprintf(fptr, "\ndirList was not found, please make sure the dirList file is in the same directory where vfae was run\n");
					printf("dirList.txt was not found - please make sure it is in the same directory where vfae was run\n");
					exit;
				}
			}
			else if(strcmp("searchfiles", codename) == 0)
			{
				std::vector<std::string> list;
				char gpath[100];
				strcpy(gpath, mp.c_str());
				strcat(gpath, dirfile);
				int count = GetFileList(gpath, list);
				fprintf(fptr, "Total number of files with .exe = %d\n", count);
				fprintf(fptr, "File listing for %s\n", dirfile);
				fprintf(fptr, "=======================\n");
				for(k = 0; k < list.size(); k++)
				{
					fprintf(fptr, "File[%d] = %s\n", k+1, list[k].c_str());
				}

				if(opt_extract == 1)
				{	
					for(k = 0; k < list.size(); k++)
					{
						string nPath = list[k];
						extractFiles(fptr, nPath, gpath);
						//fprintf(fptr, "File[%d] = %s\n", k+1, list[k].c_str());
					}

				}
				return outputfn;
				EXIT_SUCCESS;
			}
			else if(strcmp("hash", codename) == 0)
			{
				getMD5(fptr, path);
				exit;
			}
			else
			{
				printf("Something went wrong dude\n");
				EXIT_SUCCESS;
			}

			VixMntapi_FreeVolumeInfo(newVolume.volInfo);
		}


		//Cleanup for the mounted volumes
		std::vector<MountedVolume>::const_iterator iter = mountedVolumes.begin();
		for (; iter != mountedVolumes.end(); ++iter) {
			VixMntapi_FreeVolumeInfo((*iter).volInfo);
			VixMntapi_DismountVolume((*iter).volumeHandle, TRUE);
		}
		if (volumeHandles) {
			VixMntapi_FreeVolumeHandles(volumeHandles);
		}
		if (appGlobals.connection != NULL) {
			VixDiskLib_Disconnect(appGlobals.connection);
		}  

		VixDiskLib_Exit();
		free(vxConParams.vmxSpec);
	}catch(const VixDiskLibErrWrapper& e) {
		cout << "Error: [" << e.File() << ":" << e.Line() << "]  " <<
			std::hex << e.ErrorCode() << " " << e.Description() << "\n";
	}

}

/* 
* Function name:  getFileList
* Description:    get the list of files from a specific directory 
* Input:          constant char pointer and allocated list
* output:		  integer
*/
int GetFileList(const char *searchkey, std::vector<std::string> &list)
{
	WIN32_FIND_DATA fd;

	HANDLE h = FindFirstFile(searchkey,&fd);

	if(h == INVALID_HANDLE_VALUE)
	{
		return 0; // no files found
	}

	while(1)
	{
		list.push_back(fd.cFileName);
		if(FindNextFile(h, &fd) == FALSE)
			break;
	}
	return list.size();
}

/* 
* Function name:  findDirectories
* Description:    Finds a list of directories (starting point is parent directory)
* Input:          VMDK mount point and file pointer
* output:		  nothing
*/
int findDirectories(string mountPoint, FILE *fp)
{
	std::fstream fin, fout;
	string line = "";
	string fullpath = "";
	fprintf(fp, "Triage starting - Opening file %s\n", "dirList.txt");

	DIR *dirp;
	struct dirent *entry;

	fin.open("dirList.txt", std::ios::in);
	if (!fin.good())
	{
		printf(" dirList was not found. exiting out.  Please install dirList.txt into the local directory.\n");
		return 1;
	}

	fprintf(fp, "In findDirectories now\n");
	fprintf(fp, "Mount point = %s\n", mountPoint.c_str());
	//cout << "mountPoint = " << mountPoint << endl;
	while(!fin.eof())
	{
		std::getline(fin, line, '\n');
		fullpath  = mountPoint + line;
		fprintf(fp, "\nOpening %s\n", line.c_str());
		//cout << "\n" << "Opening the following directory = " << line << "\n" << endl;
		if(dirp = opendir(fullpath.c_str()))
			while(entry = readdir(dirp))
				if ((strcmp(".", entry -> d_name) != 0) || (strcmp("..", entry -> d_name) != 0))
				{	
					fprintf(fp, "%s\n", entry->d_name);
				}
				/*else
				{
				fprintf(fp, "%s\n", entry->d_name);
				}
				*/
				closedir(dirp);
	} 

	return 0;
}

/* 
* Function name:  extractFiles
* Description:    extracts files from the offline VMDK that has been mounted
* Input:          file pointer, string, and pointer to a char
* output:		  nothing
*/
void extractFiles(FILE *filepointer, string npath, char *volPath)
{

	MD5 md5;
	char line[100];
	char *vmdkmd5;

	char  *md5output;

	//printf("Volpath = %s\n", volPath);
	string mainDir = ExtractDirectory(volPath);


	//printf("mainDir = %s\n", mainDir.c_str());
	//printf("path that will be extracted = %s\n", npath.c_str());
	string totalpath = mainDir + npath.c_str();
	//printf("Totalpath = %s\n", totalpath.c_str());
	string mainFile = ExtractFilename(totalpath);
	//printf("mainfile = %s\n", mainFile.c_str());

	size_t sz = 0;
	char *curdir=NULL;
	curdir=getcwd(curdir,sz);
	char *dirname = "Extracted files";

	if(!FolderExists(dirname))
	{
		fprintf(filepointer, "\nCreated the directory: %s\n\n", dirname);
		mkdir(dirname);
		//printf("Directory %s already exists.\n", dirname);
	}

	//fprintf(filepointer, "Location of copied files:  %s\n", curdir);
	string finalpath = "Extracted files\\";
	finalpath += mainFile.c_str();
	if(CopyFile(totalpath.c_str(),finalpath.c_str(),FALSE)){
		strcpy(line, totalpath.c_str());
		md5output = md5.digestFile( line );
		fprintf(filepointer, "File : MD5 that was extracted:  %s : %s\n", npath.c_str(), md5output);
	}

}

/* 
* Function name:  getMD5
* Description:    gets the MD5 hash value for the VMDK provided.
* Input:          file pointer, pointer to a char
* output:		  return value
*/
int getMD5(FILE *fp, char *vmdk_path)
{

	MD5 md5; /* brings in MD5 */
	char *vmdkmd5; /* pointer to 32 character string */
	printf("The VMDK path is: %s\n", vmdk_path);
	printf("MD5 hashing %s...\n", vmdk_path);

	/* need to do test here for valid MD5 hash */
	vmdkmd5 = md5.digestFile( vmdk_path );
	printf("\nVMDK path and MD5: %s:%s\n", vmdk_path, vmdkmd5);
	fprintf(fp, "\nVMDK path and MD5: %s:%s\n", vmdk_path, vmdkmd5);
	return 0;
}


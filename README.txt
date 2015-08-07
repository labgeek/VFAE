VMDK Forensic Artifact Extractor (VFAE)
By:  labgeek@gmail.com
Date:  01/10/2012
Program version:  1.2 beta

VFAE is windows based tool written in C++ that extracts files with a known location from VMDK images running
the Windows operating system.  The tool utilizes the VDDK (Virtual Disk Development Kit) API for the 
heavy lifting such as mounting, opening, and reading the VMDK selected.  When vfae.exe is executed, it copies out
files from an off-line VMDK file.  The application allows the user to conduct a quick triage of the Windows 
directory structure by outputing the results to a specific output file (vfae_output_<localtime>.txt.  Additionally, 
it conducts a MD5 hash value of the VMDK itself if needed.  For specific file searching 
purposes, it searches for any filetype within the off-line VMDK based on a passed in argument via the command-line.
Furthermore, you can extract those file that were found in a hard-coded "Extracted Files" directory as well as
provide the MD5 hash of each file that was extracted.

TODO:
-  add more command line parameters
-  clean up code
-  possibly convert from console line to windows form applicaiton


Example Usage:

1.  vfae.exe -d c:\path\to\vmdk\location.vmdk
2.  vfae.exe -d c:\path\to\vmdk\location.vmdk -md5 [gets the MD5 hash value for the VMDK selected - for verification purposes]
3.  vfae.exe -d c:\path\to\vmdk\location.vmdk -s Windows\*.exe [Parses the off-line VMDK for executable files in the mounted Windows directory]
3a. vfae.exe -d c:\path\to\vmdk\location.vmdk -s Windows\temp\*.* [Searches for any files found in the Windows\temp directory]
4.  vfae.exe -d c:\path\to\vmdk\location.vmdk -s Windows\*.exe -e [Parses the off-line VMDK for executable files in the mounted Windows directory
and extracts the files that were found copying them to the Extracted Files\ directory on your machine.  The Extracted Files directory
is created in the directory where vfae.exe was run]
5.  vfae.exe -v [gets the current version of the application]
6.  vfae.exe -h [help or usuage functionality]

Compiled headers:
-  dirent.h (used for recursive directory searches - windows version)
-  vixdisklib.h (vmware's library for VMware disk access)
-  vixmntapi.h (vmware's library for mounting VMDKs)
-  md5.h (encryption header for MD5 hashing)

Requires:
- vixdisklib.dll file in the home directory where vfae.exe lives
- vixMntapi.dll file in the home directory where vfae.exe lives.
-  dirList.txt file - this file contains a listing of directories that are searched in the off-line VMDK (for triage purposes)

Has been tested on the following OS's:
-  Windows XP SP1-3 (have not had time to test on others)

Example output:

After vfae.exe has run, it produces a vfae_output_<localtime>.txt file in the current
working directory.  Here is an example of the output of that file:

vfae.exe -d c:\jd.vmdk
===================

VFAE Execution Summary Output file

VMDK Forensic Artifact Extractor (VFAE) - version 1.2 beta
By:  JD Durick <labgeek@gmail.com
Current local time:  Sat Dec 31 13:40:30 2011

Total number of sectors:  8388607
Physical Geometry: 1023/64/63
Number of total volumes found 1
Mounting volume now.
Getting volume information now

Mounted Volume 0, Type 1, isMounted 1, symLink \\.\vstor2-mntapi10-shared-FA570500007E00000000000001000000\, numGuestMountPoints 1 (C:)

Get the list of directories found in dirList.txt file
Opening file dirList.txt
In findDirectories now
Mount point = \\.\vstor2-mntapi10-shared-FA570500007E00000000000001000000\

Opening Program Files\
Common Files
ComPlus Applications
Internet Explorer
Messenger
microsoft frontpage
Movie Maker
MSN
MSN Gaming Zone
NetMeeting
Online Services
Outlook Express
Uninstall Information
VMware
Windows Media Player
Windows NT
WindowsUpdate
xerox

Opening Documents and Settings\
All Users
Default User
jd
LocalService
NetworkService

Opening Documents and Settings\Administrator\

Opening Documents and Settings\All Users\
Application Data
Desktop
Documents
DRM
Favorites
Start Menu
Templates

Opening Documents and Settings\Administrator\Local Settings\Temp\

Opening WINDOWS\Temp\
vmware-SYSTEM

Opening \
AUTOEXEC.BAT
boot.ini
CONFIG.SYS
Documents and Settings
IO.SYS
MSDOS.SYS
NTDETECT.COM
ntldr
pagefile.sys
pic1.jpg
Program Files
System Volume Information
temp
Thumbs.db
WINDOWS

For any questions, hit me up at labgeek@gmail.com.


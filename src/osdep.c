/* vim:expandtab:ts=2 sw=2:
*/
/*  Grafx2 - The Ultimate 256-color bitmap paint program

    Copyright 2020 Thomas Bernard
    Copyright 2011 Pawel Góralski
    Copyright 2008 Yves Rizoud
    Copyright 2008 Franck Charlet
    Copyright 2007 Adrien Destugues
    Copyright 1996-2001 Sunset Design (Guillaume Dorme & Karl Maritaud)

    Grafx2 is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; version 2
    of the License.

    Grafx2 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Grafx2; if not, see <http://www.gnu.org/licenses/>
*/
//////////////////////////////////////////////////////////////////////////////
///@file osdep.c
/// OS Dependend code
//////////////////////////////////////////////////////////////////////////////

#if defined(USE_SDL) || defined(USE_SDL2)
#include <SDL.h>
#elif !defined(WIN32)
#include <sys/time.h>
#endif

#if defined(WIN32)
#include <windows.h>
#endif

#if defined(__macosx__) || defined(__FreeBSD__) || defined(__OpenBSD__)
    #include <sys/param.h>
    #include <sys/mount.h>
#elif defined(__NetBSD__)
    #include <sys/statvfs.h>
#elif defined (__linux__) || defined(__SYLLABLE__)
    #include <sys/vfs.h>
#elif defined (__HAIKU__)
	#include "haiku.h"
#elif defined (__MINT__)
    #include <mint/sysbind.h>
    #include <mint/osbind.h>
    #include <mint/ostruct.h>
#elif defined(__AROS__)
    #include <sys/mount.h>
#endif

#if defined(__linux__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
#include <stdlib.h>
#endif

#if defined(__macosx__)
#import <CoreFoundation/CoreFoundation.h>
#import <ApplicationServices/ApplicationServices.h>
#endif

#include "struct.h"
#include "unicode.h"
#include "gfx2log.h"
#include "gfx2mem.h"

dword GFX2_GetTicks(void)
{
#if defined(USE_SDL) || defined(USE_SDL2)
  return SDL_GetTicks();
#elif defined(WIN32)
  return GetTickCount();
#else
  struct timeval tv;
  if (gettimeofday(&tv, NULL) < 0)
    return 0;
  return tv.tv_sec * 1000 + tv.tv_usec / 1000;
#endif
}

void GFX2_OpenURL(const char * buffer, unsigned int len)
{
#if defined(WIN32)
  (void)len;
  /*HINSTANCE hInst = */ShellExecuteA(NULL, "open", buffer, NULL, NULL, SW_SHOWNORMAL);
#elif defined(__macosx__)
  OSStatus ret;

  CFURLRef url = CFURLCreateWithBytes (
      NULL,                   // allocator
      (UInt8*)buffer,         // URLBytes
      len,                    // length
      kCFStringEncodingASCII, // encoding
      NULL                    // baseURL
    );
  ret = LSOpenCFURLRef(url,0);
  if (ret != noErr)
    GFX2_Log(GFX2_ERROR, "LSOpenCFURLRef() returned %d\n", ret);
  CFRelease(url);
#elif defined(__linux__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
  int ret;
  char command[256]; // use the xdg-open command to open the url in the default browser
  (void)len;

  snprintf(command, sizeof(command), "xdg-open \"%s\"", buffer);
  ret = system(command);
  if (ret < 0)
    GFX2_Log(GFX2_ERROR, "system('%s') FAILED\n", command);
  else if (ret == 127)
    GFX2_Log(GFX2_ERROR, "system() FAILED to execute shell\n");
#else
  // TODO : HAIKU, MINT, etc.
  (void)len; (void)buffer;
  GFX2_Log(GFX2_WARNING, "URL open not supported yet on this system.\n");
#endif
}

qword GFX2_DiskFreeSpace(const char * path)
{
#if defined(__WIN32__) || defined(WIN32)
  ULARGE_INTEGER tailleU, totalbytes, totalfreebytes;
  if (GetDiskFreeSpaceExA(path, &tailleU, &totalbytes, &totalfreebytes))
  {
    GFX2_Log(GFX2_DEBUG, "%s: %luMB free for GrafX2 (total %luMB, %luMB free)\n",
             path, (unsigned long)(tailleU.QuadPart >> 20),
             (unsigned long)(totalbytes.QuadPart >> 20), (unsigned long)(totalfreebytes.QuadPart >> 20));
    return tailleU.QuadPart;
  }
  else
  {
    GFX2_Log(GFX2_ERROR, "GetDiskFreeSpaceExA() failed\n");
    return 0;
  }
#elif defined(__linux__) || defined(__macosx__) || defined(__FreeBSD__) || defined(__SYLLABLE__) || defined(__AROS__) || defined(__OpenBSD__)
  struct statfs disk_info;
  statfs(path ,&disk_info);
  return (qword) disk_info.f_bfree * (qword) disk_info.f_bsize;
#elif defined(__NetBSD__)
  struct statvfs disk_info;
  statvfs(path, &disk_info);
  return (qword) disk_info.f_bfree * (qword) disk_info.f_bsize;
#elif defined(__HAIKU__)
  return haiku_get_free_space(path);
#elif defined (__MINT__)
  _DISKINFO drvInfo;

  Dfree(&drvInfo, 0);
  //number of free clusters*sectors per cluster*bytes per sector;
  // reports current drive
  return drvInfo.b_free*drvInfo.b_clsiz*drvInfo.b_secsiz;
#else
  #define NODISKSPACESUPPORT
  // Free disk space is only for shows. Other platforms can display 0.
  #if !defined(__SWITCH__)
  #warning "Missing code for your platform !!! Check and correct please :)"
  #endif
  return 0;
#endif
}

#if defined(__macosx__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__SWITCH__)
  #if defined(__OpenBSD__)
  #include <sys/param.h>
  #endif
  #include <sys/types.h>
  #include <sys/sysctl.h>
#elif defined(__BEOS__) || defined(__HAIKU__)
  #include <kernel/OS.h>
#elif defined(__AROS__) || defined(__amigaos4__) || defined(__MORPHOS__) || defined(__amigaos__)
  #include <proto/exec.h>
#elif defined(__MINT__)
  #include <mint/osbind.h>
  #include <mint/sysbind.h>
#elif defined(__SKYOS__)
  #include <skyos/sysinfo.h>
#elif !defined(WIN32)
  #include <sys/sysinfo.h> // sysinfo() for free RAM
#endif

#if defined (__MINT__)
// atari have two kinds of memory
// standard and fast ram
void Atari_Memory_free(unsigned long *stRam, unsigned long *ttRam){
  *stRam = Mxalloc(-1L, 0);
  *ttRam = Mxalloc(-1L, 1);
}
#else
// Indique quelle est la mémoire disponible
unsigned long Memory_free(void)
{
  // Memory is no longer relevant. If there is ANY problem or doubt here,
  // you can simply return 10*1024*1024 (10Mb), to make the "Pages"something
  // memory allocation functions happy.

  // However, it is still a good idea to make a proper function if you can...
  // If Grafx2 thinks the memory is full, weird things may happen. And if memory
  // ever becomes full and you're still saying there are 10MB free here, the
  // program will crash without saving any picture backup ! You've been warned...
#if defined(WIN32)
#if _WIN32_WINNT >= _WIN32_WINNT_WIN2K
  // GlobalMemoryStatusEx() is supported since Windows 2000
  MEMORYSTATUSEX mstt;
  mstt.dwLength = sizeof(mstt);
  if (GlobalMemoryStatusEx(&mstt))
  {
    GFX2_Log(GFX2_DEBUG, "Phys %lu / %luMB, Page %lu / %luMB, Virtual %lu / %luMB\n",
             (unsigned long)(mstt.ullAvailPhys >> 20), (unsigned long)(mstt.ullTotalPhys >> 20),
             (unsigned long)(mstt.ullAvailPageFile >> 20), (unsigned long)(mstt.ullTotalPageFile >> 20),
             (unsigned long)(mstt.ullAvailVirtual >> 20), (unsigned long)(mstt.ullTotalVirtual >> 20));
    return mstt.ullAvailPhys;
  }
  else
  {
    GFX2_Log(GFX2_ERROR, "GlobalMemoryStatusEx() failed\n");
    return 10*1024*1024;
  }
#else
  MEMORYSTATUS mstt;
  mstt.dwLength = sizeof(MEMORYSTATUS);
  GlobalMemoryStatus(&mstt);
  return mstt.dwAvailPhys;
#endif
#elif defined(__macosx__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
  int mib[2];
  int maxmem;
  size_t len;

  mib[0] = CTL_HW;
  mib[1] = HW_USERMEM;
  len = sizeof(maxmem);
  sysctl(mib,2,&maxmem,&len,NULL,0);
  return maxmem;
#elif defined(__HAIKU__) || defined(__BEOS__)
  int pages;
  system_info systemInfo;
  get_system_info(&systemInfo);

  pages = systemInfo.max_pages - systemInfo.used_pages;
  return pages * B_PAGE_SIZE;
#elif defined(__AROS__) || defined(__MORPHOS__) || defined(__amigaos__)
  return AvailMem(MEMF_ANY);
#elif defined(__linux__)
  struct sysinfo info;
  sysinfo(&info);
  return info.freeram*info.mem_unit;
#else
  // AvailMem is misleading on os4 (os4 caches stuff in memory that you can still allocate)
#if defined(__SWITCH__)
  // There is some way to get memory information on switch (see include switch/kernel/svc.h svcGetInfo svcGetSystemInfo)
  // but the usage is a bit confusing for the first and the later need privilege to be used.
  // If you come here with a solution, you'r welcome. For now we just return the default value.
#elif
#warning "There is missing code there for your platform ! please check and correct :)"
#endif
  return 10*1024*1024;
#endif
}
#endif

#if defined(WIN32)
/**
 * Converts an unicode string to UTF8
 * @param str the unicode string
 * @param utf8len pointer to receive the utf8 string length (excluding null terminator)
 * @return malloc'ed UTF8 string
 */
char * Unicode_to_utf8(const word * str, size_t * utf8len)
{
  size_t unicode_len = Unicode_strlen(str);
  int len = WideCharToMultiByte(CP_UTF8, 0, str, unicode_len, NULL, 0, NULL, NULL);
  if (len <= 0)
  {
    GFX2_Log(GFX2_WARNING, "WideCharToMultiByte() failed\n");
    return NULL;
  }
  else
  {
    char * utf8 = (char *)GFX2_malloc(len+1);
    if (utf8 == NULL)
      return NULL;
    if (WideCharToMultiByte(CP_UTF8, 0, str, unicode_len, utf8, len, NULL, NULL) <= 0)
    {
      DWORD error = GetLastError();
      GFX2_Log(GFX2_WARNING, "WideCharToMultiByte() failed error=%u\n", error);
      free(utf8);
      return NULL;
    }
    if (utf8len != NULL)
      *utf8len = (size_t)len;
    utf8[len] = '\0';
    return utf8;
  }
}
#endif

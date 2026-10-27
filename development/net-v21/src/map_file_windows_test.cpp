// Real Win32 atomic save replacement + CreateProcess and rollback; no game or Steam.
#include "map_file_windows.h"
#include <stdio.h>
#include <string.h>
#include <vector>
using namespace map_file_windows;
static int checks, failures;
static void test(bool b,const char *name) { ++checks; if(!b){++failures;printf("FAIL %s (Windows %lu)\n",name,GetLastError());} }
int main(int argc,char **argv) {
    if(argc>1 && !strcmp(argv[1],"--child")) return 0;
    char temp[MAX_PATH]={},dir[MAX_PATH]={},self[MAX_PATH]={};
    if(!GetTempPathA(MAX_PATH,temp) || !GetTempFileNameA(temp,"mop",0,dir)) return 2;
    if(!DeleteFileA(dir) || !CreateDirectoryA(dir,nullptr) || !GetModuleFileNameA(nullptr,self,MAX_PATH)) return 2;
    std::string path=std::string(dir)+"\\save.opsave";
    const std::vector<unsigned char> old={1,2,3,4}, fresh={8,7,6,5,4};
    test(atomic_file::create_new(path.c_str(),old.data(),old.size()),"create original");
    std::string backup; DWORD err=0; std::vector<unsigned char> readback;
    auto r=replace_and_launch(dir,"\"Z:\\no-such-program.exe\"",path.c_str(),fresh,&backup,&err);
    test(r==map_file::Result::launch_rolled_back && err!=0,"failed process creation rolls back");
    test(read(nullptr,path.c_str(),&readback) && readback==old,"original bytes restored after launch failure");
    test(read(nullptr,backup.c_str(),&readback) && readback==old,"first backup survives launch failure");
    bool marker=false;
    test(exists(nullptr,(path+".mp_transaction").c_str(),&marker) && !marker,"successful rollback cleans marker");
    std::string old_backup=backup;
    std::string cmd=std::string("\"")+self+"\" --child";
    r=replace_and_launch(dir,cmd,path.c_str(),fresh,&backup,&err);
    test(map_file::started(r),"a real child process starts after replacement");
    test(read(nullptr,path.c_str(),&readback) && readback==fresh,"started process sees replaced bytes on disk");
    test(read(nullptr,backup.c_str(),&readback) && readback==old && backup!=old_backup,"success has a distinct verified backup");
    test(atomic_file::create_new((path+".mp_transaction").c_str(),"blocked",7),"create synthetic unresolved marker");
    r=replace_and_launch(dir,cmd,path.c_str(),old,&backup,&err);
    test(r==map_file::Result::unresolved && read(nullptr,path.c_str(),&readback) && readback==fresh,
         "unresolved marker blocks further replacement");
    DeleteFileA((path+".mp_transaction").c_str()); DeleteFileA(path.c_str());
    WIN32_FIND_DATAA fd={}; std::string mask=std::string(dir)+"\\*";
    HANDLE h=FindFirstFileA(mask.c_str(),&fd);
    if(h!=INVALID_HANDLE_VALUE) { do { if(fd.cFileName[0]!='.') DeleteFileA((std::string(dir)+"\\"+fd.cFileName).c_str()); } while(FindNextFileA(h,&fd)); FindClose(h); }
    RemoveDirectoryA(dir);
    printf("map file Windows selftest: %d checks, %d failed\n",checks,failures); return failures?1:0;
}

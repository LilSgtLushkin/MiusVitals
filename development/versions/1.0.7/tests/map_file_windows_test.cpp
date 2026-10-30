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
    if(argc>1 && !strcmp(argv[1],"--check-environment")) {
        char v[32]={};
        return GetEnvironmentVariableA("MP_NETV21_SELFTEST_ONLY",v,sizeof v) && !strcmp(v,"child-value") ? 0 : 1;
    }
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
    const char *env_name="MP_NETV21_SELFTEST_ONLY";
    test(SetEnvironmentVariableA(env_name,"parent-value")!=0,"set parent fixture environment");
    {
        Environment env;
        test(env.set(env_name,"child-value"),"set child environment");
        test(env.set("MP_NETV21_ABSENT_TEST",nullptr),"clearing an absent key is safe");
        std::string command=std::string("\"")+self+"\" --check-environment";
        std::vector<char> mutable_cmd(command.begin(),command.end()); mutable_cmd.push_back(0);
        STARTUPINFOA si={sizeof si}; PROCESS_INFORMATION pi={};
        bool launched=CreateProcessA(nullptr,mutable_cmd.data(),nullptr,nullptr,FALSE,0,nullptr,dir,&si,&pi)!=0;
        test(launched,"child starts with prepared environment");
        if(launched) {
            DWORD status=STILL_ACTIVE;
            bool done=WaitForSingleObject(pi.hProcess,5000)==WAIT_OBJECT_0 && GetExitCodeProcess(pi.hProcess,&status);
            test(done && status==0,"child inherited the intended value");
            CloseHandle(pi.hProcess); CloseHandle(pi.hThread);
        }
        test(env.restore(),"restore parent environment after successful start");
    }
    char value[32]={};
    test(GetEnvironmentVariableA(env_name,value,sizeof value) && !strcmp(value,"parent-value"),"parent value restored");
    {
        Environment env;
        test(env.set(env_name,"temporary"),"prepare before invalid environment key");
        test(!env.set("INVALID=NAME","x"),"invalid key fails closed");
    }
    memset(value,0,sizeof value);
    test(GetEnvironmentVariableA(env_name,value,sizeof value) && !strcmp(value,"parent-value"),"failed preparation restores earlier changes");
    SetEnvironmentVariableA(env_name,nullptr);
    DeleteFileA((path+".mp_transaction").c_str()); DeleteFileA(path.c_str());
    WIN32_FIND_DATAA fd={}; std::string mask=std::string(dir)+"\\*";
    HANDLE h=FindFirstFileA(mask.c_str(),&fd);
    if(h!=INVALID_HANDLE_VALUE) { do { if(fd.cFileName[0]!='.') DeleteFileA((std::string(dir)+"\\"+fd.cFileName).c_str()); } while(FindNextFileA(h,&fd)); FindClose(h); }
    RemoveDirectoryA(dir);
    printf("map file Windows selftest: %d checks, %d failed\n",checks,failures); return failures?1:0;
}

// Fault injection for the production transaction, without a game or filesystem.
#include "map_file.h"
#include <map>
#include <string>
#include <stdio.h>
using namespace map_file;
struct Fake {
    std::map<std::string,std::vector<byte>> files;
    std::string fail;
    bool launch_ok=true, launched=false;
    int writes=0;
};
static bool exists(void *p,const char *name,bool *out) { auto &f=*(Fake *)p; if(f.fail=="inspect")return false; *out=f.files.count(name)!=0; return true; }
static bool read(void *p,const char *name,std::vector<byte> *out) {
    auto &f=*(Fake *)p; std::string n=name;
    if ((f.fail=="read" && n=="save") || (f.fail=="verify" && n=="backup") || !f.files.count(n)) return false;
    *out=f.files[n]; return true;
}
static bool create(void *p,const char *name,const void *data,size_t size) {
    auto &f=*(Fake *)p; std::string n=name;
    if ((f.fail=="backup" && n=="backup") || (f.fail=="marker" && n.find(".mp_transaction")!=n.npos) || f.files.count(n)) return false;
    f.files[n]=std::vector<byte>((const byte *)data,(const byte *)data+size); return true;
}
static bool write(void *p,const char *name,const void *data,size_t size) {
    auto &f=*(Fake *)p; ++f.writes;
    if ((f.fail=="image" && f.writes==1) || (f.fail=="rollback" && f.writes==2)) return false;
    f.files[name]=std::vector<byte>((const byte *)data,(const byte *)data+size); return true;
}
static bool remove_file(void *p,const char *name) {
    auto &f=*(Fake *)p;
    if (f.fail=="cleanup") return false;
    return f.files.erase(name)==1;
}
static bool launch(void *p) { auto &f=*(Fake *)p; f.launched=true; return f.launch_ok; }
static Io io(Fake &f) { return {&f,exists,read,create,write,remove_file,launch}; }
static int checks, failures;
static void test(bool ok,const char *what) { ++checks; if(!ok){++failures;printf("FAIL %s\n",what);} }
int main() {
    const std::vector<byte> old={1,2,3}, replacement={7,8,9,10};
    Fake f; f.files["save"]=old;
    Result r=replace_and_launch(io(f),"save","backup",replacement);
    test(r==Result::ok && f.launched && f.files["save"]==replacement && f.files["backup"]==old && !f.files.count("save.mp_transaction"),"success keeps verified original backup");
    f=Fake(); r=replace_and_launch(io(f),"save","backup",replacement);
    test(r==Result::ok && f.launched && f.files["save"]==replacement && !f.files.count("backup"),"success when destination did not exist");
    f=Fake();f.files["save"]=old;f.fail="inspect";
    test(replace_and_launch(io(f),"save","backup",replacement)==Result::inspect && f.files["save"]==old && !f.launched,"inspect failure leaves original");
    f=Fake();f.files["save"]=old;f.files["save.mp_transaction"]={1};
    test(replace_and_launch(io(f),"save","backup",replacement)==Result::unresolved && f.files["save"]==old && !f.launched,"existing recovery marker blocks new transaction");
    f=Fake();f.files["save"]=old;f.fail="read";
    test(replace_and_launch(io(f),"save","backup",replacement)==Result::read && f.files["save"]==old && !f.launched,"failed original read stops");
    f=Fake();f.files["save"]=old;f.fail="backup";
    test(replace_and_launch(io(f),"save","backup",replacement)==Result::backup && f.files["save"]==old && !f.launched,"failed backup stops");
    f=Fake();f.files["save"]=old;f.files["backup"]={4};
    test(replace_and_launch(io(f),"save","backup",replacement)==Result::backup && f.files["backup"]==std::vector<byte>({4}),"never overwrite an older backup");
    f=Fake();f.files["save"]=old;f.fail="verify";
    test(replace_and_launch(io(f),"save","backup",replacement)==Result::verify_backup && f.files["save"]==old && !f.launched,"failed backup readback stops");
    f=Fake();f.files["save"]=old;f.fail="marker";
    test(replace_and_launch(io(f),"save","backup",replacement)==Result::marker && f.files["save"]==old && f.files["backup"]==old && !f.launched,"failed recovery marker stops with original and backup intact");
    f=Fake();f.files["save"]=old;f.fail="image";
    test(replace_and_launch(io(f),"save","backup",replacement)==Result::image && f.files["save"]==old && !f.files.count("save.mp_transaction") && !f.launched,"failed image write leaves original and removes marker");
    f=Fake();f.files["save"]=old;f.launch_ok=false;
    test(replace_and_launch(io(f),"save","backup",replacement)==Result::launch_rolled_back && f.files["save"]==old && f.files["backup"]==old && !f.files.count("save.mp_transaction"),"failed launch atomically restores original");
    f=Fake();f.launch_ok=false;
    test(replace_and_launch(io(f),"save","backup",replacement)==Result::launch_rolled_back && !f.files.count("save"),"failed launch removes newly created destination");
    f=Fake();f.files["save"]=old;f.launch_ok=false;f.fail="rollback";
    test(replace_and_launch(io(f),"save","backup",replacement)==Result::launch_rollback_failed && f.files["backup"]==old && f.files.count("save.mp_transaction"),"rollback failure retains backup and marker");
    f=Fake();f.files["save"]=old;f.launch_ok=false;f.fail="cleanup";
    test(replace_and_launch(io(f),"save","backup",replacement)==Result::launch_cleanup && f.files["save"]==old && f.files.count("save.mp_transaction"),"failed cleanup after rollback reported");
    f=Fake();f.files["save"]=old;f.fail="cleanup";
    r=replace_and_launch(io(f),"save","backup",replacement);
    test(r==Result::ok_marker_retained && started(r) && f.files["save"]==replacement && f.files["backup"]==old && f.files.count("save.mp_transaction"),"failed cleanup after successful start is not reported as launch failure");
    f=Fake();f.files["save"]=old;
    test(replace_and_launch(io(f),"save","save",replacement)==Result::invalid && !f.launched,"backup must differ from target");
    test(replace_and_launch(io(f),"save","backup",{})==Result::invalid && !f.launched,"empty replacement refused");
    printf("map file transaction selftest: %d checks, %d failed\n",checks,failures); return failures?1:0;
}

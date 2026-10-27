// Synthetic AZCF-only fixtures for no-game integration tests. Not compiled into the shipping module.
#pragma once
#include "opsave_checked.h"
#include <string>
#include <vector>
#include <string.h>
namespace opsave_fixture {
using byte = unsigned char;
inline void put(std::vector<byte> &b, size_t at, unsigned n) { memcpy(b.data() + at, &n, 4); }
inline size_t align64(size_t n) { return (n + 63) & ~(size_t)63; }
struct Cell { std::string field, text; unsigned value = 0, kind = 4; };
struct Node { std::string name; unsigned type = 1, cols = 1; std::vector<Cell> cells; };
inline Cell num(const char *field, unsigned n) { Cell c; c.field=field; c.value=n; return c; }
inline Cell str(const char *text) { Cell c; c.kind=2; c.text=text; return c; }
inline std::vector<byte> build(const std::vector<Node> &nodes) {
    size_t at=align64(0x40+nodes.size()*0x80); std::vector<size_t> offsets;
    for (const auto &n:nodes) { offsets.push_back(at); at+=align64(n.cells.size()*(n.type==1?0x40:0x20)); }
    std::vector<byte> b(at,0); memcpy(b.data(),"AZCF",4); put(b,4,(unsigned)nodes.size()); put(b,8,0x40);
    memcpy(b.data()+0x20,"test_rus",8);
    for(size_t i=0;i<nodes.size();++i) {
        const Node &n=nodes[i]; size_t o=0x40+i*0x80; memcpy(b.data()+o,n.name.data(),n.name.size());
        put(b,o+0x20,n.type);put(b,o+0x24,n.cols);put(b,o+0x28,n.type==1?(unsigned)n.cells.size():(unsigned)n.cells.size()/n.cols);
        put(b,o+0x30,(unsigned)offsets[i]);
        for(size_t j=0;j<n.cells.size();++j) {
            size_t c=offsets[i]+j*(n.type==1?0x40:0x20); const Cell &v=n.cells[j]; put(b,c+16,v.kind);
            if(n.type==1)memcpy(b.data()+c+0x20,v.field.data(),v.field.size());
            if(v.kind==2) { put(b,c,(unsigned)v.text.size());put(b,c+4,(unsigned)b.size());b.insert(b.end(),v.text.begin(),v.text.end());b.push_back(0); }
            else put(b,c,v.value);
        }
    }
    // Incompressible-ish trailing bytes make the real transfer span several pieces.
    for(unsigned i=0;i<5000;++i)b.push_back((byte)((i*31+7)&255));
    return b;
}
inline std::vector<byte> host(bool ordered) {
    Node h;h.name="p_host";h.cells={num("ai",opsave::k_user),num("side",0x00524547),num("vis_mode",3),num("px",4),num("pz",4)};
    Node g;g.name="p_guest";g.cells={num("ai",0x314e45),num("side",0x52535355),num("vis_mode",1),num("px",5),num("pz",4)};
    Node op;op.name="op_stream";op.cells={num("turn_phase",0)};
    Node frames;frames.name="frames";frames.type=2;frames.cols=1;frames.cells={num("",0)};
    std::vector<Node> nodes={h,g,op,frames};
    if(ordered){Node orders;orders.name="user_orders";orders.type=2;orders.cols=2;orders.cells={num("",100),str("host move")};nodes.push_back(orders);}
    return build(nodes);
}
// Synthetic host-authored tactical description. The two squads let the no-game test
// verify that the guest can command its own units after the host starts a battle.
inline std::vector<byte> battle() {
    const unsigned ger=0x00524547, ussr=0x52535355, user=0x52455355, enem=0x4d454e45;
    Node game; game.name="i_gamespec:gamespec"; game.cells={num("user_side",ger),num("enemy_side",ussr)};
    Node squads; squads.name="squads"; squads.type=2; squads.cols=4;
    squads.cells={num("",0),num("",0x100),num("",ger),num("",user),
                  num("",0),num("",0x200),num("",ussr),num("",enem)};
    Node units; units.name="units"; units.type=2; units.cols=1;
    units.cells={str("i_human:hum_00000100"),str("i_human:hum_00000200")};
    return build({game,squads,units});
}
inline opsave::NodeRef node(const std::vector<byte> &b,const char *name){for(auto &n:opsave::nodes(b))if(n.name==name)return n;return {};}
inline size_t field(const std::vector<byte> &b,const char *name,const char *wanted){
    auto n=node(b,name);for(unsigned i=0;i<n.rows;++i){size_t c=n.data+(size_t)i*0x40;if(!strncmp((char *)b.data()+c+0x20,wanted,0x20))return c;}return 0;
}
inline bool guest_order(std::vector<byte> *p) {
    if (!p || !opsave_checked::validate(*p)) return false;
    auto &b=*p; auto n=node(b,"user_orders");
    if(n.type!=2 || n.cols!=2 || !n.rows)return false;
    put(b,field(b,"p_guest","px"),7);
    size_t old=(size_t)n.rows*n.cols*0x20; std::vector<byte> rows(b.begin()+n.data,b.begin()+n.data+old);
    rows.resize(old+n.cols*0x20,0);put(rows,old,200);put(rows,old+16,4);put(rows,old+0x20,10);put(rows,old+0x20+16,2);
    size_t offset=b.size();put(rows,old+0x20+4,(unsigned)(offset+rows.size()));b.insert(b.end(),rows.begin(),rows.end());
    const char guest[]="guest move";b.insert(b.end(),guest,guest+sizeof guest);put(b,n.header+0x28,n.rows+1);put(b,n.header+0x30,(unsigned)offset);
    opsave::set_operation_name(b,"test_ger");return opsave_checked::validate(b);
}
}

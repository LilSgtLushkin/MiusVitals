// Synthetic AZCF fixtures: these are NOT real saves from the game.
#if !defined(_WIN32)
#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
static int fopen_s(FILE **f, const char *p, const char *m) { *f = fopen(p, m); return *f ? 0 : errno; }
// Only the legacy unused text-sidecar helpers need fscanf_s on non-MSVC compilers.
static int fscanf_s(FILE *, const char *, ...) { return EOF; }
#endif
#include "opsave_checked.h"
#include "spectator_snapshot.h"
#include <stdio.h>
#include <string.h>
#include <vector>
#include <string>
using namespace opsave_checked;
static int checks, failures;
static void test(bool ok, const char *what) { ++checks; if (!ok) { ++failures; printf("FAIL %s\n", what); } }
static void put(std::vector<byte> &b, size_t off, unsigned x) { memcpy(b.data() + off, &x, 4); }
static size_t align64(size_t n) { return (n + 63) & ~(size_t)63; }
struct Cell { std::string field, text; unsigned value = 0, kind = 4; };
struct Node { std::string name; unsigned type = 1, cols = 1; std::vector<Cell> cells; };
static Cell num(const char *field, unsigned value) { Cell c; c.field = field; c.value = value; return c; }
static Cell text(const char *value) { Cell c; c.kind = 2; c.text = value; return c; }
static std::vector<byte> build(const std::vector<Node> &nodes) {
    size_t at = align64(0x40 + nodes.size() * 0x80);
    std::vector<size_t> offsets;
    for (const auto &n : nodes) { offsets.push_back(at); at += align64(n.cells.size() * (n.type == 1 ? 0x40 : 0x20)); }
    std::vector<byte> b(at, 0);
    memcpy(b.data(), "AZCF", 4); put(b, 4, (unsigned)nodes.size()); put(b, 8, 0x40);
    memcpy(b.data() + 0x20, "test_rus", 8);
    for (size_t i = 0; i < nodes.size(); ++i) {
        const Node &n = nodes[i]; size_t o = 0x40 + i * 0x80;
        memcpy(b.data() + o, n.name.data(), n.name.size());
        put(b, o + 0x20, n.type); put(b, o + 0x24, n.cols);
        put(b, o + 0x28, n.type == 1 ? (unsigned)n.cells.size() : (unsigned)n.cells.size() / n.cols);
        put(b, o + 0x30, (unsigned)offsets[i]);
        for (size_t j = 0; j < n.cells.size(); ++j) {
            size_t c = offsets[i] + j * (n.type == 1 ? 0x40 : 0x20);
            const Cell &v = n.cells[j]; put(b, c + 16, v.kind);
            if (n.type == 1) memcpy(b.data() + c + 0x20, v.field.data(), v.field.size());
            if (v.kind == 2) {
                put(b, c, (unsigned)v.text.size()); put(b, c + 4, (unsigned)b.size());
                b.insert(b.end(), v.text.begin(), v.text.end()); b.push_back(0);
            } else put(b, c, v.value);
        }
    }
    return b;
}
static std::vector<Node> fixture(bool with_orders = true) {
    Node h; h.name = "p_host"; h.cells = {num("ai", opsave::k_user), num("side", 0x00524547), num("vis_mode", 3), num("px", 4), num("pz", 4)};
    Node g; g.name = "p_guest"; g.cells = {num("ai", 0x314e45), num("side", 0x52535355), num("vis_mode", 1), num("px", 5), num("pz", 4)};
    Node op; op.name = "op_stream"; op.cells = {num("turn_phase", 0)};
    Node frames; frames.name = "frames"; frames.type = 2; frames.cols = 1; frames.cells = {num("", 0)};
    std::vector<Node> nodes = {h,g,op,frames};
    if (with_orders) { Node orders; orders.name = "user_orders"; orders.type = 2; orders.cols = 2; orders.cells = {num("", 100), text("host move")}; nodes.push_back(orders); }
    return nodes;
}
static opsave::NodeRef node(const std::vector<byte> &b, const char *name) {
    for (auto &n : opsave::nodes(b)) if (n.name == name) return n;
    return {};
}
static size_t field(const std::vector<byte> &b, const char *name, const char *wanted) {
    auto n = node(b, name);
    for (unsigned i=0; i<n.rows; ++i) {
        size_t c=n.data+(size_t)i*0x40;
        if (!strncmp((const char *)b.data()+c+0x20,wanted,0x20)) return c;
    }
    return 0;
}
static void append_order(std::vector<byte> &b) {
    auto n = node(b, "user_orders");
    size_t old = (size_t)n.rows * n.cols * 0x20;
    std::vector<byte> rows(b.begin()+n.data, b.begin()+n.data+old);
    rows.resize(old + n.cols * 0x20, 0);
    put(rows, old, 200); put(rows, old+16, 4);
    put(rows, old+0x20, 10); put(rows, old+0x20+16, 2);
    size_t offset = b.size();
    put(rows, old+0x20+4, (unsigned)(offset+rows.size()));
    b.insert(b.end(),rows.begin(),rows.end());
    const char guest[] = "guest move"; b.insert(b.end(),guest,guest+sizeof guest);
    put(b,n.header+0x28,n.rows+1); put(b,n.header+0x30,(unsigned)offset);
}
static void expect_rejected(const std::vector<byte> &base, const std::vector<byte> &bad, const Sidecar &side, const char *msg) {
    std::vector<byte> out(5, 0xa5); unsigned added = 999; std::string why;
    bool ok = accept(base,bad,side,&out,&added,&why);
    test(!ok && out == std::vector<byte>(5,0xa5) && added == 999 && !why.empty(),msg);
}
int main(int argc, char **argv) {
    std::string why; unsigned changed = 0, added = 0;
    auto host = build(fixture());
    test(validate(host,&why),"valid synthetic save");
    test(spectator_snapshot::publishable(host, false, 0, {}), "initial exact host snapshot allowed");
    test(!spectator_snapshot::publishable(host, true, opsave::turns(host), host), "unchanged host snapshot is not republished");
    auto battles_pending = host; put(battles_pending, field(battles_pending, "op_stream", "turn_phase"), 2);
    test(spectator_snapshot::publishable(battles_pending, true, opsave::turns(host), host), "changed phase 2 host snapshot is publishable");
    auto interim = host; put(interim, field(interim, "op_stream", "turn_phase"), 1);
    test(!spectator_snapshot::publishable(interim, false, 0, {}), "intermediate phase is refused");
    auto unadvanced = host; put(unadvanced, field(unadvanced, "p_host", "px"), 9);
    test(!spectator_snapshot::publishable(unadvanced, true, opsave::turns(host), host), "phase 0 update without a new frame is refused");
    auto bad_snapshot = host; bad_snapshot.resize(16);
    test(!spectator_snapshot::publishable(bad_snapshot, false, 0, {}), "truncated snapshot is refused");
    auto wrong_phase_type=host; put(wrong_phase_type,field(wrong_phase_type,"op_stream","turn_phase")+16,2);
    test(opsave::turn_phase(wrong_phase_type)==-1,"non-numeric turn phase never confirms resolution");
    Sidecar side; auto base = host;
    test(mirror(base,&side,&changed,&why) && changed==2,"checked mirror of two sides");
    test(side.ai.size()==2 && side.vis.size()==2,"sidecar preserves both ownership and visibility");
    test(equivalent(base,base,&why),"unchanged host snapshot is equivalent");
    auto changed_host=base; put(changed_host,field(changed_host,"p_host","px"),55);
    test(!equivalent(base,changed_host,&why),"host edits after handoff are detected");
    auto guest=base; put(guest,field(guest,"p_guest","px"),7); append_order(guest);
    opsave::set_operation_name(guest,"test_ger");
    std::vector<byte> result;
    test(accept(base,guest,side,&result,&added,&why) && added==1,"whole guest save accepted after ordered handoff");
    test(opsave::u32_at(result,field(result,"p_guest","px"))==7,"guest's move kept");
    test(opsave::u32_at(result,field(result,"p_host","ai"))==opsave::k_user &&
         opsave::u32_at(result,field(result,"p_guest","ai"))==0x314e45,"host ownership restored");
    test(opsave::u32_at(result,field(result,"p_host","vis_mode"))==3 &&
         opsave::u32_at(result,field(result,"p_guest","vis_mode"))==1,"host visibility restored exactly");
    test(opsave::operation_name(result)=="test_rus" && opsave::rows_of(result,"user_orders")==2,"host operation and both orders kept");
    auto processed=result;
    test(opsave::all_player_led(processed)==1 && restore_after_turn(processed,side,&changed,&why) &&
         opsave::u32_at(processed,field(processed,"p_guest","ai"))==0x314e45 &&
         opsave::u32_at(processed,field(processed,"p_guest","vis_mode"))==1,"post-turn ownership and host visibility restored");
    auto before_failed=processed; auto incomplete_after=side; incomplete_after.ai.erase("p_guest"); incomplete_after.vis.erase("p_guest");
    test(!restore_after_turn(processed,incomplete_after,&changed,&why) && processed==before_failed,"unknown post-turn group refused without mutation");
    auto orders=node(result,"user_orders"); size_t text_cell=orders.data+3*0x20;
    unsigned off=opsave::u32_at(result,text_cell+4);
    test(off<result.size() && strcmp((char *)result.data()+off,"guest move")==0,"guest text offset remains valid in whole file");

    auto relocated=guest; orders=node(relocated,"user_orders"); size_t hc=orders.data+0x20;
    put(relocated,hc+4,(unsigned)relocated.size()); const char hm[]="host move"; relocated.insert(relocated.end(),hm,hm+sizeof hm);
    test(accept(base,relocated,side,&result,&added,&why),"re-serialized unchanged host text is compared by value");
    auto bad=guest; orders=node(bad,"user_orders"); put(bad,orders.data,101);
    expect_rejected(base,bad,side,"modified host order refused without mutating output");
    bad=guest; orders=node(bad,"user_orders"); put(bad,orders.header+0x28,0);
    expect_rejected(base,bad,side,"removed host orders refused");
    bad=guest; put(bad,field(bad,"p_host","px"),99);
    expect_rejected(base,bad,side,"guest cannot move host's group");
    bad=guest; put(bad,field(bad,"p_guest","ai"),123);
    expect_rejected(base,bad,side,"changed guest ownership refused");
    bad=guest; put(bad,field(bad,"p_guest","side"),0x00524547);
    expect_rejected(base,bad,side,"changed side refused");
    bad=guest; opsave::set_operation_name(bad,"another_rus");
    expect_rejected(base,bad,side,"another operation refused");
    bad=guest; auto gn=node(bad,"p_guest"); bad[gn.data+0x20]='X';
    expect_rejected(base,bad,side,"changed field layout refused");
    bad=guest; auto n=node(bad,"frames"); put(bad,n.header+0x28,2);
    expect_rejected(base,bad,side,"unsupported table growth refused");
    bad=guest; n=node(bad,"frames"); put(bad,n.data,999);
    expect_rejected(base,bad,side,"guest cannot rewrite existing frame values");
    bad=guest; put(bad,field(bad,"op_stream","turn_phase"),2);
    expect_rejected(base,bad,side,"guest cannot advance the global turn phase");
    bad=guest; n=node(bad,"p_guest"); put(bad,n.header+0x20,99);
    expect_rejected(base,bad,side,"unknown node type refused");
    bad=guest; n=node(bad,"p_guest"); put(bad,n.header+0x30,0xffffffff);
    expect_rejected(base,bad,side,"out of bounds node refused");
    bad=guest; n=node(bad,"p_guest"); put(bad,n.data+16,7);
    expect_rejected(base,bad,side,"unknown cell kind refused");
    bad=guest; n=node(bad,"user_orders"); put(bad,n.data+0x20+4,0xffffffff);
    expect_rejected(base,bad,side,"invalid text pointer refused");
    bad=guest; put(bad,4,0xffffffff); expect_rejected(base,bad,side,"oversized node table refused");
    bad=guest; bad.resize(20); expect_rejected(base,bad,side,"truncated file refused");
    Sidecar incomplete=side; incomplete.vis.erase("p_host"); expect_rejected(base,guest,incomplete,"missing visibility refused");
    incomplete=side; incomplete.ai.erase("p_guest"); incomplete.vis.erase("p_guest"); expect_rejected(base,guest,incomplete,"missing ownership refused");
    incomplete=side; incomplete.ai["new group"]=99; expect_rejected(base,guest,incomplete,"unknown sidecar group refused");

    std::vector<byte> packed; Sidecar decoded;
    test(encode_sidecar(side,&packed) && decode_sidecar(packed,&decoded) && decoded.code==side.code &&
         decoded.ai==side.ai && decoded.vis==side.vis,"binary sidecar round trip retains ai and visibility");
    auto short_side=packed; short_side.pop_back(); test(!decode_sidecar(short_side,&decoded),"truncated sidecar refused");
    auto trailing=packed; trailing.push_back(0); test(!decode_sidecar(trailing,&decoded),"sidecar trailing data refused");
    auto unknown=packed; unknown[24]=2; test(!decode_sidecar(unknown,&decoded),"sidecar unknown flags refused");
    auto empty=side; empty.vis["not a group"]=1; test(!encode_sidecar(empty,&packed),"orphan visibility refused");
    auto repeated=fixture(); repeated[1].name="p_host"; auto dup=build(repeated); Sidecar s;
    test(!mirror(dup,&s,&changed,&why),"ambiguous tagged group names refused");
    auto missing=fixture(); missing[1].cells[1].value=0; auto no_side=build(missing);
    test(!mirror(no_side,&s,&changed,&why),"unknown opponent side refused");
    auto extreme=fixture(); extreme[0].cells[3].value=0x80000000u; extreme[1].cells[3].value=0x7fffffffu;
    auto x=build(extreme); test(mirror(x,&s,&changed,&why),"extreme coordinates do not overflow mirroring");
    // In the no-host-orders case the guest adds the first table; keep the returned file whole.
    auto no_orders=build(fixture(false)); Sidecar s2; test(mirror(no_orders,&s2,&changed,&why),"mirror without a host order table");
    auto first=fixture(); first.back().cells={num("",200),text("guest move")};
    auto first_guest=build(first); Sidecar dummy; test(mirror(first_guest,&dummy,&changed,&why),"construct first-order guest");
    test(accept(no_orders,first_guest,s2,&result,&added,&why) && added==1,"guest may create the first order table");

    for (int i=1;i<argc;++i) {
        FILE *f=nullptr; std::vector<byte> real;
        if (!fopen_s(&f,argv[i],"rb") && f) {
            fseek(f,0,SEEK_END); long len=ftell(f); fseek(f,0,SEEK_SET);
            if (len>0 && len<=64*1024*1024) { real.resize((size_t)len); if (fread(real.data(),1,real.size(),f)!=real.size()) real.clear(); }
            fclose(f);
        }
        test(validate(real,&why),"real progpack uses bounded AZCF structures");
        if (real.empty() || !validate(real,&why)) printf("  %s: %s\n",argv[i],why.c_str());
    }
    printf("opsave checked selftest: %d checks, %d failed\n",checks,failures);
    return failures ? 1 : 0;
}

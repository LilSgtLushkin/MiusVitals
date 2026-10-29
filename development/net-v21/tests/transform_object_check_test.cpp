// Построчный прогон проверки объекта перед записью трансформа. Без игры, без движка.
// Зондирование настоящее: читается карта памяти процесса (Windows — VirtualQuery,
// Linux — /proc/self/maps), поэтому проверка работает с реальными страницами, а не с имитацией.
// Так видно каждый шаг и то, на каком именно шаге конкретный объект отбракован.
//
//   Windows: build.bat selftest [debug]  -> build\selftest\transform_object_check_test.exe
//   Linux:   g++ -std=c++11 -O2 -Wall -Wextra -Werror tests/transform_object_check_test.cpp -o /tmp/toc-test
//            g++ -std=c++11 -O0 -g -Wall -Wextra -Werror ... (тот же тест в отладочной конфигурации)
#include "../src/transform_object_check.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#include <unistd.h>
#endif

using namespace transform_check;

static int checks, failed;
static void check(bool ok, const char *label)
{
    ++checks;
    if (!ok) { ++failed; printf("  FAIL: %s\n", label); }
}

// ---------------------------------------------------------------- платформа: память и зондирование
static size_t page_size()
{
#ifdef _WIN32
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return si.dwPageSize < 4096 ? 4096 : si.dwPageSize;
#else
    long p = sysconf(_SC_PAGESIZE);
    return (size_t)(p < 4096 ? 4096 : p);
#endif
}

// Доступ к памяти: чтение и/или запись, либо ничего (освобождённая или закрытая страница).
enum Access { acc_none, acc_read, acc_read_write };

static bool mem_reserve(void **out, size_t bytes)
{
#ifdef _WIN32
    void *p = VirtualAlloc(0, bytes, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
#else
    void *p = mmap(0, bytes, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
#endif
    if (!p) return false;
    *out = p;
    return true;
}
static bool mem_access(void *p, size_t bytes, Access a)
{
#ifdef _WIN32
    DWORD protect = a == acc_read_write ? PAGE_READWRITE : a == acc_read ? PAGE_READONLY : PAGE_NOACCESS;
    DWORD old = 0;
    return VirtualProtect(p, bytes, protect, &old) != 0;
#else
    int prot = a == acc_read_write ? (PROT_READ | PROT_WRITE) : a == acc_read ? PROT_READ : PROT_NONE;
    return mprotect(p, bytes, prot) == 0;
#endif
}
static void mem_release(void *p, size_t bytes)
{
    if (!p) return;
#ifdef _WIN32
    // MEM_DECOMMIT, а не MEM_RELEASE: адрес остаётся зарезервированным, но память не выдана —
    // ровно то состояние, в котором оказывается удалённый объект.
    VirtualFree(p, bytes, MEM_DECOMMIT);
#else
    munmap(p, bytes);
#endif
}

static bool probe(const void *addr, Region *out, void *)
{
    if (!addr) return false;
#ifdef _WIN32
    MEMORY_BASIC_INFORMATION m;
    if (!VirtualQuery(addr, &m, sizeof m) || m.State != MEM_COMMIT) return false;
    out->begin = (uintptr_t)m.BaseAddress;
    out->end = (uintptr_t)m.BaseAddress + (uintptr_t)m.RegionSize;
    out->protection = 0;
    switch (m.Protect & 0xff) {
    case PAGE_READWRITE: case PAGE_WRITECOPY:
    case PAGE_EXECUTE_READWRITE: case PAGE_EXECUTE_WRITECOPY:
        out->protection = k_read | k_write; break;
    case PAGE_READONLY: case PAGE_EXECUTE_READ: case PAGE_EXECUTE_WRITECOPY:
        out->protection = k_read; break;
    default:
        out->protection = 0; break;      // PAGE_NOACCESS и всё остальное — недоступно
    }
    return true;
#else
    FILE *f = fopen("/proc/self/maps", "r");
    if (!f) return false;
    char line[512];
    unsigned long long b = 0, e = 0, want = (unsigned long long)(uintptr_t)addr;
    char perms[8] = "";
    bool found = false;
    while (fgets(line, sizeof line, f)) {
        unsigned long long lo = 0, hi = 0;
        char p[8] = "";
        if (sscanf(line, "%llx-%llx %7s", &lo, &hi, p) != 3) continue;
        if (want >= lo && want < hi) { b = lo; e = hi; memcpy(perms, p, sizeof perms); found = true; break; }
    }
    fclose(f);
    if (!found) return false;
    out->begin = (uintptr_t)b;
    out->end = (uintptr_t)e;
    out->protection = 0;
    if (perms[0] == 'r') out->protection |= k_read;
    if (perms[1] == 'w') out->protection |= k_write;
    return true;
#endif
}

// ---------------------------------------------------------------- фикстура объекта движка
struct Fixture {
    unsigned char *mem = 0;
    size_t size = 0;
    unsigned char *object = 0;

    bool map(size_t pages)
    {
        size = page_size() * pages;
        void *p = 0;
        if (!mem_reserve(&p, size)) return false;
        mem = (unsigned char *)p;
        return true;
    }
    void unmap() { mem_release(mem, size); mem = 0; }
    ~Fixture() { unmap(); }

    void fill(const void *manager, const char *iface, const char *name) { fill_at(object, manager, iface, name); }
    static void fill_at(unsigned char *at, const void *manager, const char *iface, const char *name)
    {
        memset(at, 0, 0x1f0);
        const char **slot = (const char **)(at + k_iface);
        *slot = iface;
        const void **om = (const void **)(at + k_objman);
        *om = manager;
        // Без strncpy: MSVC считает её небезопасной и предупреждает, а здесь нужен просто
        // точный перенос имени с гарантированным завершающим нулём.
        size_t n = strlen(name);
        if (n > k_full_size - 1) n = k_full_size - 1;
        memcpy(at + k_full, name, n);
        at[k_full + n] = 0;
    }
};

// ---------------------------------------------------------------- построчный прогон
static Result run(const char *title, const void *object, const void *manager,
                  const void *index_object, unsigned char want_kind, bool verbose)
{
    printf("\n-- %s --\n", title);
    struct Sink {
        static void print(void *, const Step *s)
        {
            printf("  %2d  %-34s %-6s  %s\n", s->index, s->name, s->passed ? "ok" : "REFUSED",
                   s->passed ? "" : refuse_text(s->refuse));
        }
        static void count(void *c, const Step *) { ++*(int *)c; }
    };
    int n_steps = 0;
    validate(object, manager, index_object, want_kind, &probe, 0, &Sink::count, &n_steps);
    Result r = validate(object, manager, index_object, want_kind, &probe, 0,
                        verbose ? &Sink::print : 0, 0);
    if (r.ok()) printf("  result: %s (all %d steps passed)\n", refuse_text(r.refuse), n_steps);
    else printf("  result: %s - failed step %d, offset 0x%lx\n", refuse_text(r.refuse),
                r.failed_step, (unsigned long)r.failed_at);
    return r;
}

int main()
{
    const size_t ps = page_size();
    const char *iface_techn = "i_techn";
    const char *iface_human = "i_human";
    const char *iface_other = "i_other";
    char manager_storage[64];
    const void *manager = manager_storage;      // «объектный менеджер»: любой адрес из стека
    const void *other_manager = (const void *)(manager_storage + 32);

    // 1. Исправный юнит: все шаги должны пройти.
    {
        Fixture f;
        check(f.map(2), "map ok");
        f.object = f.mem;
        f.fill(manager, iface_techn, "i_techn:tank_2a");
        Result r = run("valid i_techn unit", f.object, manager, f.object, k_kind_techn, true);
        check(r.ok(), "исправный объект проходит проверку");
        check(r.failed_step == -1, "нет отказавшего шага");
    }

    // 2. Пустой указатель.
    {
        Result r = run("null pointer", 0, manager, 0, 0, true);
        check(r.refuse == Refuse::null_object, "нулёвый указатель отбракован");
        check(r.failed_step == 0, "отказ на шаге 0");
    }

    // 3. Освобождённая память: состояние удалённого объекта.
    {
        Fixture f;
        check(f.map(2), "map ok");
        f.object = f.mem;
        f.fill(manager, iface_human, "i_human:soldier_1f");
        unsigned char *gone = f.object;
        f.unmap();                              // память реально возвращена системе
        Result r = run("object in freed memory", gone, manager, gone, 0, true);
        check(r.refuse == Refuse::object_unreadable, "освобождённый объект отбракован");
        check(r.failed_step == 1, "отказ на шаге 1 (читаемость объекта)");
    }

    // 4. Память только для чтения: читается, но писать нельзя.
    {
        Fixture f;
        check(f.map(2), "map ok");
        f.object = f.mem;
        f.fill(manager, iface_techn, "i_techn:truck_11");
        check(mem_access(f.mem, f.size, acc_read), "protect read-only");
        Result r = run("object on a read-only page", f.object, manager, f.object, 0, true);
        check(r.refuse == Refuse::target_unwritable, "недоступная для записи цель отбракована");
        check(r.failed_step == 8, "отказ на шаге 8 (доступность записи)");
        check(mem_access(f.mem, f.size, acc_read_write), "restore");
    }

    // 5. Чужой менеджер.
    {
        Fixture f;
        check(f.map(2), "map ok");
        f.object = f.mem;
        f.fill(other_manager, iface_human, "i_human:soldier_2");
        Result r = run("object from another manager", f.object, manager, f.object, 0, true);
        check(r.refuse == Refuse::wrong_manager, "чужой менеджер отбракован");
        check(r.failed_step == 2, "отказ на шаге 2");
    }

    // 6. Не юнит.
    {
        Fixture f;
        check(f.map(2), "map ok");
        f.object = f.mem;
        f.fill(manager, iface_other, "i_other:thing_1");
        Result r = run("object is not i_techn/i_human", f.object, manager, f.object, 0, true);
        check(r.refuse == Refuse::wrong_kind, "не юнит отбракован");
        check(r.failed_step == 4, "отказ на шаге 4");
    }

    // 7. Битый указатель на имя интерфейса.
    {
        Fixture f;
        check(f.map(2), "map ok");
        f.object = f.mem;
        f.fill(manager, iface_human, "i_human:soldier_3");
        const char **slot = (const char **)(f.object + k_iface);
        *slot = (const char *)(uintptr_t)0x1234;    // заведомо не отображённый адрес
        Result r = run("broken interface pointer", f.object, manager, f.object, 0, true);
        check(r.refuse == Refuse::iface_unreadable, "битый интерфейс отбракован");
        check(r.failed_step == 3, "отказ на шаге 3");
    }

    // 8. Имя без завершающего нуля: мусор вместо объекта.
    {
        Fixture f;
        check(f.map(2), "map ok");
        f.object = f.mem;
        f.fill(manager, iface_human, "i_human:soldier_4");
        memset(f.object + k_full, 'A', k_full_size);
        Result r = run("instance name without terminator", f.object, manager, f.object, 0, true);
        check(r.refuse == Refuse::name_unterminated, "незавершённое имя отбраковано");
        check(r.failed_step == 6, "отказ на шаге 6");
    }

    // 9. В живом индексе по этому ключу уже другой объект.
    {
        Fixture f, g;
        check(f.map(2) && g.map(2), "map ok");
        f.object = f.mem;
        g.object = g.mem;
        f.fill(manager, iface_techn, "i_techn:tank_7");
        g.fill(manager, iface_techn, "i_techn:tank_7");   // тот же ключ, другой адрес
        Result r = run("live index holds another object", f.object, manager, g.object, 0, true);
        check(r.refuse == Refuse::index_mismatch, "подмена объекта отбракована");
        check(r.failed_step == 7, "отказ на шаге 7");
    }

    // 10. Граница региона: объект упирается в конец доступной памяти и ровно у края.
    {
        Fixture f;
        check(f.map(2), "map ok");
        // Сначала заполняем обе позиции, и только потом закрываем вторую страницу: иначе
        // подготовка фикстуры сама упадёт, записывая за границу.
        f.object = f.mem + ps - (size_t)(k_checked - 8);
        f.fill(manager, iface_human, "i_human:soldier_5");
        unsigned char *inside = f.mem + ps - (size_t)k_checked;
        Fixture::fill_at(inside, manager, iface_human, "i_human:soldier_6");
        check(mem_access(f.mem + ps, ps, acc_none), "вторая страница недоступна");
        Result r = run("object runs past the end of its region", f.object, manager, f.object, 0, true);
        check(r.refuse == Refuse::object_unreadable, "выход за границу региона отбракован");
        check(r.failed_step == 1, "отказ на шаге 1");

        // И наоборот: если объекта хватает целиком — проверка проходит.
        f.object = inside;
        Result r2 = run("object fits inside its region", f.object, manager, f.object, 0, false);
        check(r2.ok(), "объект у самой границы, но целиком внутри — проходит");
        check(mem_access(f.mem + ps, ps, acc_read_write), "restore");
    }

    // 11. Запрошенный вид юнита.
    {
        Fixture f;
        check(f.map(2), "map ok");
        f.object = f.mem;
        f.fill(manager, iface_human, "i_human:soldier_8");
        Result r = run("wanted i_techn, got i_human", f.object, manager, f.object, k_kind_techn, false);
        check(r.refuse == Refuse::wrong_kind, "несоответствие вида отбраковано");
        Result r2 = run("wanted i_human", f.object, manager, f.object, k_kind_human, false);
        check(r2.ok(), "совпавший вид проходит");
    }

    printf("\n%d checks, %d failed\n", checks, failed);
    return failed ? 1 : 0;
}

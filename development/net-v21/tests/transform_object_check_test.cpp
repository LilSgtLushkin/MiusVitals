// Linux, без игры: построчный прогон проверки объекта перед записью трансформа.
// Зондирование здесь настоящее — читается /proc/self/maps, то есть реальная карта памяти процесса.
// Так видно каждый шаг и то, на каком именно шаге конкретный объект отбраковывается.
//
//   g++ -std=c++11 -O2 -Wall -Wextra -Werror tests/transform_object_check_test.cpp -o /tmp/toc-test
#include "../src/transform_object_check.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

using namespace transform_check;

static int checks, failed;
static void check(bool ok, const char *label)
{
    ++checks;
    if (!ok) { ++failed; printf("  FAIL: %s\n", label); }
}

// Настоящее зондирование: ищем диапазон в карте памяти процесса.
struct Probe {
    static bool query(const void *addr, Region *out, void *)
    {
        if (!addr) return false;
        FILE *f = fopen("/proc/self/maps", "r");
        if (!f) return false;
        char line[512];
        unsigned long long b = 0, e = 0;
        char perms[8] = "";
        bool found = false;
        unsigned long long want = (unsigned long long)(uintptr_t)addr;
        while (fgets(line, sizeof line, f)) {
            unsigned long long lo = 0, hi = 0;
            char p[8] = "";
            if (sscanf(line, "%llx-%llx %7s", &lo, &hi, p) != 3) continue;
            if (want >= lo && want < hi) {
                b = lo; e = hi; memcpy(perms, p, sizeof perms); found = true;
                break;
            }
        }
        fclose(f);
        if (!found) return false;
        out->begin = (uintptr_t)b;
        out->end = (uintptr_t)e;
        out->protection = 0;
        if (perms[0] == 'r') out->protection |= k_read;
        if (perms[1] == 'w') out->protection |= k_write;
        return true;
    }
};

static long page()
{
    long p = sysconf(_SC_PAGESIZE);
    return p < 4096 ? 4096 : p;
}

// Объект движка: заголовок 0xf0, поля движения до +0x1ec, имя в +0x08.
struct Fixture {
    unsigned char *mem = 0;
    size_t size = 0;
    unsigned char *object = 0;

    bool map(size_t pages, int prot)
    {
        size = (size_t)page() * pages;
        mem = (unsigned char *)mmap(0, size, prot, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        return mem != MAP_FAILED;
    }
    void unmap()
    {
        if (mem && mem != MAP_FAILED) munmap(mem, size);
        mem = 0;
    }
    ~Fixture() { unmap(); }

    // Заполнить объект как настоящий юнит: iface, objman, имя с номером.
    void fill(const void *manager, const char *iface, const char *name)
    {
        fill_at(object, manager, iface, name);
    }
    static void fill_at(unsigned char *at, const void *manager, const char *iface, const char *name)
    {
        memset(at, 0, 0x1f0);
        const char **slot = (const char **)(at + k_iface);
        *slot = iface;
        const void **om = (const void **)(at + k_objman);
        *om = manager;
        strncpy((char *)at + k_full, name, k_full_size - 1);
    }
};

// Построчный вывод одного прогона.
static Result run(const char *title, const void *object, const void *manager,
                  const void *index_object, unsigned char want_kind, bool verbose)
{
    printf("\n── %s ──\n", title);
    struct Sink {
        static void print(void *, const Step *s)
        {
            printf("  %2d  %-34s %-6s  %s\n", s->index, s->name, s->passed ? "ok" : "ОТКАЗ",
                   s->passed ? "" : refuse_text(s->refuse));
        }
    };
    int n_steps = 0;
    struct Counter { static void count(void *c, const Step *) { ++*(int *)c; } };
    validate(object, manager, index_object, want_kind, &Probe::query, 0, &Counter::count, &n_steps);
    Result r = validate(object, manager, index_object, want_kind, &Probe::query, 0,
                        verbose ? &Sink::print : 0, 0);
    if (r.ok()) printf("  итог: %s (все %d шагов пройдены)\n", refuse_text(r.refuse), n_steps);
    else printf("  итог: %s — отказавший шаг %d, смещение 0x%lx\n", refuse_text(r.refuse),
                r.failed_step, (unsigned long)r.failed_at);
    return r;
}

int main()
{
    const long ps = page();
    const char *iface_techn = "i_techn";
    const char *iface_human = "i_human";
    const char *iface_other = "i_other";
    char manager_storage[64];
    const void *manager = manager_storage;      // «объектный менеджер» любого адреса из стека
    const void *other_manager = (const void *)(manager_storage + 32);

    // ── 1. Исправный юнит: все шаги должны пройти ────────────────────────────────────────────
    {
        Fixture f;
        check(f.map(2, PROT_READ | PROT_WRITE), "map ok");
        f.object = f.mem;
        f.fill(manager, iface_techn, "i_techn:tank_2a");
        Result r = run("исправный юнит i_techn", f.object, manager, f.object, k_kind_techn, true);
        check(r.ok(), "исправный объект проходит проверку");
        check(r.failed_step == -1, "нет отказавшего шага");
    }

    // ── 2. Пустой указатель ──────────────────────────────────────────────────────────────────
    {
        Result r = run("нулевой указатель", 0, manager, 0, 0, true);
        check(r.refuse == Refuse::null_object, "нулёвый указатель отбракован");
        check(r.failed_step == 0, "отказ на шаге 0");
    }

    // ── 3. Освобождённая память (то, что происходит с удалённым объектом) ────────────────────
    {
        Fixture f;
        check(f.map(2, PROT_READ | PROT_WRITE), "map ok");
        f.object = f.mem;
        f.fill(manager, iface_human, "i_human:soldier_1f");
        unsigned char *gone = f.object;
        f.unmap();                              // память реально возвращена системе
        Result r = run("объект в освобождённой памяти", gone, manager, gone, 0, true);
        check(r.refuse == Refuse::object_unreadable, "освобождённый объект отбракован");
        check(r.failed_step == 1, "отказ на шаге 1 (читаемость объекта)");
    }

    // ── 4. Память только для чтения: читается, но писать нельзя ──────────────────────────────
    {
        Fixture f;
        check(f.map(2, PROT_READ | PROT_WRITE), "map ok");
        f.object = f.mem;
        f.fill(manager, iface_techn, "i_techn:truck_11");
        check(mprotect(f.mem, f.size, PROT_READ) == 0, "mprotect read-only");
        Result r = run("объект на странице только для чтения", f.object, manager, f.object, 0, true);
        check(r.refuse == Refuse::target_unwritable, "недоступная для записи цель отбракована");
        check(r.failed_step == 8, "отказ на шаге 8 (доступность записи)");
        check(mprotect(f.mem, f.size, PROT_READ | PROT_WRITE) == 0, "mprotect back");
    }

    // ── 5. Чужой менеджер ────────────────────────────────────────────────────────────────────
    {
        Fixture f;
        check(f.map(2, PROT_READ | PROT_WRITE), "map ok");
        f.object = f.mem;
        f.fill(other_manager, iface_human, "i_human:soldier_2");
        Result r = run("объект из другого менеджера", f.object, manager, f.object, 0, true);
        check(r.refuse == Refuse::wrong_manager, "чужой менеджер отбракован");
        check(r.failed_step == 2, "отказ на шаге 2");
    }

    // ── 6. Не юнит ───────────────────────────────────────────────────────────────────────────
    {
        Fixture f;
        check(f.map(2, PROT_READ | PROT_WRITE), "map ok");
        f.object = f.mem;
        f.fill(manager, iface_other, "i_other:thing_1");
        Result r = run("объект не i_techn/i_human", f.object, manager, f.object, 0, true);
        check(r.refuse == Refuse::wrong_kind, "не юнит отбракован");
        check(r.failed_step == 4, "отказ на шаге 4");
    }

    // ── 7. Битый указатель на имя интерфейса ─────────────────────────────────────────────────
    {
        Fixture f;
        check(f.map(2, PROT_READ | PROT_WRITE), "map ok");
        f.object = f.mem;
        f.fill(manager, iface_human, "i_human:soldier_3");
        const char **slot = (const char **)(f.object + k_iface);
        *slot = (const char *)0x1234;           // заведомо не отображённый адрес
        Result r = run("битый указатель на интерфейс", f.object, manager, f.object, 0, true);
        check(r.refuse == Refuse::iface_unreadable, "битый интерфейс отбракован");
        check(r.failed_step == 3, "отказ на шаге 3");
    }

    // ── 8. Имя без завершающего нуля (мусор вместо объекта) ──────────────────────────────────
    {
        Fixture f;
        check(f.map(2, PROT_READ | PROT_WRITE), "map ok");
        f.object = f.mem;
        f.fill(manager, iface_human, "i_human:soldier_4");
        memset(f.object + k_full, 'A', k_full_size);
        Result r = run("имя экземпляра без нуля", f.object, manager, f.object, 0, true);
        check(r.refuse == Refuse::name_unterminated, "незавершённое имя отбраковано");
        check(r.failed_step == 6, "отказ на шаге 6");
    }

    // ── 9. В живом индексе по этому ключу уже другой объект ──────────────────────────────────
    {
        Fixture f, g;
        check(f.map(2, PROT_READ | PROT_WRITE) && g.map(2, PROT_READ | PROT_WRITE), "map ok");
        f.object = f.mem;
        g.object = g.mem;
        f.fill(manager, iface_techn, "i_techn:tank_7");
        g.fill(manager, iface_techn, "i_techn:tank_7");   // тот же ключ, другой адрес
        Result r = run("живой индекс указывает на другой объект", f.object, manager, g.object, 0, true);
        check(r.refuse == Refuse::index_mismatch, "подмена объекта отбракована");
        check(r.failed_step == 7, "отказ на шаге 7");
    }

    // ── 10. Граница региона: ровно у конца и на один байт за концом ─────────────────────────
    {
        // Две страницы: вторая недоступна. Объект кладём так, что проверяемый хвост (0x1f0)
        // задевает вторую страницу — ровно тот случай, когда часть полей писать нельзя.
        Fixture f;
        check(f.map(2, PROT_READ | PROT_WRITE), "map ok");
        // Сначала заполняем обе позиции, и только потом закрываем вторую страницу: иначе
        // подготовка фикстуры сама упадёт, записывая за границу.
        f.object = f.mem + ps - (size_t)(k_checked - 8);
        f.fill(manager, iface_human, "i_human:soldier_5");
        unsigned char *inside = f.mem + ps - (size_t)k_checked;
        f.fill_at(inside, manager, iface_human, "i_human:soldier_6");
        check(mprotect(f.mem + ps, ps, PROT_NONE) == 0, "вторая страница недоступна");
        Result r = run("объект упирается в конец доступной памяти", f.object, manager, f.object, 0, true);
        check(r.refuse == Refuse::object_unreadable, "выход за границу региона отбракован");
        check(r.failed_step == 1, "отказ на шаге 1");

        // И наоборот: если объекта хватает целиком — проверка проходит.
        f.object = inside;
        Result r2 = run("объект целиком внутри региона", f.object, manager, f.object, 0, false);
        check(r2.ok(), "объект у самой границы, но целиком внутри — проходит");
        check(mprotect(f.mem + ps, ps, PROT_READ | PROT_WRITE) == 0, "restore");
    }

    // ── 11. Запрошенный вид юнита ────────────────────────────────────────────────────────────
    {
        Fixture f;
        check(f.map(2, PROT_READ | PROT_WRITE), "map ok");
        f.object = f.mem;
        f.fill(manager, iface_human, "i_human:soldier_8");
        Result r = run("ожидался i_techn, а это i_human", f.object, manager, f.object, k_kind_techn, false);
        check(r.refuse == Refuse::wrong_kind, "несоответствие вида отбраковано");
        Result r2 = run("ожидался i_human", f.object, manager, f.object, k_kind_human, false);
        check(r2.ok(), "совпавший вид проходит");
    }

    printf("\n%d проверок, провалов %d\n", checks, failed);
    return failed ? 1 : 0;
}

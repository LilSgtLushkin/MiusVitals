// Проверка объекта перед записью трансформа. Портируемая: никаких windows.h, только смещения и
// инъекция запроса о памяти. В бою зондирование делает VirtualQuery (readable/writable из
// anim_sync.cpp и hit_sync.cpp), в тесте — чтение /proc/self/maps, то есть настоящая карта памяти.
//
// Зачем: путь трансформов (session.cpp) писал 33 float по сырому указателю из снимка списка
// объектов без единой проверки, тогда как anim_sync и hit_sync проверяют объект перед каждой
// записью. Объект может быть удалён между снимком и записью (shots/anim/trench в том же шаге),
// и тогда запись портит чужую память. Это и есть причина падений, поэтому проверка вынесена
// сюда, чтобы её можно было прогнать по шагам тестами без игры.
#pragma once
#include <stddef.h>
#include <stdint.h>
#include <string.h>

namespace transform_check {

// Движок: заголовок Object занимает 0xf0 байт (см. engine.h), поля движения — до +0x1ec.
enum : size_t {
    k_iface = 0x48, k_objman = 0x60, k_full = 0x08, k_full_size = 64, k_header = 0xf0,
    k_rot = 0xf0, k_inv = 0x130, k_pos = 0x1b0, k_lin = 0x1d0, k_ang = 0x1e0,
    k_rot_size = 36, k_inv_size = 36, k_vec_size = 12,
    // Поля движения лежат внутри объекта; anim_sync проверяет ровно столько же (0x1efc).
    k_checked = 0x1f0
};
enum : unsigned char { k_kind_techn = 1, k_kind_human = 2 };

enum class Refuse {
    none = 0,
    null_object,          // указатель пустой
    object_unreadable,    // объект не читается целиком (уже освобождён)
    wrong_manager,        // objman не тот: объект заменён или это другой мир
    iface_unreadable,     // указатель на имя интерфейса не читается
    wrong_kind,           // это не i_techn / i_human
    name_unreadable,      // имя экземпляра не читается
    name_unterminated,    // в имени нет завершающего нуля: это мусор, а не объект
    index_mismatch,       // в живом индексе по этому ключу лежит другой объект
    target_unwritable     // одно из полей движения недоступно для записи
};

inline const char *refuse_text(Refuse r)
{
    switch (r) {
    case Refuse::none:              return "ok";
    case Refuse::null_object:       return "null object pointer";
    case Refuse::object_unreadable: return "object memory is not readable";
    case Refuse::wrong_manager:     return "object belongs to another manager";
    case Refuse::iface_unreadable:  return "interface name is not readable";
    case Refuse::wrong_kind:        return "object is not i_techn/i_human";
    case Refuse::name_unreadable:   return "instance name is not readable";
    case Refuse::name_unterminated: return "instance name is not terminated";
    case Refuse::index_mismatch:    return "live index holds a different object";
    case Refuse::target_unwritable: return "motion field is not writable";
    }
    return "unknown";
}

// Диапазон выделенной памяти, в который попадает адрес.
struct Region {
    uintptr_t begin = 0, end = 0;
    unsigned protection = 0;      // биты k_read / k_write
};
enum : unsigned { k_read = 1u, k_write = 2u };

// false — адрес не принадлежит выделенной памяти (освобождён или вообще не из этой игры).
typedef bool (*ProbeFn)(const void *addr, Region *out, void *ctx);

struct Step {
    int index = 0;
    const char *name = "";
    bool passed = false;
    Refuse refuse = Refuse::none;
    size_t at = 0;                // смещение или адрес, к которому относится шаг
};

// Построчный разбор: вызывается для каждого шага, в том числе для последнего и проваленного.
typedef void (*TraceFn)(void *ctx, const Step *step);

struct Result {
    Refuse refuse = Refuse::none;
    int failed_step = -1;         // -1: всё прошло
    size_t failed_at = 0;
    bool ok() const { return refuse == Refuse::none; }
};

template <class T> inline T load_at(const void *base, size_t off)
{
    T v;
    memcpy(&v, (const char *)base + off, sizeof v);
    return v;
}

// object        проверяемый объект
// manager       объектный менеджер этого шага (objman)
// index_object  что лежит в живом индексе по ключу этого юнита; 0 — не сверять
// want_kind     1 = i_techn, 2 = i_human, 0 = любой из двух
// probe/ctx     запрос о памяти (VirtualQuery в бою)
// trace/tctx    построчный вывод; может быть 0
inline Result validate(const void *object, const void *manager, const void *index_object,
                       unsigned char want_kind, ProbeFn probe, void *probe_ctx,
                       TraceFn trace = 0, void *tctx = 0)
{
    Result result;
    Region region;
    Step step;
    int n = 0;

    // Каждый шаг: имя, адрес/смещение, условие, отказ. Порядок — от дешёвого к дорогому и от
    // «указатель вообще существует» к «конкретное поле доступно для записи».
#define STEP(name_, at_, refuse_, condition_)                            \
    do {                                                                 \
        step.index = n++;                                                \
        step.name = (name_);                                             \
        step.at = (size_t)(at_);                                         \
        step.refuse = (refuse_);                                         \
        step.passed = (condition_);                                      \
        if (trace) trace(tctx, &step);                                   \
        if (!step.passed) {                                              \
            result.refuse = (refuse_);                                   \
            result.failed_step = step.index;                             \
            result.failed_at = step.at;                                  \
            return result;                                               \
        }                                                                \
    } while (0)

    STEP("object pointer is not null", 0, Refuse::null_object, object != 0);
    if (!probe) return result;

    // 1. Весь проверяемый хвост объекта читается: заголовок 0xf0 и поля движения до +0x1ec.
    STEP("object memory is readable", k_checked, Refuse::object_unreadable,
         probe(object, &region, probe_ctx) && (region.protection & k_read) != 0 &&
         region.end - (uintptr_t)object >= k_checked);

    // 2. Тот же менеджер: объект из другого боя или уже переиспользованная память.
    const void *owner = load_at<const void *>(object, k_objman);
    STEP("object manager matches", k_objman, Refuse::wrong_manager, owner == manager);

    // 3-4. Интерфейс: читаемость указателя и само имя.
    const char *iface = load_at<const char *>(object, k_iface);
    STEP("interface name is readable", k_iface, Refuse::iface_unreadable,
         probe(iface, &region, probe_ctx) && (region.protection & k_read) != 0 &&
         region.end - (uintptr_t)iface >= 8);
    bool techn = !memcmp(iface, "i_techn", 8), human = !memcmp(iface, "i_human", 8);
    STEP("object is a unit", k_iface, Refuse::wrong_kind,
         (techn || human) && (!want_kind || (techn ? 1 : 2) == (int)want_kind));

    // 5-6. Имя экземпляра: по нему берётся номер юнита, поэтому мусор здесь недопустим.
    const char *full = (const char *)object + k_full;
    STEP("instance name is readable", k_full, Refuse::name_unreadable,
         probe(full, &region, probe_ctx) && (region.protection & k_read) != 0 &&
         region.end - (uintptr_t)full >= k_full_size);
    STEP("instance name is terminated", k_full, Refuse::name_unterminated,
         memchr(full, 0, k_full_size) != 0);

    // 7. Сверка с живым индексом: ловит «объект удалён, по тому же адресу уже другой».
    STEP("live index holds this object", 0, Refuse::index_mismatch,
         index_object == 0 || index_object == object);

    // 8. Поля, которые мы будем писать, доступны для записи.
    const struct { size_t off, size; const char *name; } targets[] = {
        { k_pos, k_vec_size, "position" },
        { k_rot, k_rot_size, "rotation" },
        { k_inv, k_inv_size, "inverse rotation" },
        { k_lin, k_vec_size, "linear velocity" },
        { k_ang, k_vec_size, "angular velocity" },
    };
    for (int i = 0; i < 5; i++) {
        const char *at = (const char *)object + targets[i].off;
        STEP(targets[i].name, targets[i].off, Refuse::target_unwritable,
             probe(at, &region, probe_ctx) && (region.protection & k_write) != 0 &&
             region.end - (uintptr_t)at >= targets[i].size);
    }
#undef STEP
    return result;
}

}  // namespace transform_check

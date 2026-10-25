# MiusVitals v7.1.0

Надстройка для [MiusPlayer](https://github.com/VampyrCat/MiusPlayer) — мультиплеера **Graviteam Tactics: Mius-Front**. Версия рассчитана на Steam-игру 6.00 build 3598 и MiusPlayer v20.

**Готовая загрузка:** [`MiusVitals_kit_7.1.0.zip`](MiusVitals_kit_7.1.0.zip) — это ровно комплект из 16 файлов под единым каталогом `MiusVitals/`. Его дерево проверяется allow-list'ом, а ZIP собирается воспроизводимо.

> Статус проверки: артефакты, структура ZIP и логика транспорта проверены локально; нативный тест выполняется под ASan/UBSan. Два экземпляра самой игры в этой Linux-среде не запускались, поэтому совместный бой хоста и гостя остаётся необходимой финальной проверкой.

## Что изменено в 7.1.0

### Подтверждаемый канал Vitals

Предыдущая Vitals-посылка была fire-and-forget. В 7.1.0 она заменена отдельным прикладным транспортом:

- данные Vitals — пакет **типа 52 (`0x34`)**; подтверждение — **тип 53 (`0x35`)**;
- пакет содержит ненулевые `epoch` и `sequence`; получатель кладёт его в mailbox, затем отправляет ACK;
- отправитель хранит до 32 не подтверждённых пакетов, повторяет идентичный пакет через 400 мс и прекращает после 12 повторов;
- история из 128 `(epoch, sequence)` подавляет повторное применение, но на дубликат ACK отправляется снова;
- заполненный mailbox **не** подтверждается, поэтому отправитель повторит пакет вместо молчаливой потери;
- журнал раз в пять секунд показывает `sent`, `acked`, `retried`, `replaced`, `pending`, `received`, `duplicates`, `inbox full` и `unmatched ACK`.

Типы 52/53 намеренно не пересекаются с прежним Vitals type 50 (`0x32`), поэтому старая DLL не сможет неверно прочитать расширенный заголовок. У обоих игроков должен быть одинаковый **v7.1.0**.

### Что намеренно не менялось

Нативный надёжный поток MiusPlayer: тип **10** (упорядоченные игровые записи) и тип **11** (его ACK) не изменяется. DLL только учитывает типы в диагностической статистике и передаёт их в исходный обработчик без изменения байтов или его курсоров. Анализ приложенного боевого лога показывает много штатных type-11 ACK и не доказывает неисправность этого потока; исправлять его без такого доказательства небезопасно.

Сохранены предыдущие точечные меры: периодическая отправка неподвижных юнитов (`MP_IDLE_MS`) и строго ограниченный guard подтверждённого read-AV `basic_render` (`MP_RENDER_GUARD=0` отключает только его).

## Установка

1. Установите оригинальный **MiusPlayer v20** и закройте игру.
2. Распакуйте `MiusVitals_kit_7.1.0.zip` в каталог игры: получиться должен `...\Graviteam Tactics Mius-Front\MiusVitals\`, рядом с `starter_x64.exe` и `mpmod1`.
3. Запустите `MiusVitals\Check_Build.bat`. Он создаст `build_report.txt`; поддерживаемые файлы должны быть `same`.
4. Запустите `MiusVitals\Install_Vitals.bat` от обычного пользователя.
5. Повторите у второго игрока, используя тот же v7.1.0 kit.

Откат: закройте игру и запустите `MiusVitals\Uninstall_Vitals.bat`. Установщик сохраняет оригинальный `basic_network.codelib` в `MiusVitals\state`.

Полная инструкция, контрольные суммы и признаки работы — в [`MiusVitals/README_VITALS.txt`](MiusVitals/README_VITALS.txt).

## Глобальная карта операции

Исследование текущей проблемы вынесено в [`docs/OPERATION_MAP_ANALYSIS_2026-10-25.md`](docs/OPERATION_MAP_ANALYSIS_2026-10-25.md). Главный вывод пока не является основанием для рискованного патча: соединение в лобби ещё не означает, что второй `opgame` вошёл в map-mode и подтвердил тот же epoch. Без этого хост по проекту MiusPlayer не запрашивает у клиента подтверждение хода. Приложенный `opgame`-лог завершился до сетевой map-handshake и не позволяет доказать более точную причину.

## Сборка и проверка

```sh
# Нативный тест (GCC/Clang, ASan/UBSan)
scripts/test_native.sh

# PE/хэши + exact ZIP + нативный тест
scripts/verify_release.sh

# Пересборка DLL и ровно этого ZIP (Python с ziglang 0.17.0)
ZIG_PYTHON=/path/to/python-with-zig MiusVitals/src/build.sh
python3 scripts/package_kit.py --output MiusVitals_kit_7.1.0.zip
```

Для полного воспроизводимого rebase трёх `codelib`-вариантов нужен оригинальный MiusPlayer v20 `basic_network.codelib`:

```sh
ZIG_PYTHON=/path/to/python-with-zig MiusVitals/src/build_release.sh \
  /path/to/mpmod1/files/bin/rel/x64/basic_network.codelib
```

Контрольные суммы и процесс — в [`docs/BUILD_AND_VALIDATION.md`](docs/BUILD_AND_VALIDATION.md). Анализ upstream — в [`docs/MIUSPLAYER_ANALYSIS.md`](docs/MIUSPLAYER_ANALYSIS.md).

## Совместимость и ограничения

- Поддерживается лишь MiusPlayer v20 с исходным `basic_network.codelib` SHA-256 `8108bd1e…4ae5723` и Steam build 3598/1 для rebase.
- DLL перед установкой каждого перехвата проверяет timestamp и машинные сигнатуры. При несовпадении она не ставит соответствующий перехват.
- Мод не меняет сохранения игры и не является доказанной проверкой на реальной паре игроков.

Upstream MiusPlayer: [`VampyrCat/MiusPlayer`](https://github.com/VampyrCat/MiusPlayer), исследованный commit `43b2736d3b4a1e208e6c80f45f6ebbdd476de3aa` от 2026-10-14, лицензия CC0-1.0; см. [`NOTICE.md`](NOTICE.md).

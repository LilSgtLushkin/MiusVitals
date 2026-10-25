# Сборка и проверка MiusVitals v7.2.0

## Воспроизводимые входы и артефакты

`development/kit-source/src/build_release.sh` принимает только оригинальный MiusPlayer v20 `basic_network.codelib`; SHA-256 входа проверяется до всех изменений.

| Вариант | Преобразование | SHA-256 |
| --- | --- | --- |
| Исходный MiusPlayer v20 | входной файл | `8108bd1e9af8080ef8b1050512b8311848f5a336acb49a7479388733b4ae5723` |
| Main | rebase 3598/1, verified operation Next Turn, импорт DLL | `4c91874c4b62db424ecca36586121bcb3b4cc7849cc900285e68de5151eddc36` |
| `nostart` | тот же rebase без Start group, импорт DLL | `f0766bec48696869d59d4a654a04f20bd7baf9f73adb6fbf139e7da31dfc1f4f` |
| `norebase` | только импорт DLL; диагностический вариант | `d2e34581b42d0573547e221873d7538b64a413267e8ed388abeb46dd42b630d2` |
| `mp_vitals.dll` | Zig 0.17.0, no CRT, KERNEL32 only | `8f336d5645c01a394b7be776fda704e0910adb949083c16fea6ba25df7294f3e` |

Main и `nostart` изменяют три связанных элемента operation-map detour только после строгой сверки исходного v20:

1. immediate по RVA `basic_network+0x1a327`: `adv_opgame+0x36b40` → `+0x36b30`;
2. проверяемую 16-байтную сигнатуру по `basic_network+0x48da0`;
3. уже ранее rebased worker (`+0x76510`) и save routine (`+0xd6080`).

`verify_artifacts.py` проверяет не только хэш, импорт `mp_vitals.dll!MpVitalsInit` и PE surface, но и новый operation Next Turn target/signature в main/nostart. Таким образом artifact не может выглядеть валидным, если map detour снова окажется отключённым старой проверкой.

## Exact downloadable kit

`development/kit-source/` — редактируемая development структура, не каталог установки. `scripts/package_kit.py` — allow-list единственного installable download: `MiusVitals_v7.2.0_INSTALL.zip`. В архив попадают ровно эти 16 файлов под единственным `MiusVitals/` root:

```text
Check_Build.ps1
Uninstall_Vitals.bat
known_3598.sha256
Check_Build.bat
Install_Vitals.bat
README_VITALS.txt
src/mp_vitals.c
src/patch_import.py
src/build.sh
src/rebase_3598_1.py
src/test/t.c
src/test/stub/windows.h
files/mp_vitals.dll
files/basic_network.codelib
files/alt/basic_network_nostart.codelib
files/alt/basic_network_v5_norebase.codelib
```

ZIP использует фиксированный timestamp 2026-10-25 00:00:00, явный порядок записей, фиксированные mode bits и deflate level 9. `--verify` проверяет CRC, дерево, timestamp и точную byte identity со входами kit.

```sh
python3 scripts/package_kit.py --output MiusVitals_v7.2.0_INSTALL.zip
python3 scripts/package_kit.py --verify MiusVitals_v7.2.0_INSTALL.zip
sha256sum MiusVitals_v7.2.0_INSTALL.zip
```

## Локальная проверка

```sh
scripts/test_native.sh
scripts/verify_release.sh
```

Native harness запускается с ASan/UBSan. Помимо прежних тестов смертей/ранений/техники, ACK/retry, duplicate mailbox и transparent type 10/11, он утверждает v7.2 правила:

- Vitals state не уходит, не помещается в mailbox и не ACK-ается, пока object-number exchange не готов;
- запрос exchange начинается через три секунды и повторяется;
- после `yes` отправляется snapshot и повторённый пакет peer принимается;
- перед statistic hook выполняется final forced scan.

## Полная пересборка

```sh
python3 -m venv /tmp/mius-v7-venv
/tmp/mius-v7-venv/bin/pip install -r development/kit-source/src/requirements-build.txt
ZIG_PYTHON=/tmp/mius-v7-venv/bin/python development/kit-source/src/build_release.sh \
  /path/to/mpmod1/files/bin/rel/x64/basic_network.codelib
scripts/verify_release.sh
```

Скрипт регенерирует все варианты codelib, собирает DLL, проверяет hashes/PE/map detour и создаёт exact install ZIP. Он не устанавливает ничего в игровую папку.

Нативные и static проверки не заменяют живую пару Graviteam. Первый тест после релиза должен включать: обмен карты, приказы обоих игроков, оба Next Turn, передачу следующего map epoch, существующее и появляющееся во время turn подкрепление, а затем тактический бой с `object-number lists exchanged: yes` на обеих машинах. Текущий merge гарантированно добавляет новые `user_orders`, но не доказан для произвольной новой structural row подкрепления. Финальный forced scan проверен нативно, но это ещё не отдельный блокирующий ACK-barrier при выходе из tactical game: его нельзя считать доказанным без живой проверки перехода карты.

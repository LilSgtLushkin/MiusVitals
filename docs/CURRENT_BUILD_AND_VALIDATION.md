# Текущая сборка и правила проверки net_v21

## Устройство дерева

* `development/versions/1.0.8/src/` — рабочий C++ модуля и launcher. Версия меняется: путь к рабочему исходнику —
  `development/versions/<версия>/`, текущая версия записана в `development/versions/current.txt`. Исторический
  `SOURCE_MANIFEST.sha256` фиксирует исходный импорт и не является хеш-листом изменённой версии.
* `development/versions/1.0.8/tests/` — тесты, peer-стенды и вспомогательные фикстуры. По ранее принятому
  решению `named_shots.h` и его интеграционный тест пока остаются на прежнем месте в `src/`.
* `development/versions/1.0.8/fixtures/protocol17/` — три закреплённых исходника 1.0.7 как неподвижная
  ссылка на прежний поток событий; происхождение, SHA-256 и ограничения описаны в README рядом
  с ними. Текущий модуль использует wire tag 18: совпадения заголовков нет и не требуется,
  mixed-version запуск удалён, проверяются только сами закреплённые байты.
* `development/upstream/` — неизменяемый публичный baseline; он не является рабочим `net_v21`.
  `development/versions/1.0.8/tools/` — инструменты, `build/` — игнорируемые результаты сборки.
* [`NETWORK_SESSION_2026-10-29.md`](NETWORK_SESSION_2026-10-29.md) отделяет подтверждённые
  наблюдения от предположений и описывает незавершённое расследование последнего вылета.

## Быстрые локальные проверки

Например, для изменённой логики кэша и проверки pinned fixture на Linux:

```sh
c++ -std=c++17 -O2 -Wall -Wextra -Werror -Idevelopment/versions/1.0.8/src \
  development/versions/1.0.8/tests/transform_gc_test.cpp -o /tmp/mius-transform-gc-test
/tmp/mius-transform-gc-test
c++ -std=c++17 -O2 -Wall -Wextra -Werror -Idevelopment/versions/1.0.8/src \
  -Idevelopment/versions/1.0.8/tests \
  development/versions/1.0.8/tests/image_mirror_identity_test.cpp -o /tmp/mius-image-test
/tmp/mius-image-test
python3 development/versions/1.0.8/tools/extract_legacy_shots_fixture_selftest.py
python3 development/versions/1.0.8/tools/pause_session_slice_selftest.py
# Структура shipped-установщика и совпадение с записанным рядом SHA-256:
python3 scripts/check_shipped_installer.py
```

`image_mirror_identity_test` покрывает зеркалирование сторон и перепись командных тегов
(`image::squad_tags()`), `pause_session_slice_selftest.py` извлекает настоящие функции паузы
и шапки состояния из `session.cpp` и проверяет их на подставном движке.

Windows-зависимые части можно отдельно проверить Zig через `development/versions/1.0.8/tools/syntax/check.sh`;
это не замена MSVC и не запуск игры. В Windows с VS2022 из `development/net-v21` используются
`build.bat module`, `build.bat selftest` и `build.bat join`. `selftest` строит отдельные `.exe`;
их выполнение и межпроцессные сценарии заданы в финальном workflow.

## Финальная проверка и выпуск

Полный gate запускается **один раз на готовом non-draft PR**: Windows/MSVC, отдельные и двухпроцессные
стенды, сборка модуля и launcher, установщик с rollback/upgrade, Linux/native и проверки архивов.
Изменение HEAD после запуска делает результат устаревшим и требует повторной проверки. Фокусные
проверки не заменяют gate; CI не доказывает работу игры и Steam. Исторические файлы `_known_good` и разрешённые архивы не перезаписываются. В дереве
остался один установщик 1.0.7 — `MiusMultiplayer_v1.0.7_EXPERIMENTAL_NET+FPS_INSTALL.zip`; прежние
сборки 1.0.7 удалены и доступны в истории Git, а их имена зарезервированы упаковщиком.

Готовый установщик в `main` собирает [`publish-installer.yml`](../.github/workflows/publish-installer.yml)
после зелёного gate: модуль берётся из проверенного прогона (или собирается тем же `build.bat`),
архив проверяется `--verify --verify-source` и коммитится в ветку gate вместе со своим SHA-256.
Порядок и причины — в [описании net+fps](NET_V21_107_EXPERIMENTAL_NET_FPS.md), раздел «Как
публикуется текущий установщик».

Игровая гарантия 100% без проверки в игре недостижима. При отключении синхронизации или установке
неизвестных хуков нельзя скрывать отказ либо считать успех компиляции доказательством корректного боя.
Подробности последнего опубликованного архива: [`NET_V21_107_EXPERIMENTAL_NET_FPS.md`](NET_V21_107_EXPERIMENTAL_NET_FPS.md).

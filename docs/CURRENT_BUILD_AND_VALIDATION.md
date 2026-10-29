# Текущая сборка и правила проверки net_v21

## Устройство дерева

* `development/net-v21/src/` — рабочий C++ модуля и launcher. Исторический
  `SOURCE_MANIFEST.sha256` фиксирует исходный импорт и не является хеш-листом изменённой версии.
* `development/net-v21/tests/` — тесты, peer-стенды и вспомогательные фикстуры. По ранее принятому
  решению `named_shots.h` и его интеграционный тест пока остаются на прежнем месте в `src/`.
* `development/net-v21/fixtures/protocol17/` — три проверяемых файла для mixed-version теста;
  происхождение, SHA-256 и ограничения описаны в README рядом с ними.
* `development/upstream/` — неизменяемый публичный baseline; он не является рабочим `net_v21`.
  `development/net-v21/tools/` — инструменты, `build/` — игнорируемые результаты сборки.
* [`NETWORK_SESSION_2026-10-29.md`](NETWORK_SESSION_2026-10-29.md) отделяет подтверждённые
  наблюдения от предположений и описывает незавершённое расследование последнего вылета.

## Быстрые локальные проверки

Например, для изменённой логики кэша и проверки pinned fixture на Linux:

```sh
c++ -std=c++17 -O2 -Wall -Wextra -Werror -Idevelopment/net-v21/src \
  development/net-v21/tests/transform_gc_test.cpp -o /tmp/mius-transform-gc-test
/tmp/mius-transform-gc-test
c++ -std=c++17 -O2 -Wall -Wextra -Werror -Idevelopment/net-v21/src \
  -Idevelopment/net-v21/tests \
  development/net-v21/tests/image_mirror_identity_test.cpp -o /tmp/mius-image-test
/tmp/mius-image-test
python3 development/net-v21/tools/extract_legacy_shots_fixture_selftest.py
python3 development/net-v21/tools/pause_session_slice_selftest.py
```

`image_mirror_identity_test` покрывает зеркалирование сторон и перепись командных тегов
(`image::squad_tags()`), `pause_session_slice_selftest.py` извлекает настоящие функции паузы
и шапки состояния из `session.cpp` и проверяет их на подставном движке.

Windows-зависимые части можно отдельно проверить Zig через `development/net-v21/tools/syntax/check.sh`;
это не замена MSVC и не запуск игры. В Windows с VS2022 из `development/net-v21` используются
`build.bat module`, `build.bat selftest` и `build.bat join`. `selftest` строит отдельные `.exe`;
их выполнение и межпроцессные сценарии заданы в финальном workflow.

## Финальная проверка и выпуск

Полный gate запускается **один раз на готовом non-draft PR**: Windows/MSVC, отдельные и двухпроцессные
стенды, сборка модуля и launcher, установщик с rollback/upgrade, Linux/native и проверки архивов.
Изменение HEAD после запуска делает результат устаревшим и требует повторной проверки. Фокусные
проверки не заменяют gate; CI не доказывает работу игры и Steam. Исторические файлы `_known_good`,
все существующие архивы 1.0.7 и другие разрешённые архивы не перезаписываются.

Игровая гарантия 100% без проверки в игре недостижима. При отключении синхронизации или установке
неизвестных хуков нельзя скрывать отказ либо считать успех компиляции доказательством корректного боя.
Подробности последнего опубликованного архива: [`NET_V21_107_EXPERIMENTAL_NET_FPS.md`](NET_V21_107_EXPERIMENTAL_NET_FPS.md).

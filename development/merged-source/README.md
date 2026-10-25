# Объединённая исходная рабочая область MiusPlayer + MiusVitals

Эта директория снимает прежнее ограничение «только 16 файлов» **для разработки**: рядом с исходником Vitals теперь хранится закреплённая, проверяемая копия публичного исходника MiusPlayer. Она предназначена для чтения, правки, white-box проверки и будущей полной сборки. Это не папка установки игры и не замена `MiusVitals_v7.5.0_INSTALL.zip`.

## Состав и граница слоёв

| Слой | Путь | Назначение |
| --- | --- | --- |
| Публичный MiusPlayer | [`../upstream/miusplayer-public-43b2736/`](../upstream/miusplayer-public-43b2736/) | C++ `basic_network`, исходники program-pack tooling и шаблоны mpmod1 |
| MiusVitals sidecar | [`../kit-source/src/mp_vitals.c`](../kit-source/src/mp_vitals.c) | Узкие проверяемые guards и operation tactical policy для установленного бинаря |
| Проверяемый игроком kit | [`../kit-source/`](../kit-source/) | Существующий install flow; не используйте его как базу для C++-редактирования |

Публичный source snapshot — `VampyrCat/MiusPlayer` commit `43b2736d3b4a1e208e6c80f45f6ebbdd476de3aa`, лицензия CC0. `UPSTREAM.json` и `SOURCE_MANIFEST.sha256` фиксируют происхождение и каждый включённый файл. Из upstream намеренно исключены object/exe/codelib outputs, Ghidra database и screenshots: они не являются редактируемым исходным кодом и только скрывают настоящую разницу сборки.

## Почему это действительно связано с FPS-исправлением

`basic_network/session.cpp` публичного источника содержит целевую native semantics:

```cpp
return forced >= 0 ? forced != 0 : g_host_mode != HOST_MODE_OPERATION;
```

То есть operation tactical battle должен реплицировать positions без full saved state, если `MP_SYNC_STATE` не задан. MiusVitals v7.5 восстанавливает только этот **отсутствующий** process default для строго распознанного operation-tactical command line; он не меняет `g_host_mode`, map state или codelib. Так C++ источник и sidecar теперь лежат в одном редактируемом дереве и проверяются одной командой:

```sh
python3 development/merged-source/verify_workspace.py
```

Проверка утверждает hash каждого vendored source file, связь опубликованного базового codelib с v20 input и оба конца policy boundary. Она не является игровым тестом.

## Полная пересборка: безопасный путь

На Windows с Visual Studio 2022 C++ tools публичный C++ кандидат строится штатным upstream script:

```bat
cd development\upstream\miusplayer-public-43b2736\basic_network
build.bat
```

Перед этим запускайте `build_public_source.bat` из этой директории: он верифицирует source manifest, запускает upstream build и печатает hash кандидата.

**Не копируйте этот C++ output в игру и не пропускайте его через текущий `patch_import.py`.** Публичный source build, уже лежащий upstream, имеет SHA-256 `eff7ae…f7af5`, тогда как опубликованный v20 module имеет `8108bd…5723`. Последний совпадает с проверенным input текущего MiusVitals release pipeline, но source-build output не совпадает. Поэтому source tree делает исправления обозримыми, но пока не доказывает бинарную совместимость адресных Vitals guards.

Точки переноса каждой текущей функции Vitals в C++ и обязательные условия собраны в [`PORTING_LEDGER.md`](PORTING_LEDGER.md).

Чтобы превратить полную C++ пересборку в installable release, нужно выполнить все этапы:

1. внести изменение в vendored C++ source и добавить его как явный patch/commit в этой рабочей области;
2. собрать candidate MSVC toolchain и закрепить его timestamp, PE layout, exports и hashes;
3. либо перенести нужную Vitals логику в C++, либо создать новый проверенный sidecar profile — не применять старые RVA/signatures к новому binary;
4. добавить native/PE tests и двух-peer game evidence;
5. только затем расширять player archive или выпускать новый install kit.

## Явный non-scope

Глобальная карта остаётся без изменений. Наличие upstream `session.cpp` не даёт права смешивать или выпускать map edits без отдельной доказательной цепочки.

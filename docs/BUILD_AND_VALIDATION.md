# Сборка и проверка MiusVitals v7.1.0

## Воспроизводимые входы

`development/kit-source/src/build_release.sh` принимает оригинальный `basic_network.codelib` из MiusPlayer v20 и проверяет его SHA-256 до изменений.

| Вариант | Преобразование | SHA-256 |
| --- | --- | --- |
| Исходный MiusPlayer v20 | входной файл | `8108bd1e9af8080ef8b1050512b8311848f5a336acb49a7479388733b4ae5723` |
| Main | rebase 3598/1 + импорт DLL | `7caf36cddabf7eb388a3c6c393b05733e051955d63d32ef13531ce88004dcfba` |
| `nostart` | rebase без группы Start + импорт DLL | `2fb049b9b465de2e05bd9bbe7d5573a772319cc4d65fb20bb146e648a7cdf8ec` |
| `norebase` | только импорт DLL | `d2e34581b42d0573547e221873d7538b64a413267e8ed388abeb46dd42b630d2` |
| `mp_vitals.dll` | Zig 0.17.0, no CRT, KERNEL32 only | `83387d8434241773eed6216224cf9bd20ebac61167c5e675fad9a6fb101a5bf0` |
| `MiusVitals_v7.1.0_INSTALL.zip` | fixed-time exact 16-file install kit | `7c98cf37188107cd84e80f0adf079f6ee43d24e88596cc720d590d9fff9e881d` |

## Exact downloadable kit

`development/kit-source/` is the editable development tree and is **not** an install directory. It can contain manifest and verification helpers that deliberately do not reach players. `scripts/package_kit.py` is the source-of-truth allow-list for the only installable download, `MiusVitals_v7.1.0_INSTALL.zip`. It writes only these sixteen files under a single `MiusVitals/` root:

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

The builder fixes every ZIP entry timestamp to 2026-10-25 00:00:00, sorts the explicit tree, fixes permission metadata, and compresses at level 9. `--verify` checks CRCs, the ordered tree (including only required directories), timestamps, and byte-for-byte equality to the kit inputs.

```sh
python3 scripts/package_kit.py --output MiusVitals_v7.1.0_INSTALL.zip
python3 scripts/package_kit.py --verify MiusVitals_v7.1.0_INSTALL.zip
sha256sum MiusVitals_v7.1.0_INSTALL.zip
```

The generated ZIP is committed so it is directly downloadable from the repository; re-running the command with the same checked-in kit must retain the hash above.

## Local validation

```sh
# Portable C logic test: GCC/Clang, AddressSanitizer and UndefinedBehaviorSanitizer.
scripts/test_native.sh

# codelib/DLL SHA-256 + PE import/export surface + exact ZIP + native test.
scripts/verify_release.sh
```

The native harness tests infantry/vehicle state rules, no echo, receiver mailbox handling, epoch/sequence validation, duplicate ACK behaviour, full-mailbox no-ACK behaviour, exact retransmission, retry and bounded sender retention. It also asserts that native MiusPlayer types 10 and 11 pass through the hook unchanged. The render guard test accepts only the proven `basic_render` read-AV context; valid indices, write faults, another RIP, and a bad fallback continue normal exception search.

## Full rebuild

```sh
python3 -m venv /tmp/mius-v7-venv
/tmp/mius-v7-venv/bin/pip install -r development/kit-source/src/requirements-build.txt
ZIG_PYTHON=/tmp/mius-v7-venv/bin/python development/kit-source/src/build_release.sh \
  /path/to/mpmod1/files/bin/rel/x64/basic_network.codelib
scripts/verify_release.sh
```

`build_release.sh` regenerates the three module variants, compiles the DLL, verifies them, then writes the exact downloadable ZIP at the repository root. It intentionally writes no game installation. Review resulting SHA-256 values before publishing if changing the toolchain or source.

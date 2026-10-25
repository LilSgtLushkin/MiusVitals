# Сборка и проверка MiusVitals v7.0.1

## Воспроизводимые входы

`MiusVitals/src/build_release.sh` принимает оригинальный `basic_network.codelib` из MiusPlayer v20. Скрипт откажется работать с любым другим SHA-256.

| Вариант | Преобразование | SHA-256 |
| --- | --- | --- |
| Исходный MiusPlayer v20 | входной файл | `8108bd1e9af8080ef8b1050512b8311848f5a336acb49a7479388733b4ae5723` |
| Main | rebase 3598/1 + импорт DLL | `7caf36cddabf7eb388a3c6c393b05733e051955d63d32ef13531ce88004dcfba` |
| `nostart` | rebase без группы Start + импорт DLL | `2fb049b9b465de2e05bd9bbe7d5573a772319cc4d65fb20bb146e648a7cdf8ec` |
| `norebase` | только импорт DLL | `d2e34581b42d0573547e221873d7538b64a413267e8ed388abeb46dd42b630d2` |

## Локальная проверка

```sh
# Нативная логика: GCC/Clang + ASan/UBSan, Linux/macOS.
scripts/test_native.sh

# Проверка PE-импортов и контрольных сумм подготовленного комплекта.
scripts/verify_release.sh
```

`test_native.sh` моделирует солдат, технику, очередь пакетов и перехват кадра; он проверяет смерть, ранение, отсутствие обратного эха, режимы только-диагностики, возобновление надёжного потока после постоянной дыры, точечный патч idle-интервала и v7.0.1 render guard. Для guard тестируется только подтверждённый read-AV/контекст; нормальный индекс, запись, иной RIP и неверный fallback должны пройти дальше без перехвата.

## Rebuild

```sh
python3 -m venv /tmp/mius-v7-venv
/tmp/mius-v7-venv/bin/pip install -r MiusVitals/src/requirements-build.txt
ZIG_PYTHON=/tmp/mius-v7-venv/bin/python MiusVitals/src/build_release.sh \
  /path/to/mpmod1/files/bin/rel/x64/basic_network.codelib
scripts/verify_release.sh
```

Скрипт перезаписывает только артефакты внутри `MiusVitals/files/`. Перед заменой рабочего комплекта рекомендуется сохранить его копию.

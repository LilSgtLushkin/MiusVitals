MiusVitals v7.5.0 — implementation layer; НЕ отдельный install-kit
=====================================================================

ВАЖНО: игрокам следует ставить только единый MiusMultiplayer_v1.0.0_INSTALL.zip. Этот каталог сохраняет исходник/исторические компоненты Vitals для разработки и проверки; не запускайте его Install_Vitals.bat как отдельную установку.

Этот internal layer относится к Steam game build 3598/1 + MiusPlayer v20 и содержит
сохраняемые исходники/manifest Vitals. Runtime install flow намеренно вынесен в
`development/unified-kit-source/` и архив `MiusMultiplayer_v1.0.0_INSTALL.zip`.

Не используйте Install_Vitals.bat, Uninstall_Vitals.bat или Check_Build.bat из
этой директории на игровом ПК: это legacy implementation material для анализа
старого отдельного sidecar flow. Для установки и удаления используйте только
`MiusMultiplayer\Enable_Multiplayer.bat` и `Disable_Multiplayer.bat` из единого
архива.

Глобальная карта: без изменений в v7.5.0
------------------------------------------
Пользователь по-прежнему наблюдает несинхронизированные ходы на глобальной карте.
Поэтому эта версия сознательно НЕ меняет `adv_opgame`, `basic_network.codelib`,
map-mode, ownership или Next Turn. Оба codelib-варианта сохранены байт-в-байт;
main SHA-256 остаётся

  485B655662BD062E16278F9D699A3097B49A08DE1948D2500DD589703A3102A3

Не используйте карту как тест этого тактического исправления и не подменяйте
codelib вручную. Когда исследование карты будет разрешено вновь, нужны оба full
`basic_network_opgame_*.log`, оба `MiusVitals\build_report.txt` и `.opsave`,
созданный после hand-off.

Только installer и DLL обновлены в этом релизе; у обоих игроков должны совпадать
выбранный main/nostart codelib и новый DLL:

  mp_vitals.dll  71A1BF3A37923BBE6C8EE84448D0B91705C2BFF7382B647F990B9D671EE224DF

Тактический FPS: operation state policy
-------------------------------------
В slow tactical logs operational battle запускался с full saved-object state
replication, хотя native operation semantics требует positions/shots/hits/Vitals,
не object snapshots. Это даёт тысячи `state:` объектов и работу remote transforms
каждые 5 секунд; это не решается throttle.

До кэширования native policy DLL распознаёт только command line с
`tacgame.progpack` и отдельным token `19YY_MM_campaign:battle`. Только когда
`MP_SYNC_STATE` отсутствует, она задаёт process default `MP_SYNC_STATE=0`.
Skirmish, custom mission, `opgame` и любое явно заданное MP_SYNC_STATE остаются
нетронутыми. DLL не пишет g_host_mode, map state и codelib.

После загрузки log одного operation tactical battle должен содержать ровно:

  vitals: tactical operation detected; full object-state replication is disabled (native position, shots, hits and Vitals remain)

Перед заявлением исправления обязателен живой two-peer operation test: после
object-number exchange `state:` saved-state traffic отсутствует, а transforms,
shots и Vitals работают у обоих игроков. Native tests доказывают только
command-line classifier и absent-only/explicit-override policy.

Object-number exchange и Vitals
-------------------------------
Номера descriptions/objects в operation battle могут быть разными на двух ПК.
Поэтому Vitals не считает первый exchange необязательной диагностикой:

* до первого native `ready` он каждые 3 секунды запрашивает передачу таблиц;
* до этого момента не посылает, не применяет и не ACK-ает Vitals state;
* первый успешный exchange **latched на весь текущий tactical session**. Raw
  v20 ready-flag затем штатно кратко пульсирует в 0 для других transfer cycles,
  но больше не заставляет Vitals повторно переводить тысячи descriptions и не
  останавливает Vitals state sync;
* latch сбрасывается только при закрытии/смене tactical session или смене peer.

В нормальном логе идут строки только старта:

  vitals: waiting for object-number exchange before tactical state sync
  vitals: object-number exchange complete; tactical state sync enabled

Длинная Vitals telemetry раз в 5 секунд теперь выключена по умолчанию;
`MP_LOG_VITALS=1` включает её для диагностики. Если первый exchange не
завершается, сохраните оба full tactical log. Не пытайтесь компенсировать это
заменой DLL лишь на одной машине.

Итог боя, статистика и трупы
----------------------------
Перед выполнением послебоевой статистики Vitals принудительно сканирует
монотонные состояния смерти/ранения ещё раз; запись:

  vitals: final tactical state scan before battle statistics

означает, что последний snapshot опубликован. Статистическая функция использует
одинаковый seed у обоих игроков. Глобальная карта далее авторитетно продолжается
по обработанному host save и передаётся joiner целиком на новом map epoch.

Синхронизируются здоровье, смерть и squad record. Этот forced scan проходит через
обычный ACK/retry Vitals, но v7.5.0 пока НЕ устанавливает отдельный блокирующий
ACK-barrier именно в момент выхода из tactical game. Поэтому запись о final scan
не доказывает, что peer подтвердил её до аварийно быстрого закрытия боя.

Вид конкретного трупа/ragdoll на карте боя может оставаться локальным
render/physics результатом движка; он не является источником campaign result.
v7.5.0 не пишет guessed offsets в corpse lifecycle. Если при совпадающем dead
клиентское тело не появляется, необходимы логи и проверенные engine addresses.
Для сравнения проверяйте строки Vitals: после нормального боя dead here должен
согласовываться, а still apart — быть 0.

Корень нового basic_render crash и v7.5.0
------------------------------------------
Новый minidump доказывает write-AV в basic_render+129a2 при deployment. Это не
пакет сети: valid node record содержала 24-bit matrix/transform-cache slot,
который не принадлежит текущему renderer pool. Stock код без проверки делает
запись pool + slot * 64. Старый deployment crash с другим save произошёл и без
mp_vitals.dll, поэтому причина — нарушенный инвариант transient renderer cache,
а не Vitals/object numbers.

На строго проверенном Steam 3598 коде v7.5.0 перед каждым таким use проверяет,
что slot принадлежит текущему writable Windows allocation matrix pool. Иначе он
атомарно ставит штатный sentinel -2 в record+34. Затем неизменённый basic_render
идёт в свой cache-miss allocator, получает свежий slot и сам записывает его в
record. При редкой гонке exact write fallback направляет только подтверждённый
movaps в тот же allocator; render operation не пропускается и slot не выдумывается.

Нормальный log после загрузки renderer:

  vitals: basic_render cache-lifetime repair and invalid-child guards armed ...

Запись "rebuilt ... stale basic_render transform cache entries" означает, что
одна или несколько просроченных cache entries были пересозданы движком.

Текущий, отдельный read-AV был в basic_render+39b52 (`movups xmm9,[rax+rsi]`).
В dump `RAX=FAAEC630` уже является 32-bit index/stride-derived offset, а
`RSI+RAX` точно совпадает с invalid address. Stock triangle reader не проверяет
границы index-derived vertex/attribute address. v7.5.0 перед входом именно в эту
проверенную функцию сверяет весь index array и каждый будущий 16-byte primary и
optional attribute read. Если geometry некорректна, функция возвращает только
свой штатный no-hit (`false`, query+0xa4=0), не выполняя invalid load.

Это не broad exception handling. Двенадцать exact load instructions оставлены
в VEH только как backup гонки между preflight и engine read; совпасть должны
read AV, RIP, effective address и writable output. Любой другой exception
остаётся движку. Нормальная строка запуска теперь:

  vitals: basic_render cache-lifetime, geometry-index and invalid-child guards armed ...

А `skipped ... malformed basic_render geometry read ... no-hit path` означает,
что была отвергнута некорректная geometry; сохраните полный tactical log.
Подробный разбор dump и тестов: docs/TRIANGLE_GEOMETRY_ROOT_CAUSE_2026-10-25.md
в repository.

Сохранённые меры v7.1.1
-----------------------
* Vitals type 52 (0x34) имеет epoch+sequence, ACK type 53 (0x35), mailbox,
  retry через 400 ms и suppression duplicate.
* Native MiusPlayer type 10 (игровые записи) и type 11 (его ACK) не
  переписываются DLL и всегда передаются исходному обработчику.
* Неподвижные unit transforms по умолчанию остаются на штатной staggered
  cadence около 2 секунд. Только явный `MP_IDLE_MS=N` меняет интервал;
  `MP_IDLE_MS=0` — диагностический every-tick override, не оптимизация.
* MP_RENDER_GUARD=0 отключает только узкий guard двух подтверждённых
  basic_render invalid-child read-AV.

Переменные среды
----------------
MP_VITALS=0        выключить Vitals; 1 — только сравнение/лог; 2 или 3 — применять.
MP_VEHICLES=0/1/2  выключить / только лог / применять статусы техники.
MP_STATS=0         не задавать одинаковый seed послебоевой статистики.
MP_LOG_VITALS=1    включить individual events и 5-second aggregate telemetry для диагностики.
MP_IDLE_MS=N       ЯВНО изменить период повторной отправки неподвижных unit в мс.
                   По умолчанию patch не применяется; 0 = every tick (только диагностика).
MP_RENDER_GUARD=0  выключить только exact basic_render cache-lifetime, geometry-index и invalid-child guards для диагностики.

Контрольные суммы и состав
--------------------------
files/mp_vitals.dll                     SHA-256 71A1BF3A37923BBE6C8EE84448D0B91705C2BFF7382B647F990B9D671EE224DF
files/basic_network.codelib             SHA-256 485B655662BD062E16278F9D699A3097B49A08DE1948D2500DD589703A3102A3
files/alt/basic_network_nostart.codelib SHA-256 E4317E630A21198A55E0E86BE4AA29C71FCADA6065824C858ACD0E2BDEF8ADF9
files/alt/basic_network_v5_norebase.codelib SHA-256 D2E34581B42D0573547E221873D7538B64A413267E8ED388ABEB46DD42B630D2

В ZIP ровно 16 файлов под одним корнем MiusVitals/. Папка development из Git
не является установочным комплектом.

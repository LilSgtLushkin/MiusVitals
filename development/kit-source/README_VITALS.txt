MiusVitals v7.4.0 — глобальная карта, Vitals и guard basic_render
====================================================================

ВАЖНО: это комплект только для Steam game build 3598/1 + MiusPlayer v20.
ОБА игрока должны установить один и тот же MiusVitals v7.4.0.

Быстрая установка
-----------------
1. Установите оригинальный MiusPlayer v20 и полностью закройте игру.
2. Распакуйте этот архив в корень игры. Должен получиться путь:
   ...\Graviteam Tactics Mius-Front\MiusVitals\Install_Vitals.bat
   Рядом с папкой MiusVitals должны находиться starter_x64.exe и mpmod1.
3. Запустите Check_Build.bat. Он ничего не меняет и создаёт build_report.txt.
4. Запустите Install_Vitals.bat. Он сохранит исходный basic_network.codelib
   в MiusVitals\state и установит mp_vitals.dll в корень игры.
5. Установите этот же архив у второго игрока.

Откат: закройте игру и запустите Uninstall_Vitals.bat. Не удаляйте state
до отката.

Глобальная карта: что изменено
------------------------------
В Steam 3598/1 обработчик кнопки Next Turn в adv_opgame.codelib начинается
на +36b30. Предыдущий MiusPlayer-код проверял старый +36b40, поэтому безопасно
отказывался включать shared map turn и писал:

  map: unknown game build, ending the turn is not coordinated between the players

v7.4.0 в main/nostart basic_network.codelib содержит проверенное исправление:
* указатель Next Turn перенесён на +36b30;
* embedded signature заменена точным 16-байтным прологом целевой Steam 3598/1
  функции;
* адреса worker Next Turn (+76510) и operation save (+d6080) уже соответствуют
  этой сборке и также проходят свои строгие проверки.

При точном совпадении включается штатная логика MiusPlayer:
* хост передаёт клиенту зеркальную копию operation save;
* оба игрока одновременно отдают приказы и двигают свои formations/battle groups;
* клиент отдаёт свою карту назад; хост объединяет его numeric changes и новые
  user_orders с собственной картой, обрабатывает ход и присылает клиенту полную
  карту следующего epoch.

v7.4.0 устраняет конкретную гонку между выдачей карты и запуском отдельного
opgame клиента. Раньше host gate Next Turn требовал уже полученный map-mode Hello
(phase 0xfe). Но карта уже была выдана, а joiner ещё мог скачивать save/стартовать.
В это окно Next Turn хоста шёл в stock handler: ход сразу проигрывался, а клиент
попадал в следующую tactical battle вместо своей карты.

Теперь убрана только эта ранняя проверка map-mode Hello. Host ждёт, если карта уже
выдана и соединение ещё живо; он НЕ считает клиента готовым и НЕ делает merge,
пока не получит настоящий Hello с совпадающим epoch и Next Turn клиента. То есть
это не задержка по таймеру и не поддельное подтверждение — только устранение
неправильного перехода в stock Next Turn во время запуска уже выданной карты.

Проверка после запуска operation:
* на host: "map: the turn ends when both players have pressed next turn...";
* если host нажал Next Turn до появления карты у joiner: "map: this player has
  ended the turn, waiting for ..."; не должно быть "map: ending the turn";
* на joiner: "net: joining ...", затем "map: this player's side of the
  operational map...";
* только после Next Turn обоих допустимо "map: ending the turn - asking ...".

`map: unknown game build, ending the turn is not coordinated between the players`
означает, что detour не включился и требует полного лога/build_report. Напротив,
`map: unknown game build, what the other player's intelligence finds out is not
kept for him` относится только к persistence разведданных: он не отключает
корректно установленный Next Turn hook.

Подкрепления не отправляются отдельным неполным списком: существующие состояния
formations/groups входят в полную operation save и её числовой merge. Но текущий
merge структурно добавляет только новые строки user_orders; появление новой строки
подкрепления во время совместного turn ещё не подтверждено живой парой игроков.
Не обещайте такое новое reinforcement до этого теста.

Важно о сообщении «за вторым человеком действует бот»: supplied host log уже
показывает `391 formations and battle groups turned round`. Именно это меняет
operation command tags на USER для joiner; `AI commander switched off` в
тактическом log — другой механизм. Однако в том же host log за весь `opgame`
нет ни одной строки `vitals:`; у joiner такая строка есть. `mp_vitals.dll`
загружается только импортом проверенного `basic_network.codelib`, поэтому это
доказательство разных/неполно установленных модулей на двух ПК, а не повод
писать догадочный ownership patch.

v7.4.0 Install_Vitals.bat теперь сверяет после copy **оба** файла. У ОБОИХ
игроков должны совпасть main/nostart variant и строки:

  basic_network.codelib 485B655662BD062E16278F9D699A3097B49A08DE1948D2500DD589703A3102A3
  mp_vitals.dll          D22E931575A119136CCE9F40622EE90F8919EBD250F2738E3E50D7561C4FC486

Только после этого лог карты является валидным тестом. Если при этих hash и
строках `vitals:` на обоих ПК симптом остаётся, нужны обе full opgame logs,
оба build_report.txt и created-after-hand-off .opsave; не подменяйте module
вручную и не используйте `norebase` для normal Steam 3598/1.

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
обычный ACK/retry Vitals, но v7.4.0 пока НЕ устанавливает отдельный блокирующий
ACK-barrier именно в момент выхода из tactical game. Поэтому запись о final scan
не доказывает, что peer подтвердил её до аварийно быстрого закрытия боя.

Вид конкретного трупа/ragdoll на карте боя может оставаться локальным
render/physics результатом движка; он не является источником campaign result.
v7.4.0 не пишет guessed offsets в corpse lifecycle. Если при совпадающем dead
клиентское тело не появляется, необходимы логи и проверенные engine addresses.
Для сравнения проверяйте строки Vitals: после нормального боя dead here должен
согласовываться, а still apart — быть 0.

Корень нового basic_render crash и v7.4.0
------------------------------------------
Новый minidump доказывает write-AV в basic_render+129a2 при deployment. Это не
пакет сети: valid node record содержала 24-bit matrix/transform-cache slot,
который не принадлежит текущему renderer pool. Stock код без проверки делает
запись pool + slot * 64. Старый deployment crash с другим save произошёл и без
mp_vitals.dll, поэтому причина — нарушенный инвариант transient renderer cache,
а не Vitals/object numbers.

На строго проверенном Steam 3598 коде v7.4.0 перед каждым таким use проверяет,
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
границы index-derived vertex/attribute address. v7.4.0 перед входом именно в эту
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
files/mp_vitals.dll                     SHA-256 D22E931575A119136CCE9F40622EE90F8919EBD250F2738E3E50D7561C4FC486
files/basic_network.codelib             SHA-256 485B655662BD062E16278F9D699A3097B49A08DE1948D2500DD589703A3102A3
files/alt/basic_network_nostart.codelib SHA-256 E4317E630A21198A55E0E86BE4AA29C71FCADA6065824C858ACD0E2BDEF8ADF9
files/alt/basic_network_v5_norebase.codelib SHA-256 D2E34581B42D0573547E221873D7538B64A413267E8ED388ABEB46DD42B630D2

В ZIP ровно 16 файлов под одним корнем MiusVitals/. Папка development из Git
не является установочным комплектом.

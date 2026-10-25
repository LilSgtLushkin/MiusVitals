MiusVitals v7.2.0 — глобальная карта, Vitals и guard basic_render
====================================================================

ВАЖНО: это комплект только для Steam game build 3598/1 + MiusPlayer v20.
ОБА игрока должны установить один и тот же MiusVitals v7.2.0.

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

v7.2.0 в main/nostart basic_network.codelib содержит проверенное исправление:
* указатель Next Turn перенесён на +36b30;
* embedded signature заменена точным 16-байтным прологом целевой Steam 3598/1
  функции;
* адреса worker Next Turn (+76510) и operation save (+d6080) уже соответствуют
  этой сборке и также проходят свои строгие проверки.

При точном совпадении включается штатная логика MiusPlayer:
* хост передаёт клиенту зеркальную копию operation save;
* оба игрока одновременно отдают приказы и двигают свои formations/battle groups;
* Next Turn только помечает игрока готовым; ход обрабатывается после готовности
  обоих игроков;
* клиент отдаёт свою карту назад; хост объединяет его numeric changes и новые
  user_orders с собственной картой, обрабатывает ход и присылает клиенту полную
  карту следующего epoch.

Подкрепления не отправляются отдельным неполным списком: существующие состояния
formations/groups входят в полную operation save и её числовой merge. Но текущий
merge структурно добавляет только новые строки user_orders; появление новой строки
подкрепления во время совместного turn ещё не подтверждено живой парой игроков.
Не обещайте такое новое reinforcement до этого теста.

Проверка после запуска operation:
* на host: "map: the turn ends when both players have pressed next turn...";
* на joiner: "map: this player's side of the operational map...";
* не должно быть "map: unknown game build...".

Если последняя строка всё же есть, не меняйте codelib вручную: приложите полный
basic_network_opgame_*.log и build_report.txt. Detour должен оставаться выключен
на непроверенной сборке, а не прыгать в неизвестный код.

Object-number exchange и Vitals
-------------------------------
Номера descriptions/objects в operation battle могут быть разными на двух ПК.
Поэтому Vitals v7.2.0 не считает exchange необязательной диагностикой:

* пока не появится "object-number lists exchanged: yes", он каждые 3 секунды
  запрашивает нативную передачу таблиц;
* до yes не посылает, не применяет и не ACK-ает свой Vitals state;
* сразу после yes включает state sync и отправляет полный актуальный snapshot.

В нормальном логе идут строки:

  vitals: waiting for object-number exchange before tactical state sync
  vitals: object-number exchange complete; tactical state sync enabled
  vitals: object-number lists exchanged: yes (asked again N times)

Состояние NO во время всего боя — основание остановить диагностику результата и
сохранить оба полных tactical log. Не пытайтесь компенсировать его заменой DLL
лишь на одной машине.

Итог боя, статистика и трупы
----------------------------
Перед выполнением послебоевой статистики Vitals принудительно сканирует
монотонные состояния смерти/ранения ещё раз; запись:

  vitals: final tactical state scan before battle statistics

означает, что последний snapshot опубликован. Статистическая функция использует
одинаковый seed у обоих игроков. Глобальная карта далее авторитетно продолжается
по обработанному host save и передаётся joiner целиком на новом map epoch.

Синхронизируются здоровье, смерть и squad record. Этот forced scan проходит через
обычный ACK/retry Vitals, но v7.2.0 пока НЕ устанавливает отдельный блокирующий
ACK-barrier именно в момент выхода из tactical game. Поэтому запись о final scan
не доказывает, что peer подтвердил её до аварийно быстрого закрытия боя.

Вид конкретного трупа/ragdoll на карте боя может оставаться локальным
render/physics результатом движка; он не является источником campaign result.
v7.2.0 намеренно не пишет guessed offsets в lifecycle render/corpse. Если при
совпадающем dead клиентское тело не появляется, необходимы логи и проверенные
engine addresses. Для сравнения проверяйте строки Vitals: после нормального боя
dead here должен согласовываться, а still apart — быть 0.

Сохранённые меры v7.1.1
-----------------------
* Vitals type 52 (0x34) имеет epoch+sequence, ACK type 53 (0x35), mailbox,
  retry через 400 ms и suppression duplicate.
* Native MiusPlayer type 10 (игровые записи) и type 11 (его ACK) не
  переписываются DLL и всегда передаются исходному обработчику.
* Неподвижные unit transforms повторяются каждый tick по умолчанию
  (MP_IDLE_MS=N меняет интервал).
* MP_RENDER_GUARD=0 отключает только узкий guard двух подтверждённых
  basic_render invalid-child read-AV.

Переменные среды
----------------
MP_VITALS=0        выключить Vitals; 1 — только сравнение/лог; 2 или 3 — применять.
MP_VEHICLES=0/1/2  выключить / только лог / применять статусы техники.
MP_STATS=0         не задавать одинаковый seed послебоевой статистики.
MP_LOG_VITALS=1    писать отдельные Vitals-события.
MP_IDLE_MS=N       период повторной отправки неподвижных unit в миллисекундах.
MP_RENDER_GUARD=0  выключить только render guard для диагностики.

Контрольные суммы и состав
--------------------------
files/mp_vitals.dll                     SHA-256 8F336D5645C01A394B7BE776FDA704E0910ADB949083C16FEA6BA25DF7294F3E
files/basic_network.codelib             SHA-256 4C91874C4B62DB424ECCA36586121BCB3B4CC7849CC900285E68DE5151EDDC36
files/alt/basic_network_nostart.codelib SHA-256 F0766BEC48696869D59D4A654A04F20BD7BAF9F73ADB6FBF139E7DA31DFC1F4F
files/alt/basic_network_v5_norebase.codelib SHA-256 D2E34581B42D0573547E221873D7538B64A413267E8ED388ABEB46DD42B630D2

В ZIP ровно 16 файлов под одним корнем MiusVitals/. Папка development из Git
не является установочным комплектом.

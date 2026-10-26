# Что именно v20 добавляет к публичному исходнику: строки и функции

Строк только в v20: **379** (в v12 их нет). Ниже они сгруппированы по префиксу сообщения;
для каждой указана функция, которая её использует, и её статус в разборе:
`SAME` — код совпадает с v12, `CHANGED` — v12 с правками, `NEW` — кода в v12 нет.

## other — 241

| строка | RVA функции | статус функции |
|---|---|---|
| `  Your public address: ` | 024be4 | NEW |
| ` (Withdraw pressed)` | 03de68 | NEW |
| ` (a friend's game, while this game runs)` | 03f5cc | NEW |
| ` (a support could not be carried: its squad is an ordinary one)` | 0207c0 | NEW |
| ` (its player pressed Withdraw)` | 03d280 | NEW |
| ` (on the command line)` | 03f5cc | NEW |
| ` (test stand-in)` | 03fba0 | NEW |
| ` (this game's clock follows the host's)` | 019f88 | NEW |
| ` - sent` | 0364dc | NEW |
| ` - set YEAR AND SEASON back to the host's` | 0270ac | NEW |
| ` - set your own side to the ` | 0270ac | NEW |
| ` and of the craters made here so far` | 0378e8 | NEW |
| ` seen from this side` | 03d73c | NEW |
| `"%s\starter_x64.exe" root\programs\tacgame.progpack, %s, users\profiles, %s, %s` | 027cbc | CHANGED |
| `$@&4(.*:(` | — | — |
| `%95s %u` | 0184fc | NEW |
| `%s asks for a pause - pause as well to agree` | 01daa0 | CHANGED |
| `%s asks for speed x%.1f - set your speed to agree` | 01daa0 | CHANGED |
| `%s is withdrawing - the battle ends in %d:%02d` | 01daa0 | CHANGED |
| `%s proposes a ceasefire  (ESC menu: CEASEFIRE to accept)` | 01daa0 | CHANGED |
| `%s proposes a ceasefire. The battle ends when you accept.` | 024820 | NEW |
| `%s, %s, %s` | 00ecf0 | NEW |
| `%s\%s.opsave` | 01ede4 | NEW |
| `%s\*.damsav2` | 02d208 | NEW |
| `%s\users\mp_battle_setup.cfgpack` | 018f20 | NEW |
| `%s\users\mp_my_force.cfgpack` | 0273a8, 02a47c, 02ce78 | NEW |
| `%s\users\mp_peer_force.cfgpack` | 010964, 017de4 | NEW |
| `%s\users\mp_saves\_intel_found.txt` | 012788 | NEW |
| `%s\users\mp_saves\_known_%s.txt` | 012e08 | NEW |
| `%s\users\profiles\%s\ccombat.cfgpack` | 017efc, 01bd64, 01bf54 | NEW |
| `%sgtmf_fake_steam_%llu.txt` | 03ea40 | NEW |
| `%u platoons, %u squads, %u units in place of %u, %u, %u; %u platoons placed where the battle's stood; %u supports%s` | 0207c0 | NEW |
| `*.cfgpack` | 02d208 | NEW |
| `*.tcsprofile` | 02d208 | NEW |
| `*:*, 0, 1` | 027cbc | CHANGED |
| `+?)\O?` | — | — |
| `+gtmf_join=` | 02ce78, 03edc0, 03efa8 | NEW |
| `, sent` | 036d38 | NEW |
| `, the other side left` | 03d620 | NEW |
| `, this side left` | 03d620 | NEW |
| `.?AUJoinRequested@@` | — | — |
| `.?AUPresenceUpdate@@` | — | — |
| `.?AUSessionFail@@` | — | — |
| `.?AUSessionRequest@@` | — | — |
| `.?AVCallbackBase@@` | — | — |
| `.damsav2` | 029714 | NEW |
| `@SUAUAWH` | — | — |
| `@SUVAUAVH` | — | — |
| `@SUVWATAVAWH` | — | — |
| `@USVWATAUAVAWH` | — | — |
| `A friend who hosts shows up here; joining him needs no address and no port.` | 026470 | NEW |
| `ACCEPT CEASEFIRE` | 024820 | NEW |
| `ATAVAWH` | — | — |
| `CEASEFIRE` | 024820 | NEW |
| `CEASEFIRE AGREED` | 024820 | NEW |
| `CEASEFIRE PROPOSED` | 024820 | NEW |
| `CHOOSE MY FORCE` | 024be4 | NEW |
| `Choose your force on the Skirmish screen: what it costs is the most the other player's own force may cost.` | 024be4 | NEW |
| `F %llu dropped side=%08x kind=%u pos=%.2f,%.2f,%.2f` | — | — |
| `F %llu own side=%08x kind=%u pos=%.2f,%.2f,%.2f` | — | — |
| `F %llu received side=%08x kind=%u pos=%.2f,%.2f,%.2f` | — | — |
| `GetFileAttributesExA` | — | — |
| `GetTempPathA` | — | — |
| `Host port (UDP)` | 026470 | NEW |
| `Joining through Steam is done from the game's menu, with Steam running.` | 02a130 | NEW |
| `Looking for %s through Steam ...  (%u s)` | 024be4 | NEW |
| `MP_AI_ALLIES` | 0229e8 | NEW |
| `MP_AUTOJOIN_READY` | 02a47c, 02ce78 | NEW |
| `MP_BATTLE_SETUP` | 012008, 012edc, 018f20 | CHANGED/NEW |
| `MP_FORCE_COST` | 02a47c, 02ce78 | NEW |
| `MP_FORCE_FILE` | 02a47c, 02ce78 | NEW |
| `MP_FORCE_LIMIT` | 019f20, 01c33c | NEW |
| `MP_FORCE_NOTE` | 02a47c, 02ce78 | NEW |
| `MP_JOIN_BATTLE_OF` | 027cbc | CHANGED |
| `MP_JOIN_PLAIN` | 027cbc | CHANGED |
| `MP_JOIN_PROFILE` | 027cbc | CHANGED |
| `MP_LOG_HITS` | 03cedc | NEW |
| `MP_PEER_FORCE` | 010964, 012008, 014054 | NEW |
| `MP_SETUP_DATE` | 01c430, 01e870 | NEW |
| `MP_STATE_TRANSLATE` | 02eab0 | NEW |
| `MP_STEAM` | 03f75c | NEW |
| `MP_STEAM_FAKE` | 03f75c | NEW |
| `MP_STEAM_PEER` | 011148, 016230 | NEW |
| `MP_SYNC_ANIM` | 03be00 | NEW |
| `MP_SYNC_CRATERS` | 037f98 | NEW |
| `MP_SYNC_FACING` | 03be00 | NEW |
| `MP_SYNC_HITS` | 03cedc | NEW |
| `MP_TEST_AI_OFF` | 0229e8 | NEW |
| `MP_TEST_TRACE` | 01c544 | NEW |
| `MP_TRACE_PLACES` | 037f98 | NEW |
| `MP_UPNP` | 01c4b0, 01d33c | CHANGED/NEW |
| `MULTIPLAYER - the host has not chosen his force yet: its cost will be your limit` | 0270ac | NEW |
| `MULTIPLAYER - the host's battle has begun: your own force can only go into the next one` | 0270ac | NEW |
| `MULTIPLAYER - your own force: cost %u of %u allowed (what the host's force costs)%s%s%s` | 0270ac | NEW |
| `NOT hooked` | 0347c4 | NEW |
| `No answer yet. Check the address and the port, and that UDP port %u reaches the host.` | 024be4 | NEW |
| `No answer yet. Steam is looking for a way to the host's game; that can take a moment.` | 024be4 | NEW |
| `None of your Steam friends is hosting right now.` | 026470 | NEW |
| `Open it by UPnP` | 026470 | NEW |
| `Propose to end the battle now. It ends when %s chooses CEASEFIRE too.` | 024820 | NEW |
| `RtlCaptureStackBackTrace` | — | — |
| `STEAM FRIENDS HOSTING A GAME` | 026470 | NEW |
| `Steam is not available to this game.` | 026470 | NEW |
| `SteamAPI_ISteamFriends_GetFriendByIndex` | 03eb34 | NEW |
| `SteamAPI_ISteamFriends_GetFriendCount` | 03eb34 | NEW |
| `SteamAPI_ISteamFriends_GetFriendGamePlayed` | 03eb34 | NEW |
| `SteamAPI_ISteamFriends_GetFriendPersonaName` | 03eb34 | NEW |
| `SteamAPI_ISteamFriends_GetFriendRichPresence` | 03eb34 | NEW |
| `SteamAPI_ISteamFriends_GetPersonaName` | 03eb34 | NEW |
| `SteamAPI_ISteamFriends_RequestFriendRichPresence` | 03eb34 | NEW |
| `SteamAPI_ISteamFriends_SetRichPresence` | 03eb34 | NEW |
| `SteamAPI_ISteamNetworking_AcceptP2PSessionWithUser` | 03eb34 | NEW |
| `SteamAPI_ISteamNetworking_AllowP2PPacketRelay` | 03eb34 | NEW |
| `SteamAPI_ISteamNetworking_IsP2PPacketAvailable` | 03eb34 | NEW |
| `SteamAPI_ISteamNetworking_ReadP2PPacket` | 03eb34 | NEW |
| `SteamAPI_ISteamNetworking_SendP2PPacket` | 03eb34 | NEW |
| `SteamAPI_ISteamUser_BLoggedOn` | 03eb34 | NEW |
| `SteamAPI_ISteamUser_GetSteamID` | 03eb34 | NEW |
| `SteamAPI_ISteamUtils_GetAppID` | 03eb34 | NEW |
| `SteamAPI_RegisterCallback` | 03eb34 | NEW |
| `SteamAPI_SteamFriends_v017` | 03eb34 | NEW |
| `SteamAPI_SteamNetworking_v006` | 03eb34 | NEW |
| `SteamAPI_SteamUser_v021` | 03eb34 | NEW |
| `SteamAPI_SteamUtils_v010` | 03eb34 | NEW |
| `That force is of the %s; you command the %s in this battle. CHOOSE MY FORCE again with the %s as your side.` | 02a47c | NEW |
| `The battle ends a few seconds after the menu is closed. Press again to take your word back.` | 024820 | NEW |
| `The battle is in the host's year and season: set YEAR AND SEASON back to what the screen opened with, then pick your units.` | — | — |
| `The host has not chosen his force yet - its cost is your limit. Wait for it, then press start again.` | 029df8 | NEW |
| `The host sets up the battle on the Skirmish screen. The other player may pick his own force with CHOOSE MY FORCE;` | 024be4 | NEW |
| `The host's battle has begun - your own force can only go into the next one. Leave this screen and press READY to enter the battle.` | — | — |
| `The host's battle is still in deployment: CHOOSE MY FORCE (the %s, up to %u) puts your own force in - his battle starts again with it.` | — | — |
| `The port has to be a number from 1 to 65535 (empty: 1943).` | 02bb0c | NEW |
| `The port to host on has to be a number from 1 to 65535 (empty: 1943).` | 02bb0c | NEW |
| `UPnP is off - players outside your network need UDP port %u forwarded to this PC.` | 024be4 | NEW |
| `UPnP: asking the router to pass UDP port %u on to this PC ...` | 024be4 | NEW |
| `UPnP: the router did not open the port - forward UDP port %u to this PC by hand.` | 024be4 | NEW |
| `UPnP: the router passes UDP port %u on to this PC.%s%s` | 024be4 | NEW |
| `USWAUAWH` | — | — |
| `UWATAUAWH` | — | — |
| `VWATAVAWH` | — | — |
| `Waiting for %s to choose CEASEFIRE as well. Press again to take the proposal back.` | 024820 | NEW |
| `You command the %s in this battle: set your own side to the %s.` | 029df8 | NEW |
| `You command the %s. CHOOSE MY FORCE opens the Skirmish screen: the %s as your side, units costing up to %u (the host's force), then its start button.` | — | — |
| `You command the %s. Your own force may cost as much as the host's - he has not chosen his yet.` | 024be4 | NEW |
| `Your Steam friends find this game on their JOIN A GAME screen - they need no address and no open port.` | 024be4 | NEW |
| `Your force costs %u - the host's costs %u. Take something out.` | 029df8 | NEW |
| `Your force costs %u: the other player's own force may cost as much.` | 024be4 | NEW |
| `Your force could not be kept (users folder).` | 02a47c | NEW |
| `Your force could not be priced.` | 029df8 | NEW |
| `Your force could not be read from the Skirmish screen. Press CHOOSE MY FORCE to try again.` | 02a47c | NEW |
| `Your own force: %u squads of the %s - %s` | 024be4 | NEW |
| `Your own force: %u squads of the %s, cost %u.` | 02a47c | NEW |
| `_*.damsav2` | 00ecf0 | NEW |
| `_aligned_msize` | — | — |
| `_strnicmp` | — | — |
| `_strtoui64` | — | — |
| `a hit on a unit is its owner's game's: worked out there, sent, and carried out the same here` | 03cedc | NEW |
| `an unknown kind of value (%u) in table %s` | 0207c0 | NEW |
| `another player` | 016230 | NEW |
| `artillery` | 02aa74 | NEW |
| `asks for a pause` | 016230, 01e530 | NEW |
| `battle running` | 0371b0 | NEW |
| `being sent to the host...` | 024be4 | NEW |
| `blowup` | 02aa74 | NEW |
| `ceasefire` | 02aa74 | NEW |
| `ceasefire agreed - the battle ends in %d s` | 01daa0 | CHANGED |
| `chooseforce` | 02aa74 | NEW |
| `closed` | 014458 | NEW |
| `connect` | 03efa8 | NEW |
| `connect ` | 02aa74 | NEW |
| `could not be hooked, each game works out every hit for itself` | 03cedc | NEW |
| `could not be removed` | 022c84 | NEW |
| `could not be traced` | 037f98 | NEW |
| `counted by where they come from` | 037f98 | NEW |
| `custom mission` | 026470 | NEW |
| `did not get to stamping` | 0361d4 | NEW |
| `dropped-offmap` | 033684 | CHANGED |
| `each game makes its own` | 037f98 | NEW |
| `each game makes its own (MP_SYNC_CRATERS=0)` | 037f98 | NEW |
| `fake%llu` | 03f6e0, 03f75c | NEW |
| `fights on` | 03d280 | NEW |
| `flares` | 02aa74 | NEW |
| `he has none any more` | 01083c | NEW |
| `his own force: %u squads of the %s` | 024be4 | NEW |
| `his own force: %u squads of the %s, cost %u` | 024be4 | NEW |
| `hooked` | 0347c4 | NEW |
| `hosts %s %s` | 026470 | NEW |
| `hostskirmish` | 02aa74 | NEW |
| `i_divisions:divisions` | 00da80 | NEW |
| `i_opgame:opgame` | 017efc, 01bd64, 01bf54 | NEW |
| `is removed` | 022c84 | NEW |
| `isdigit` | — | — |
| `l$ VWAUAVAWH` | — | — |
| `lobbyready` | 02aa74 | NEW |
| `lroundf` | — | — |
| `made his stamp` | 0361d4 | NEW |
| `menuoff` | 02aa74 | NEW |
| `mp_guest` | 027cbc, 02d208 | CHANGED/NEW |
| `no longer asks for a pause` | 016230, 01e530 | NEW |
| `not known` | 0347c4 | NEW |
| `off already` | 020190 | CHANGED |
| `opened` | 014458 | NEW |
| `opgame.progpack` | 00ffe4 | NEW |
| `otherwise he commands the one the setup gives him. SWAP SIDES exchanges who commands which.` | 024be4 | NEW |
| `own-offmap` | 033684 | CHANGED |
| `p WAVAWH` | — | — |
| `paused by both players - it goes on when either of you un-pauses` | 01daa0 | CHANGED |
| `period` | 01bf54, 01e870 | NEW |
| `pl_%s_%u` | 0207c0 | NEW |
| `proposes a ceasefire` | 03d280, 03e690 | NEW |
| `root\programs\opgame.progpack, %ssteam:%llu` | 03edc0, 03efa8 | NEW |
| `skirmishstart` | 02aa74 | NEW |
| `speed ` | 02aa74 | NEW |
| `speed x%.1f, agreed by both players` | 01daa0 | CHANGED |
| `stamped a trench` | 0361d4 | NEW |
| `stamped nothing` | 0361d4, 036d38 | NEW |
| `stamped nothing either` | 0361d4 | NEW |
| `stamped tile %u cell %u kind %u direction %u variant %d flags %x (%d calls)` | 036d38 | NEW |
| `steam_api64.dll` | 03f75c | NEW |
| `steamjoin` | 02aa74 | NEW |
| `steamrequest ` | 02aa74 | NEW |
| `strnlen` | — | — |
| `supports` | 00da80 | NEW |
| `t$ WMP_SYNC_END` | — | — |
| `table %s is not the same in both descriptions` | 0207c0 | NEW |
| `table %s not found` | 0207c0 | NEW |
| `tacgame.progpack` | 02a47c | NEW |
| `takes the ceasefire proposal back` | 03d280, 03e690 | NEW |
| `the battle has no pool for that side` | 0207c0 | NEW |
| `the chosen force has no units of that side` | 0207c0 | NEW |
| `the chosen force's squads have no command tag` | 0207c0 | NEW |
| `the ground is the host's: its craters are sent and made the same here` | 037f98 | NEW |
| `the host cannot use it - it is not of your side.` | 024be4 | NEW |
| `the host has it.` | 024be4 | NEW |
| `turn_flags` | 01fc38 | CHANGED |
| `while the battle is built` | 0371b0 | NEW |
| `withdraw` | 02aa74 | NEW |
| `you have asked for a pause - it starts when %s pauses too` | 01daa0 | CHANGED |
| `you have asked for speed x%.1f - the battle runs at x%.1f until %s asks for it too` | 01daa0 | CHANGED |
| `you have proposed a ceasefire - waiting for %s` | 01daa0 | CHANGED |
| `your force is withdrawing - the battle ends in %d:%02d` | 01daa0 | CHANGED |

## steam — 20

| строка | RVA функции | статус функции |
|---|---|---|
| `steam:` | 012870, 01d33c, 02a130 | CHANGED/NEW |
| `steam: 5 s: %llu datagrams sent through Steam, %llu received%s` | 03fba0 | NEW |
| `steam: Steam did not take the "connect" text for its Join Game` | 03efa8 | NEW |
| `steam: Steam did not take the note for friends (asked for '%s', it holds '%s')` | 03efa8 | NEW |
| `steam: Steam is not running for this game - no Steam joining` | 03f75c | NEW |
| `steam: Steam's Join Game asks to join %s%s` | 03f5cc | NEW |
| `steam: Steam's callbacks arrive in this program` | 03e7a4 | NEW |
| `steam: TEST STAND-IN for Steam: this game is 'steam:%llu', its datagrams travel over UDP on this PC` | 03f75c | NEW |
| `steam: a join request that is not a multiplayer game's ('%.80s') - ignored` | 03f5cc | NEW |
| `steam: friends are told that this game can be joined (%s)` | 03efa8 | NEW |
| `steam: no Steam user - no Steam joining` | 03f75c | NEW |
| `steam: no connection to %llu could be made (error %u)` | 03e7cc | NEW |
| `steam: no longer shown to friends as a game to join%s` | 03efa8 | NEW |
| `steam: ready - user %llu (%s), application %u, %d friends, logged on: %s` | 03f75c | NEW |
| `steam: session asked for by %llu - accepted` | 03e804, 03e860 | NEW |
| `steam: switched off (MP_STEAM=0)` | 03f75c | NEW |
| `steam: test stand-in: UDP port %llu is taken` | 03f75c | NEW |
| `steam: the game has not loaded Steam's library - no Steam joining` | 03f75c | NEW |
| `steam: this game's Steam library has no %s - no Steam joining` | 03eb34 | NEW |
| `steam:%llu` | 02bb0c, 03f5cc | NEW |

## lobby — 17

| строка | RVA функции | статус функции |
|---|---|---|
| `lobby: %s is sending the force he picked (%u bytes)` | 01083c | NEW |
| `lobby: cannot keep the battle setup in %s, playing the battle as it is` | 018f20 | NEW |
| `lobby: cannot keep the other player's force in %s` | 010964 | NEW |
| `lobby: cannot rewrite the battle description, playing it as it is` | 018f20 | NEW |
| `lobby: could not restart the battle (%lu), playing it as it is` | 018f20 | NEW |
| `lobby: restarting the battle with the other player's own force` | 012008 | NEW |
| `lobby: the host's force costs %u - the other player's force may cost as much` | 01c33c | NEW |
| `lobby: the other player's force (%s) cannot be read - his side is played as set up` | 014054 | NEW |
| `lobby: the other player's force (%u squads of the %s) is not used: he commands the %s` | 010964 | NEW |
| `lobby: the other player's force arrived damaged, fetching it again` | 010964 | NEW |
| `lobby: the other player's force could not be put into the battle (%s) - his side is played as set up` | 014054 | NEW |
| `lobby: the other player's force is of the %s and he commands the %s - his side is played as set up` | 014054 | NEW |
| `lobby: the other player's own force arrived during deployment - restarting the battle with it` | 012edc | NEW |
| `lobby: the other player's own force arrived: %u squads, %u units of the %s - his side of the battle will be this force` | 010964 | NEW |
| `lobby: the other player's own force could not be put into this battle - his side is played as it is` | 012edc | NEW |
| `lobby: the other player's own force is dropped (%s)` | 017de4 | NEW |
| `lobby: the other player's own force is put into the battle: %s` | 014054 | NEW |

## end — 16

| строка | RVA функции | статус функции |
|---|---|---|
| `end: both players want the ceasefire - the battle ends in %g s` | 03e53c | NEW |
| `end: could not be hooked, each game ends its own battle` | 03dfc8 | NEW |
| `end: each game ends its own battle (MP_SYNC_END=0)` | 03dfc8 | NEW |
| `end: result %u here - the host's verdict%s` | 03d73c | NEW |
| `end: the battle has run out here - it ends when the host's game ends it` | 03d620 | NEW |
| `end: the battle is over here (result %u%s%s) - the other game is told to end its battle now` | 03d620 | NEW |
| `end: the host's battle is over (result %u there) - ending this one` | 03d280 | NEW |
| `end: the other player %s` | 03d280 | NEW |
| `end: the side %s %s, says its game` | 03d280 | NEW |
| `end: the side %s leaves the battle%s - the battle ends in %.0f s; the other game is told` | 03de68 | NEW |
| `end: the side %s leaves the battle%s, says its game - carried out here; the battle ends in %.0f s` | 03d280 | NEW |
| `end: this game's own numbers gave result %u, the host's give %u - the host's stand` | 03d73c | NEW |
| `end: this game's own view that the side %s should leave the battle is not acted on - that is for the game that commands it` | — | — |
| `end: this player %s` | 03e690 | NEW |
| `end: unknown game build, each game ends its own battle` | 03dfc8 | NEW |
| `end: withdrawing, a ceasefire and the end of the battle are shared between the two games (retreat time %.0f s, %.0f s of battle left)` | — | — |

## test — 16

| строка | RVA функции | статус функции |
|---|---|---|
| `test: Withdraw pressed in the ESC menu and confirmed (box 0)` | 014458 | NEW |
| `test: artillery - %d of this player's squads have off-map support, %d called it` | 0353b8 | NEW |
| `test: artillery - no divisions manager or no other units` | 0353b8 | NEW |
| `test: artillery - squad %08x (support %u, side %08x) called %d rounds of kind %d onto %.0f,%.0f: mission %d` | 0353b8 | NEW |
| `test: battle time %.2f s` | — | — |
| `test: blowup - the crew of %s will blow it up in 1 s` | 035638 | NEW |
| `test: blowup - this player has no vehicle` | 035638 | NEW |
| `test: flares - %d flares for squad %08x (side %08x) over %.0f,%.0f` | 035720 | NEW |
| `test: flares - no divisions manager or no other units` | 035720 | NEW |
| `test: flares - this player has no squad` | 035720 | NEW |
| `test: no trench manager to dig with` | 037d50 | NEW |
| `test: pause key` | 014458 | NEW |
| `test: speed level %d` | 014458 | NEW |
| `test: the Skirmish screen is not up (screen %d)` | 02bb0c | NEW |
| `test: the Skirmish screen's start` | 02bb0c | NEW |
| `test: the game's menu is %s` | 014458 | NEW |

## net — 14

| строка | RVA функции | статус функции |
|---|---|---|
| `net: %d computer-led formations on this player's side: the AI commander leads them, and only them` | 0229e8 | NEW |
| `net: %zu units on this player's side, %zu on the other side` | 013cf4 | CHANGED |
| `net: AI commander: %llu steps for formations of this side, %llu skipped for the other player's` | 019f88 | NEW |
| `net: UPnP: the router's forward of UDP %ld %s` | 022c84 | NEW |
| `net: joining through Steam (steam:%llu)` | 01d33c | CHANGED |
| `net: pause and speed are both players': the battle pauses or runs faster only when both ask for it%s` | 019f88 | NEW |
| `net: player connected through Steam (steam:%s, %s)` | 016230 | NEW |
| `net: the battle is over - back to the menu, not to the host's operation (%s)` | 00ffe4 | NEW |
| `net: the host is to be reached through Steam, and Steam is not there` | 01d33c | CHANGED |
| `net: the other game's units shown by its battle clock: transforms %.0f ms apart, its clock %+.0f ms from this game's and running x%.3f, arrivals wande` | — | — |
| `net: this battle belongs to the operation %s: %u files of what earlier battles left behind (%zu bytes) go to a game that joins` | — | — |
| `net: unknown game build, the computer-led formations on this player's side are not led` | 0229e8 | NEW |
| `net: unknown game build, the other player's units can be selected` | 012354 | CHANGED |
| `net: when this battle ends the game goes back to the menu (and from there to the host)` | 019f88 | NEW |

## map — 11

| строка | RVA функции | статус функции |
|---|---|---|
| `map: %u more of the host's battle groups have fought %s - identified for him from now on (%zu in all)` | 010f18 | NEW |
| `map: %u of the host's battle groups fight %s in this battle - identified for him from now on` | 01ede4 | NEW |
| `map: abilities pass - intelligence found out %u battle groups this turn, %u of them the host's (now known to the other player)` | — | — |
| `map: after the turn the host sees %u of the other player's %u battle groups, has identified %u (the game's own scouting)` | 0112e8 | CHANGED |
| `map: shared with %s: %s, %zu bytes, %u formations and battle groups turned round; he has identified %u of the host's groups` | — | — |
| `map: the operation's groups were not found, scouting not worked out` | 012ab8 | NEW |
| `map: the other player's intelligence found out %zu of the host's battle groups - %u newly identified for him` | 020004 | NEW |
| `map: this map is the host's operation, not this side's: scouting left as the host's copy has it` | 012ab8 | NEW |
| `map: this side's own scouting worked out: of the host's %u groups %u seen now (the copy said %u), %u identified` | 012ab8 | NEW |
| `map: unknown game build, scouting not worked out for this side` | 012ab8 | NEW |
| `map: unknown game build, what the other player's intelligence finds out is not kept for him` | 019f88 | NEW |

## ui — 11

| строка | RVA функции | статус функции |
|---|---|---|
| `ui: CHOOSE MY FORCE - Skirmish screen open; its start button hands this player's force to the host (%s)` | 02bb0c | NEW |
| `ui: CHOOSE MY FORCE - start refused: %s` | 029df8 | NEW |
| `ui: CHOOSE MY FORCE - the Skirmish screen is set to the host's year (%u) and season (%u), the %s against the %s` | 01bf54 | NEW |
| `ui: CHOOSE MY FORCE - the Skirmish screen's battle is not started: %s Back to %s's lobby.` | 02a47c | NEW |
| `ui: CHOOSE MY FORCE - the force costs %u of %u allowed` | 029df8 | NEW |
| `ui: Steam's Join Game - into the lobby of %s when the main menu is up` | 02bb0c | NEW |
| `ui: UPnP %s` | 026470 | NEW |
| `ui: click at %.0f, %.0f with the ESC menu open; CEASEFIRE is at %.0f..%.0f, %.0f..%.0f` | 024820 | NEW |
| `ui: hosting %s chosen, on UDP port %u, UPnP %s` | 02bb0c | NEW |
| `ui: joining %s through Steam ('%s')` | 02bb0c | NEW |
| `ui: this player's own force is offered to the host: %u squads, %u units of the %s` | 02815c | NEW |

## trench — 8

| строка | RVA функции | статус функции |
|---|---|---|
| `trench: of his digs %llu stamped a trench here as in his game (%llu of them where this game's own dig would not have), %llu were kept from stamping on` | — | — |
| `trench: outside any dig: %llu stamped here told to the other game, %llu of the other game's stamped here, %llu of them this game had already; %llu of ` | — | — |
| `trench: squad %08x dug in (kind %u, settings %x %u %u %u, tile %u cell %u direction %u, %s): %s%s` | 036d38 | NEW |
| `trench: squad %08x of the other player dug in here from his values (kind %u, settings %x, tile %u cell %u direction %u): his game %s, this game's dig ` | — | — |
| `trench: stamped from the other game's list: tile %u cell %u kind %u direction %u variant %d flags %x` | 0378e8 | NEW |
| `trench: stamped outside a dig: tile %u cell %u kind %u direction %u variant %d flags %x squad %s (%s), asked for by adv_game+0x%llx <- +0x%llx` | — | — |
| `trench: the other game is told of %zu trenches stamped here outside any dig%s` | 0378e8 | NEW |
| `trench: this game's own stamp left out, made on the other game's word already: tile %u cell %u kind %u direction %u variant %d flags %x` | — | — |

## hit — 6

| строка | RVA функции | статус функции |
|---|---|---|
| `hit: %s` | 03cedc | NEW |
| `hit: %s struck (kind %u, part %x, ammunition %u), sent` | 03c99c | NEW |
| `hit: %s struck in its owner's game (kind %u, part %x), carried out here` | 03c650 | NEW |
| `hit: 5 s: %lld on this game's units worked out here and sent, %lld of the other game's carried out here, %lld of this game's own on the other player's` | — | — |
| `hit: each game works out every hit for itself (MP_SYNC_HITS=0)` | 03cedc | NEW |
| `hit: unknown game build, each game works out every hit for itself` | 03cedc | NEW |

## clock — 5

| строка | RVA функции | статус функции |
|---|---|---|
| `clock: the engine's automatic speed control is %s; off while the other player is in the battle` | 020190 | CHANGED |
| `clock: the other player %s` | 016230 | NEW |
| `clock: the other player asks for speed x%.2f` | 016230 | NEW |
| `clock: this player %s` | 01e530 | NEW |
| `clock: this player asks for speed x%.2f` | 01e770 | NEW |

## anim — 4

| строка | RVA функции | статус функции |
|---|---|---|
| `anim: 5 s: %llu soldiers' animation values sent, %llu received, %llu of this game's own orders for the other player's soldiers held back, %llu groups ` | — | — |
| `anim: each game animates every soldier for itself (MP_SYNC_ANIM=0)` | 03be00 | NEW |
| `anim: unknown game build, each game animates every soldier for itself` | 03be00 | NEW |
| `anim: what a soldier's animation is asked to do is its owner's: sent, and followed here` | 03be00 | NEW |

## crater — 3

| строка | RVA функции | статус функции |
|---|---|---|
| `crater: %llu made here as the host, %llu of the host's made here (%llu more this game had made itself already), %llu of this game's own dropped, %llu ` | — | — |
| `crater: %s` | 037f98 | NEW |
| `crater: kind %d at %.1f %.1f %.1f, flags %x, size %.2f, vector %.2f %.2f %.2f %.2f%s` | 0364dc | NEW |

## places — 2

| строка | RVA функции | статус функции |
|---|---|---|
| `places: %llu added from %s <- %s <- %s` | 0385bc | NEW |
| `places: %s` | 037f98 | NEW |

## shots — 2

| строка | RVA функции | статус функции |
|---|---|---|
| `shots: %s%s; explosions %s (vehicles blown up by their crews: %s), flares %s; off-map fire goes by the side it is fired for` | — | — |
| `shots: 5 s: %llu of this game's launches were off-map fire; explosions: %llu made here and sent, %llu received, %llu local ones dropped; flares: %llu ` | — | — |

## select — 1

| строка | RVA функции | статус функции |
|---|---|---|
| `select: 5 s: %llu of the other player's units kept out of this player's selection` | 019f88 | NEW |

## state — 1

| строка | RVA функции | статус функции |
|---|---|---|
| `state: 5 s: %llu states came from the other game's handle numbering: %llu handles rewritten to this game's` | 02f6dc | NEW |

## trace — 1

| строка | RVA функции | статус функции |
|---|---|---|
| `trace: battle %.3f shown %.3f real %.4f  by battle time %.3f %.3f  by arrival %.3f %.3f` | 01c544 | NEW |


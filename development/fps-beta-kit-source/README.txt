MiusMultiplayer beta_FPS_fix
============================

EXPERIMENTAL TWO-PLAYER FPS TEST ONLY. This is a second, public installer archive;
it does not replace MiusMultiplayer_v1.0.0_INSTALL.zip.

Target game: final Steam version 6.00 build 3598/1.

Steam friends regression repair
-------------------------------
The earlier beta replaced basic_network.codelib with a public-source rebuild.
That public source does not include the released module's Steam friends,
rich-presence Join Game callback, or SteamNetworking P2P implementation. As a
result, Steam friends could not join a beta host. This archive must not be used
with that old candidate:

  78e2718383bc0679154118415189d1c1ee05ed0179a0a924455c22cb4ae2916e

The replacement candidate starts with the verified Steam-capable 3598/1 module
and applies one deterministic, reviewed FPS change: on_ctrl_tick does not call
session_sim_step when the engine reports a zero-millisecond simulation step. A
23-byte .fpsfix PE trampoline takes every non-zero step through the original
function unchanged. It is a real code change, not the unchanged stable binary.

The candidate retains the Steam runtime paths for MP_STEAM_PEER, rich presence,
Steam friends Join Game, SteamFriends/SteamUser/SteamNetworking, SendP2PPacket,
ReadP2PPacket, session acceptance, and JoinRequested. The package verifier
rejects a module that loses any of these capability markers.

Candidate SHA-256:

  7b42a69abe58deb1583d2f5320eebbecd9718ffa27ea08933613e67df1901f67

What this beta changes
----------------------
It automatically replaces only:

  bin\rel\x64\basic_network.codelib

It backs up any previously installed codelib, including an unknown module, in
MiusMultiplayer_beta_FPS_fix\state\backup before replacement. The backup receives
its own SHA-256 record; Disable_Beta_FPS_Fix.bat verifies and restores that exact
previous file. If a stale beta backup exists while a different external codelib is
active, the installer stops without changing either file so data cannot be lost.

Install
-------
1. If the old candidate above is active, close the game and run this archive's
   Disable_Beta_FPS_Fix.bat first. It recognizes and restores the saved pre-beta
   module for the withdrawn candidate too. Do this on both PCs with the game closed.
2. Extract this whole MiusMultiplayer_beta_FPS_fix folder next to starter_x64.exe.
3. Close the game on BOTH PCs.
4. Run Check_Beta_FPS_Fix.bat, then Enable_Beta_FPS_Fix.bat on BOTH PCs.
5. Both players must use this exact same beta archive.
6. Run the Steam regression before FPS comparison: host through Steam rich
   presence, join with the friend's Steam Join Game action, reach the P2P lobby,
   start a tactical session, leave, then repeat with the other player hosting.
7. For the FPS comparison run the same tactical operation once with each player
   hosting. Preserve both basic_network logs.
8. Run Disable_Beta_FPS_Fix.bat on BOTH PCs before returning to stable
   MiusMultiplayer v1.0.0 or using its installer/remover.

Critical beta boundary
----------------------
This codelib has deterministic PE/source validation but has NOT had a Windows
Steam two-peer gameplay certification. It is an FPS test candidate, not a
compatibility release. The Steam implementation is preserved from the verified
release module; it was not reconstructed from incomplete public source.

The released mp_vitals.dll import/signature surface remains, but this is not
proof that Vitals health synchronization or renderer guards work in this beta.
Test connection, Steam friends joining, reconnect, tactical movement, battle
exit, FPS, and restoration. Do not call any of those gameplay-certified until
both-PC evidence has been recorded.

The stable unified archive remains:

  MiusMultiplayer_v1.0.0_INSTALL.zip

Use that stable archive for normal MiusPlayer + Vitals play.

Build provenance
----------------
The codelib is built deterministically by
scripts/build_steam_compatible_fps_candidate.py from the exact verified
Steam-capable 3598/1 module SHA-256:

  485b655662bd062e16278f9d699a3097b49a08de1948d2500dd589703a3102a3

The verifier proves the expected PE transformation, the original ctrl_tick call
signature, the injected .fpsfix code, the retained Vitals import, and every
listed Steam capability marker. The older VS2022 public-source workflow is not
an acceptable release source because that source omits the Steam implementation.

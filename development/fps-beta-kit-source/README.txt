MiusMultiplayer beta_FPS_fix
============================

EXPERIMENTAL TWO-PLAYER FPS TEST ONLY. This is a second, public installer archive;
it does not replace MiusMultiplayer_v1.0.0_INSTALL.zip.

Target game: final Steam version 6.00 build 3598/1.

What this beta changes
----------------------
It automatically replaces only:

  bin\rel\x64\basic_network.codelib

with a reproducible Windows VS2022 source build containing the remote-transform
write-elision candidate. The candidate SHA-256 is:

  78e2718383bc0679154118415189d1c1ee05ed0179a0a924455c22cb4ae2916e

It keeps a private backup of the previously installed recognised codelib in
MiusMultiplayer_beta_FPS_fix\state\backup and Disable_Beta_FPS_Fix.bat restores it.
The beta installer never overwrites an unknown active multiplayer module.

Install
-------
1. Extract this whole MiusMultiplayer_beta_FPS_fix folder next to starter_x64.exe.
2. Close the game on BOTH PCs.
3. Run Check_Beta_FPS_Fix.bat, then Enable_Beta_FPS_Fix.bat on BOTH PCs.
4. Both players must use this exact same beta archive.
5. For the comparison run the same tactical operation once with each player hosting.
6. Run Disable_Beta_FPS_Fix.bat on BOTH PCs before returning to stable
   MiusMultiplayer v1.0.0 or using its installer/remover.

Critical beta boundary
----------------------
This codelib was reproducibly compiled, but it has NOT had a Windows two-peer
gameplay certification. It is a FPS test candidate, not a compatibility release.
It is not the released v20 binary and does not carry the released mp_vitals.dll
import/RVA profile. Therefore this beta must not be used to validate Vitals health
synchronization or renderer guards; test FPS, tactical movement, connection,
reconnect, battle exit and restoration only.

The stable unified archive remains:

  MiusMultiplayer_v1.0.0_INSTALL.zip

Use that stable archive for normal MiusPlayer + Vitals play.

Build provenance
----------------
The codelib is built twice in GitHub Actions Windows 2022 with VS2022 x64 and
/Brepro; both outputs must compare byte-for-byte before publication. The build
workflow is .github/workflows/build-fps-beta-codelib.yml. The exact source patch
is development/merged-source/patches/0001-elide-idempotent-remote-transform-writes.patch.

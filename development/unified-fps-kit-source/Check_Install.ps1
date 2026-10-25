# MiusMultiplayer v1.0.1: read-only install and MULTIPLAYER-menu load report.
# Nothing in this file changes, replaces, refuses or repairs a game file. It only reports.
$ErrorActionPreference = 'SilentlyContinue'

$mod = Split-Path -Parent $MyInvocation.MyCommand.Path
$game = $null
$probe = $mod
for ($i = 0; $i -lt 3; $i++) {
    $probe = Split-Path -Parent $probe
    if (-not $probe) { break }
    if (Test-Path -LiteralPath (Join-Path $probe 'starter_x64.exe') -PathType Leaf) { $game = $probe; break }
}

$netHash = '7B42A69ABE58DEB1583D2F5320EEBBECD9718FFA27EA08933613E67DF1901F67'
$vitalsHash = '71A1BF3A37923BBE6C8EE84448D0B91705C2BFF7382B647F990B9D671EE224DF'
$joinHash = '6AA417CF6E1F01480C997446F2691A58AB55B90FA36E1FF511945989AADFEEAA'
$opgameHash = 'ECA75B740F7B11D9E13F2E2001F1B2B133427D0860FB3770220B89B0B8909535'
$tacgameHash = '01923EABDF6A9D5FA37DC70A3D76740242DBC9A7F26C0298CA3FAA8CCA87600D'
$appidHash = '1A43840CA36F1401C85FF5F6E6981088927CFE09B23D155A9C4301DD13CA9610'

$kitFiles = [ordered]@{
    'files\bin\rel\x64\basic_network.codelib' = $netHash
    'files\mp_vitals.dll' = $vitalsHash
    'files\mp_join.exe' = $joinHash
    'files\root\programs\opgame.progpack' = $opgameHash
    'files\root\programs\tacgame.progpack' = $tacgameHash
    'files\steam_appid.txt' = $appidHash
}
$runtimeFiles = [ordered]@{
    'bin\rel\x64\basic_network.codelib' = $netHash
    'mp_vitals.dll' = $vitalsHash
    'mp_join.exe' = $joinHash
    'root\programs\opgame.progpack' = $opgameHash
    'root\programs\tacgame.progpack' = $tacgameHash
    'steam_appid.txt' = $appidHash
}

$lines = New-Object 'System.Collections.Generic.List[string]'
function Add-Line([string]$text) { [void]$lines.Add($text) }

function Get-Sha256([string]$path) {
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { return '' }
    try { return (Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash.ToUpper() } catch { return '' }
}

function Get-FileText([string]$path) {
    try { return [System.Text.Encoding]::ASCII.GetString([System.IO.File]::ReadAllBytes($path)) } catch { return '' }
}

# Import table of a PE file. The multiplayer module is a DLL loaded by the engine, and the engine
# silently does nothing when one of its imports cannot be resolved - there is then no MULTIPLAYER
# entry at all. This is the report that names the unresolved name.
function Get-PeInfo([string]$path) {
    $info = [ordered]@{ Machine = 0; Imports = @(); Error = '' }
    try {
        $b = [System.IO.File]::ReadAllBytes($path)
    } catch {
        $info.Error = 'unreadable'
        return $info
    }
    try {
        if ($b.Length -lt 0x100 -or $b[0] -ne 0x4D -or $b[1] -ne 0x5A) { $info.Error = 'not a PE file'; return $info }
        $pe = [System.BitConverter]::ToInt32($b, 0x3C)
        if ($b[$pe] -ne 0x50 -or $b[$pe + 1] -ne 0x45) { $info.Error = 'no PE signature'; return $info }
        $info.Machine = [System.BitConverter]::ToUInt16($b, $pe + 4)
        $count = [System.BitConverter]::ToUInt16($b, $pe + 6)
        $optSize = [System.BitConverter]::ToUInt16($b, $pe + 20)
        $opt = $pe + 24
        $magic = [System.BitConverter]::ToUInt16($b, $opt)
        if ($magic -eq 0x20B) { $dirs = $opt + 112 } else { $dirs = $opt + 96 }
        $table = $opt + $optSize
        $map = New-Object 'System.Collections.ArrayList'
        for ($i = 0; $i -lt $count; $i++) {
            $at = $table + $i * 40
            $vsize = [System.BitConverter]::ToUInt32($b, $at + 8)
            $va = [System.BitConverter]::ToUInt32($b, $at + 12)
            $rawSize = [System.BitConverter]::ToUInt32($b, $at + 16)
            $raw = [System.BitConverter]::ToUInt32($b, $at + 20)
            [void]$map.Add(@($va, [Math]::Max($vsize, $rawSize), $raw))
        }
        $rvaToOffset = {
            param([uint32]$rva)
            foreach ($s in $map) {
                if ($rva -ge $s[0] -and $rva -lt ($s[0] + $s[1])) { return $s[2] + $rva - $s[0] }
            }
            return -1
        }
        $impRva = [System.BitConverter]::ToUInt32($b, $dirs + 8)
        if ($impRva -eq 0) { return $info }
        $at = & $rvaToOffset $impRva
        while ($at -ge 0 -and ($at + 20) -le $b.Length) {
            $nameRva = [System.BitConverter]::ToUInt32($b, $at + 12)
            if ($nameRva -eq 0) { break }
            $atName = & $rvaToOffset $nameRva
            if ($atName -lt 0) { break }
            $end = $atName
            while ($end -lt $b.Length -and $b[$end] -ne 0) { $end++ }
            if ($end -gt $atName) {
                $name = [System.Text.Encoding]::ASCII.GetString($b, $atName, $end - $atName)
                if ($name) { $info.Imports += $name }
            }
            $at += 20
        }
    } catch {
        $info.Error = 'malformed PE'
    }
    return $info
}

Add-Line 'MiusMultiplayer v1.0.1 - install and MULTIPLAYER-menu load report'
Add-Line ("Started: {0}" -f (Get-Date -Format 'yyyy-MM-dd HH:mm:ss'))
Add-Line "Kit:  $mod"
if (-not $game) {
    Add-Line 'Game: NOT FOUND - this MiusMultiplayer folder is not inside the game folder.'
    Add-Line ''
    Add-Line 'ERROR: put the whole MiusMultiplayer folder next to starter_x64.exe and run this again.'
    $lines | ForEach-Object { Write-Host $_ }
    exit 3
}
Add-Line "Game: $game"
Add-Line ''

Add-Line 'Kit files (informational, nothing is refused or repaired):'
foreach ($entry in $kitFiles.GetEnumerator()) {
    $path = Join-Path $mod $entry.Key
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { Add-Line "  MISSING  $($entry.Key)"; continue }
    $hash = Get-Sha256 $path
    $state = if ($hash -eq $entry.Value) { 'OK      ' } else { 'WRONG   ' }
    Add-Line ("  {0}{1}  {2}" -f $state, $entry.Key, $hash)
}
Add-Line ''

Add-Line 'Installed runtime files (DIFFERENT/MISSING only tells you what to rerun Enable_Multiplayer.bat for):'
foreach ($entry in $runtimeFiles.GetEnumerator()) {
    $path = Join-Path $game $entry.Key
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { Add-Line "  MISSING  $($entry.Key)"; continue }
    $hash = Get-Sha256 $path
    $state = if ($hash -eq $entry.Value) { 'OK       ' } else { 'DIFFERENT' }
    Add-Line ("  {0}  {1}  {2}" -f $state, $entry.Key, $hash)
}
Add-Line ''

# ---------------------------------------------------------------- the MULTIPLAYER entry preconditions
Add-Line 'MULTIPLAYER entry in the main menu - load preconditions:'
$module = Join-Path $game 'bin\rel\x64\basic_network.codelib'
$dll = Join-Path $game 'mp_vitals.dll'
$opgame = Join-Path $game 'root\programs\opgame.progpack'
$tacgame = Join-Path $game 'root\programs\tacgame.progpack'

if (-not (Test-Path -LiteralPath $module -PathType Leaf)) {
    Add-Line '  module: bin\rel\x64\basic_network.codelib is MISSING - the engine has nothing to load.'
} else {
    $pe = Get-PeInfo $module
    if ($pe.Error) {
        Add-Line ("  module: unreadable PE ({0})" -f $pe.Error)
    } else {
        $arch = if ($pe.Machine -eq 0x8664) { 'x64' } else { ('machine 0x{0:X4}' -f $pe.Machine) }
        Add-Line ("  module: {0}, imports {1}" -f $arch, ($pe.Imports -join ', '))
        if ($pe.Imports -notcontains 'mp_vitals.dll') {
            Add-Line '  module: no mp_vitals.dll import (unexpected for this kit)'
        }
        $missing = New-Object 'System.Collections.Generic.List[string]'
        foreach ($name in $pe.Imports) {
            if ($name -like 'api-ms-win-*') { continue }
            if (Test-Path -LiteralPath (Join-Path $game $name) -PathType Leaf) { continue }
            if (Test-Path -LiteralPath (Join-Path $env:SystemRoot ('System32\' + $name)) -PathType Leaf) { continue }
            [void]$missing.Add($name)
        }
        if ($missing.Count) {
            Add-Line ("  module: UNRESOLVED IMPORT(S): {0} - Windows then does not load the module at all." -f ($missing -join ', '))
            Add-Line '          Put the missing DLL into the game folder (next to starter_x64.exe) or install the missing runtime.'
        } else {
            Add-Line '  module: every imported DLL resolves (game folder or System32).'
        }
    }
}
if (Test-Path -LiteralPath $dll -PathType Leaf) {
    Add-Line ("  mp_vitals.dll in the game folder: present, {0}" -f (Get-Sha256 $dll))
} else {
    Add-Line '  mp_vitals.dll in the game folder: MISSING - basic_network.codelib imports it, so the module cannot load.'
}

$crt = @('vcruntime140.dll', 'vcruntime140_1.dll', 'msvcp140.dll')
$crtMissing = @()
foreach ($name in $crt) {
    if (-not (Test-Path -LiteralPath (Join-Path $env:SystemRoot ('System32\' + $name)) -PathType Leaf)) { $crtMissing += $name }
}
if ($crtMissing.Count) {
    Add-Line ("  Visual C++ 2015-2022 x64 runtime: MISSING {0}" -f ($crtMissing -join ', '))
    Add-Line '          Install the Microsoft Visual C++ 2015-2022 Redistributable (x64), then start the game again.'
} else {
    Add-Line '  Visual C++ 2015-2022 x64 runtime: present.'
}

foreach ($pack in @(@('root\programs\opgame.progpack', $opgame, $opgameHash), @('root\programs\tacgame.progpack', $tacgame, $tacgameHash))) {
    if (-not (Test-Path -LiteralPath $pack[1] -PathType Leaf)) {
        Add-Line ("  {0}: MISSING - the stock packed program still loads basic_network.codelib but never calls its" -f $pack[0])
        Add-Line '          render command, so the module logs and NO MULTIPLAYER entry is drawn. Rerun Enable_Multiplayer.bat.'
        continue
    }
    $text = Get-FileText $pack[1]
    $packHash = Get-Sha256 $pack[1]
    if (-not ($text.Contains('basic_network.codelib') -and $text.Contains('i_network'))) {
        Add-Line ("  {0}: does not name basic_network.codelib/i_network - it cannot create the multiplayer object." -f $pack[0])
    } elseif ($packHash -ne $pack[2]) {
        Add-Line ("  {0}: foreign pack (SHA-256 {1})." -f $pack[0], $packHash)
        Add-Line '          If it is a stock/other-pack program, the module loads but its render call is absent: no MULTIPLAYER entry.'
    } else {
        Add-Line ("  {0}: this kit's pack - requests basic_network.codelib, i_network and the render call." -f $pack[0])
    }
}

$motw = $false
foreach ($path in @($module, $dll, $opgame, $tacgame)) {
    try {
        if (Get-Item -LiteralPath $path -Stream Zone.Identifier -ErrorAction Stop) { $motw = $true }
    } catch { }
}
if ($motw) {
    Add-Line '  internet mark (Mark of the Web): PRESENT on an installed module - SmartScreen or Smart App Control can refuse to load it.'
    Add-Line '          Run Enable_Multiplayer.bat again (it unblocks the installed files) or right-click the file, Properties, Unblock.'
} else {
    Add-Line '  internet mark (Mark of the Web): none on the installed modules.'
}

$sac = (Get-ItemProperty -Path 'HKLM:\SYSTEM\CurrentControlSet\Control\CI\Policy' -Name 'VerifiedAndReputablePolicyState').VerifiedAndReputablePolicyState
switch ($sac) {
    1 { Add-Line '  Smart App Control: ON - it can block an unsigned module; if it does, the game has no MULTIPLAYER entry and no log.' }
    2 { Add-Line '  Smart App Control: evaluation mode.' }
    default { Add-Line '  Smart App Control: off.' }
}

$outDir = Join-Path $game 'out'
if (-not (Test-Path -LiteralPath $outDir -PathType Container)) {
    Add-Line '  logs: no out folder yet - the module writes basic_network_*.log there; Enable_Multiplayer.bat creates it.'
} else {
    $logs = @(Get-ChildItem -LiteralPath $outDir -Filter 'basic_network_*.log' -File -ErrorAction SilentlyContinue)
    if ($logs.Count -eq 0) {
        Add-Line '  logs: none yet. AFTER starting the game, no log means Windows never loaded basic_network.codelib;'
        Add-Line '        the module/import/runtime lines above then name the reason.'
    } else {
        $newest = $logs | Sort-Object LastWriteTime -Descending | Select-Object -First 1
        Add-Line ("  logs: {0} file(s); newest {1} ({2})" -f $logs.Count, $newest.Name, $newest.LastWriteTime.ToString('yyyy-MM-dd HH:mm:ss'))
        $text = Get-FileText $newest.FullName
        if ($text.Contains('basic_network.codelib loaded')) { Add-Line '         module loaded marker: present' } else { Add-Line '         module loaded marker: absent' }
        if ($text.Contains('ui: Direct3D device')) { Add-Line '         menu draw marker ("ui: Direct3D device"): present - the entry was drawn' }
        elseif ($text.Contains('ui: no i_render object')) { Add-Line '         menu draw marker: no i_render object - the engine had no render object to draw through' }
        elseif ($text.Contains('ui: no Direct3D device')) { Add-Line '         menu draw marker: no Direct3D device found in i_render' }
        else { Add-Line '         menu draw marker: absent - the module was loaded but the main menu was not recognised (game build other than 3598/1)' }
        if ($text.Contains('vitals:')) { Add-Line '         vitals: lines present' } else { Add-Line '         vitals: no line in this log yet (normal outside a session)' }
    }
}
Add-Line ''

Add-Line 'Historical Steam 3598/1 fingerprint (diagnostic only; differences never block install or replacement):'
$list = Join-Path $mod 'game_build_3598_1.sha256'
if (-not (Test-Path -LiteralPath $list -PathType Leaf)) {
    Add-Line '  reference list absent; nothing compared.'
} else {
    $total = 0
    $different = 0
    foreach ($raw in Get-Content -LiteralPath $list) {
        if (-not $raw -or $raw.Trim().Length -eq 0) { continue }
        $parts = $raw -split '\s+', 2
        if ($parts.Count -ne 2) { continue }
        $total++
        $relative = $parts[1].Trim() -replace '\\\\', '\'
        $path = Join-Path $game $relative
        if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { $different++; continue }
        if ((Get-Sha256 $path) -ne $parts[0].ToUpper()) { $different++ }
    }
    Add-Line ("  {0} of {1} listed game files differ from the historical reference." -f $different, $total)
    Add-Line '  That is informational: the installer replaces its files on any build, and every engine-sensitive'
    Add-Line '  hook inside the module is enabled only after its own exact runtime signature check.'
}

$report = Join-Path $mod 'build_report.txt'
$lines | Set-Content -LiteralPath $report -Encoding UTF8
$lines | ForEach-Object { Write-Host $_ }
Write-Host ""
Write-Host "Report saved to: $report"
exit 0

# GTMP_alpha: build ONE report file (MiusMultiplayer\GTMP_alpha_log.txt) that is enough to
# diagnose a two-PC test without asking the tester for anything else.
#
# It never changes the game folder: it only reads files and hashes them.
param(
    [string]$Mod = ''
)

if (-not $Mod) { $Mod = Split-Path -Parent $PSCommandPath }
$ErrorActionPreference = 'Continue'
$report = New-Object 'System.Collections.Generic.List[string]'
function Add-Line([string]$text = '') { [void]$report.Add($text) }

$mod  = (Resolve-Path -LiteralPath $Mod).Path
$game = Split-Path -Parent $mod
$out  = Join-Path $game 'out'
$dest = Join-Path $mod 'GTMP_alpha_log.txt'

# Files this alpha archive installs, and what they must hash to.
$expected = [ordered]@{
    'bin\rel\x64\basic_network.codelib' = '7B42A69ABE58DEB1583D2F5320EEBBECD9718FFA27EA08933613E67DF1901F67'
    'mp_vitals.dll'                     = '808E1D5F6CB301AC971700E0ABA6E78D1DF1B13B02E748B8D01DD4BFC8A9881C'
    'mp_join.exe'                       = '6AA417CF6E1F01480C997446F2691A58AB55B90FA36E1FF511945989AADFEEAA'
    'root\programs\opgame.progpack'     = 'ECA75B740F7B11D9E13F2E2001F1B2B133427D0860FB3770220B89B0B8909535'
    'root\programs\tacgame.progpack'    = '01923EABDF6A9D5FA37DC70A3D76740242DBC9A7F26C0298CA3FAA8CCA87600D'
    'steam_appid.txt'                   = '1A43840CA36F1401C85FF5F6E6981088927CFE09B23D155A9C4301DD13CA9610'
}

function Get-Sha256([string]$path) {
    try { return (Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash.ToUpper() } catch { return '' }
}

function Get-FileText([string]$path) {
    try { return (Get-Content -LiteralPath $path -Raw) } catch { return '' }
}

function Add-Section([string]$title) {
    Add-Line ''
    Add-Line ('=' * 78)
    Add-Line $title
    Add-Line ('=' * 78)
}

# ---------------------------------------------------------------- hashes and verdicts first
$hashes = [ordered]@{}
foreach ($relative in $expected.Keys) {
    $path = Join-Path $game $relative
    if (Test-Path -LiteralPath $path -PathType Leaf) {
        $hashes[$relative] = Get-Sha256 $path
    } else {
        $hashes[$relative] = ''
    }
}

$logs = @()
$logBytes = 0
if (Test-Path -LiteralPath $out -PathType Container) {
    $logs = @(Get-ChildItem -LiteralPath $out -File -Filter 'basic_network_*.log' -ErrorAction SilentlyContinue | Sort-Object LastWriteTime)
}
$logText = @{}
foreach ($log in $logs) { $logText[$log.FullName] = Get-FileText $log.FullName }

$allText = ($logText.Values -join "`r`n")
$allLines = @($allText -split "`r?`n")
foreach ($log in $logs) { $logBytes += $log.Length }
$banner = [regex]::Matches($allText, 'vitals: GTMP_alpha-1[^\r\n]*')
$sessions = [regex]::Matches($allText, 'vitals: on \(mode')
$exchange = [regex]::Matches($allText, 'object-number exchange complete')
$noTraffic = [regex]::Matches($allText, 'rx: no datagrams')
$errors = @($allLines | Where-Object { $_ -match '(?i)could not|failed|lost the connection|connection lost|not installed|not armed|not recognised|crash|exception' })

$dllOk = ($hashes['mp_vitals.dll'] -eq $expected['mp_vitals.dll'])
$codeOk = ($hashes['bin\rel\x64\basic_network.codelib'] -eq $expected['bin\rel\x64\basic_network.codelib'])

Add-Line 'GTMP_alpha - one-file report for a two-PC test'
Add-Line ('Generated: {0:yyyy-MM-dd HH:mm:ss}  on {1}' -f (Get-Date), $env:COMPUTERNAME)
Add-Line ''
Add-Line 'QUICK VERDICT'
Add-Line ('  alpha DLL loaded (banner in a log):  {0}' -f $(if ($banner.Count) { 'YES' } else { 'not found' }))
Add-Line ('  mp_vitals.dll hash:                  {0}' -f $(if ($dllOk) { 'OK (this is the GTMP_alpha DLL)' } elseif ($hashes['mp_vitals.dll']) { 'MISMATCH: ' + $hashes['mp_vitals.dll'] } else { 'MISSING' }))
Add-Line ('  basic_network.codelib hash:          {0}' -f $(if ($codeOk) { 'OK (stable v1.0.1 module)' } elseif ($hashes['bin\rel\x64\basic_network.codelib']) { 'MISMATCH: ' + $hashes['bin\rel\x64\basic_network.codelib'] } else { 'MISSING' }))
Add-Line ('  logs in out\:                        {0} file(s), {1} bytes' -f $logs.Count, $logBytes)
Add-Line ('  Vitals sessions in the logs:         {0}' -f $sessions.Count)
Add-Line ('  object-number exchange completed:    {0}' -f $exchange.Count)
Add-Line ('  "no datagrams" warnings:             {0}' -f $noTraffic.Count)
Add-Line ('  error-like lines:                    {0}' -f $errors.Count)
$stateLines = @($allLines | Where-Object { $_ -match 'state: ' }).Count
Add-Line ('  "state:" saved-state lines:          {0}' -f $stateLines)
if ($banner.Count) {
    Add-Line ''
    Add-Line '  build banner seen:'
    $banner | Select-Object -First 3 | ForEach-Object { Add-Line ('    ' + $_.Value) }
}
if (-not $sessions.Count) {
    Add-Line ''
    Add-Line '  NOTE: no Vitals session was started. Either no multiplayer game was played, or the module/DLL did not load.'
}

# ---------------------------------------------------------------- system
Add-Section 'SYSTEM'
try {
    $os = Get-CimInstance -ClassName Win32_OperatingSystem -ErrorAction Stop
    Add-Line ('OS:        {0} {1} (build {2}, {3})' -f $os.Caption, $os.Version, $os.BuildNumber, $os.OSArchitecture)
    Add-Line ('Memory:    {0:N0} MB total, {1:N0} MB free' -f ($os.TotalVisibleMemorySize / 1024), ($os.FreePhysicalMemory / 1024))
    Add-Line ('Booted:    {0}' -f $os.LastBootUpTime)
} catch { Add-Line ('OS:        unavailable (' + $_.Exception.Message + ')') }
try {
    $cpu = Get-CimInstance -ClassName Win32_Processor -ErrorAction Stop | Select-Object -First 1
    Add-Line ('CPU:       {0} ({1} cores)' -f $cpu.Name.Trim(), $cpu.NumberOfCores)
} catch { }
try {
    foreach ($gpu in (Get-CimInstance -ClassName Win32_VideoController -ErrorAction Stop)) {
        Add-Line ('GPU:       {0} driver {1}' -f $gpu.Name, $gpu.DriverVersion)
    }
} catch { }
try {
    Add-Line ('PowerShell:{0}' -f $PSVersionTable.PSVersion)
} catch { }

# ---------------------------------------------------------------- game folder
Add-Section 'GAME FOLDER'
Add-Line ('Game:      ' + $game)
Add-Line ('Mod:       ' + $mod)
$starter = Join-Path $game 'starter_x64.exe'
if (Test-Path -LiteralPath $starter -PathType Leaf) {
    try {
        $vi = (Get-Item -LiteralPath $starter).VersionInfo
        Add-Line ('starter_x64.exe: {0} / {1}' -f $vi.FileVersion, $vi.ProductVersion)
        Add-Line ('starter_x64.exe SHA-256: ' + (Get-Sha256 $starter))
    } catch { Add-Line 'starter_x64.exe: version unavailable' }
} else {
    Add-Line 'starter_x64.exe: NOT FOUND - this folder is not the game folder'
}
$appid = Join-Path $game 'steam_appid.txt'
if (Test-Path -LiteralPath $appid -PathType Leaf) { Add-Line ('steam_appid.txt: ' + (Get-FileText $appid).Trim()) }
$steamDll = Join-Path $game 'steam_api64.dll'
if (Test-Path -LiteralPath $steamDll -PathType Leaf) {
    try { Add-Line ('steam_api64.dll: {0}' -f ((Get-Item -LiteralPath $steamDll).VersionInfo.FileVersion)) } catch { Add-Line 'steam_api64.dll: present' }
} else {
    Add-Line 'steam_api64.dll: not in the game folder (the game may carry it elsewhere)'
}
$vc = Join-Path $env:SystemRoot 'System32\vcruntime140.dll'
if (Test-Path -LiteralPath $vc -PathType Leaf) { Add-Line 'vcruntime140.dll: present' } else { Add-Line 'vcruntime140.dll: MISSING' }

# ---------------------------------------------------------------- installed files
Add-Section 'INSTALLED FILES (hash compared with the GTMP_alpha archive)'
foreach ($relative in $expected.Keys) {
    $path = Join-Path $game $relative
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) {
        Add-Line ('  MISSING  {0}' -f $relative)
        continue
    }
    $state = 'DIFFERENT'
    if ($hashes[$relative] -eq $expected[$relative]) { $state = 'OK       ' }
    Add-Line ('  {0}  {1}  {2}' -f $state, $relative, $hashes[$relative])
}
$installed = Join-Path $mod 'state\installed.txt'
if (Test-Path -LiteralPath $installed -PathType Leaf) {
    Add-Line ''
    Add-Line 'state\installed.txt:'
    Get-Content -LiteralPath $installed | ForEach-Object { Add-Line ('  ' + $_) }
}
$installedState = Join-Path $mod 'state\enabled.txt'
if (Test-Path -LiteralPath $installedState -PathType Leaf) {
    Add-Line ('state\enabled.txt: ' + (Get-FileText $installedState).Trim())
}

# ---------------------------------------------------------------- environment overrides
Add-Section 'ENVIRONMENT OVERRIDES (MP_*)'
$anyEnv = $false
foreach ($scope in @('Process', 'User', 'Machine')) {
    try { $vars = [Environment]::GetEnvironmentVariables($scope) } catch { continue }
    foreach ($name in ($vars.Keys | Sort-Object)) {
        if ($name -notlike 'MP_*') { continue }
        Add-Line ('  [{0}] {1}={2}' -f $scope, $name, $vars[$name])
        $anyEnv = $true
    }
}
if (-not $anyEnv) { Add-Line '  none set (the game will use the built-in defaults)' }

# ---------------------------------------------------------------- install report
$buildReport = Join-Path $mod 'build_report.txt'
if (Test-Path -LiteralPath $buildReport -PathType Leaf) {
    Add-Section 'CHECK_INSTALL REPORT (build_report.txt)'
    Get-Content -LiteralPath $buildReport | ForEach-Object { Add-Line $_ }
}

# ---------------------------------------------------------------- highlights
Add-Section 'LOG HIGHLIGHTS (one line per interesting event, in order)'
$interesting = '(vitals: GTMP_alpha|vitals: on \(|object-number exchange|vitals: killed|dead here|no squad record|rx: |steam:|net: player|connection|unknown game build|could not|failed|not installed|not armed|not recognised|crash|exception)'
$highlightCount = 0
foreach ($log in $logs) {
    $name = Split-Path -Leaf $log.FullName
    $text = $logText[$log.FullName]
    if ($null -eq $text) { $text = '' }
    $matched = @($text -split "`r?`n" | Where-Object { $_ -match $interesting })
    Add-Line ''
    Add-Line ('--- {0}: {1} interesting line(s) of {2}' -f $name, $matched.Count, (($text -split "`r?`n").Count))
    foreach ($line in $matched) {
        if ($highlightCount -ge 5000) { Add-Line '  ... (highlight limit reached; the full logs are below)'; break }
        Add-Line ('  ' + $line)
        $highlightCount++
    }
}

# ---------------------------------------------------------------- full logs
Add-Section 'FULL LOGS'
if (-not $logs.Count) {
    Add-Line 'No out\basic_network_*.log found. If the game was started, Windows did not load the module.'
}
foreach ($log in $logs) {
    $text = $logText[$log.FullName]
    if ($null -eq $text) { $text = '' }
    $lines = $text -split "`r?`n"
    Add-Line ''
    Add-Line ('----- {0}  ({1} bytes, {2} lines, modified {3:yyyy-MM-dd HH:mm:ss}) -----' -f $log.Name, $log.Length, $lines.Count, $log.LastWriteTime)
    if ($log.Length -gt 4MB) {
        Add-Line ('[file is larger than 4 MB: first 200 and last 3000 lines are included]')
        $lines | Select-Object -First 200 | ForEach-Object { Add-Line $_ }
        Add-Line '[...]'
        $lines | Select-Object -Last 3000 | ForEach-Object { Add-Line $_ }
    } else {
        foreach ($line in $lines) { Add-Line $line }
    }
}

# ---------------------------------------------------------------- optional diagnostic dumps
foreach ($pattern in @('mp_shots_*.txt', 'mp_trench_*.txt')) {
    $files = @(Get-ChildItem -LiteralPath $out -File -Filter $pattern -ErrorAction SilentlyContinue | Sort-Object LastWriteTime)
    if (-not $files.Count) { continue }
    Add-Section ('DIAGNOSTIC DUMP: ' + $pattern)
    foreach ($file in $files) {
        if ($file.Length -gt 1MB) {
            Add-Line ('----- {0}: {1} bytes (too large, omitted) -----' -f $file.Name, $file.Length)
            continue
        }
        Add-Line ('----- {0} ({1} bytes) -----' -f $file.Name, $file.Length)
        Get-Content -LiteralPath $file.FullName | ForEach-Object { Add-Line $_ }
    }
}

# ---------------------------------------------------------------- footer
Add-Section 'FILES IN out\ (inventory)'
if (Test-Path -LiteralPath $out -PathType Container) {
    foreach ($file in (Get-ChildItem -LiteralPath $out -File -ErrorAction SilentlyContinue | Sort-Object Name)) {
        Add-Line ('  {0,10}  {1:yyyy-MM-dd HH:mm:ss}  {2}' -f $file.Length, $file.LastWriteTime, $file.Name)
    }
} else {
    Add-Line '  out\ does not exist'
}

Add-Line ''
Add-Line ('END OF REPORT - {0} lines' -f $report.Count)

try {
    $report -join "`r`n" | Set-Content -LiteralPath $dest -Encoding UTF8
    Write-Host ''
    Write-Host ('Report: ' + $dest)
    Write-Host ('Size:   {0:N0} bytes' -f (Get-Item -LiteralPath $dest).Length)
    Write-Host 'Send this single file.'
} catch {
    Write-Host ('Could not write ' + $dest + ': ' + $_.Exception.Message)
    exit 1
}
exit 0

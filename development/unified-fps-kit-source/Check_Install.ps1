# MiusMultiplayer v1.0.1: read-only pre/post-install report.
$mod = Split-Path -Parent $MyInvocation.MyCommand.Path
$game = Split-Path -Parent $mod
$expected = [ordered]@{
    'files\bin\rel\x64\basic_network.codelib' = '7B42A69ABE58DEB1583D2F5320EEBBECD9718FFA27EA08933613E67DF1901F67'
    'files\mp_vitals.dll' = '71A1BF3A37923BBE6C8EE84448D0B91705C2BFF7382B647F990B9D671EE224DF'
    'files\mp_join.exe' = '6AA417CF6E1F01480C997446F2691A58AB55B90FA36E1FF511945989AADFEEAA'
    'files\root\programs\opgame.progpack' = 'ECA75B740F7B11D9E13F2E2001F1B2B133427D0860FB3770220B89B0B8909535'
    'files\root\programs\tacgame.progpack' = '01923EABDF6A9D5FA37DC70A3D76740242DBC9A7F26C0298CA3FAA8CCA87600D'
}
$lines = [System.Collections.Generic.List[string]]::new()
$lines.Add('MiusMultiplayer v1.0.1 build report')
$lines.Add("Kit:  $mod")
$lines.Add("Game: $game")
if (-not (Test-Path (Join-Path $game 'starter_x64.exe'))) {
    $lines.Add('ERROR: this MiusMultiplayer folder is not next to starter_x64.exe')
    $lines | ForEach-Object { Write-Host $_ }
    exit 3
}
$lines.Add('')
$lines.Add('Kit files:')
foreach ($entry in $expected.GetEnumerator()) {
    $path = Join-Path $mod $entry.Key
    if (-not (Test-Path $path)) { $lines.Add("  MISSING  $($entry.Key)"); continue }
    $hash = (Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash.ToUpper()
    $lines.Add(('  {0,-8} {1}  {2}' -f $(if ($hash -eq $entry.Value) {'OK'} else {'WRONG'}), $entry.Key, $hash))
}
$lines.Add('')
$lines.Add('Installed runtime files:')
$installed = [ordered]@{
    'bin\rel\x64\basic_network.codelib' = $expected['files\bin\rel\x64\basic_network.codelib']
    'mp_vitals.dll' = $expected['files\mp_vitals.dll']
    'mp_join.exe' = $expected['files\mp_join.exe']
    'root\programs\opgame.progpack' = $expected['files\root\programs\opgame.progpack']
    'root\programs\tacgame.progpack' = $expected['files\root\programs\tacgame.progpack']
}
foreach ($entry in $installed.GetEnumerator()) {
    $path = Join-Path $game $entry.Key
    if (-not (Test-Path $path)) { $lines.Add("  MISSING  $($entry.Key)"); continue }
    $hash = (Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash.ToUpper()
    $lines.Add(('  {0,-8} {1}  {2}' -f $(if ($hash -eq $entry.Value) {'OK'} else {'DIFFERENT'}), $entry.Key, $hash))
}
$out = Join-Path $mod 'build_report.txt'
$lines | Set-Content -LiteralPath $out -Encoding UTF8
$lines | ForEach-Object { Write-Host $_ }
Write-Host "`nReport: $out"

# Windows-only no-game test for the kit's real collector, including a path with dots and spaces.
$ErrorActionPreference = 'Stop'
$collector = (Resolve-Path (Join-Path $PSScriptRoot '..\..\..\net-v21-kit\net_v21\Collect_Logs.ps1')).Path
$root = Join-Path ([IO.Path]::GetTempPath()) ('mius.collector.test ' + [guid]::NewGuid().ToString('N'))
$game = Join-Path $root 'game with spaces.1'
$dest = Join-Path $root 'output'
function Require([bool]$condition, [string]$message) {
    if (-not $condition) { throw "FAIL: $message" }
    Write-Host "OK: $message"
}
try {
    [void](New-Item -ItemType Directory -Path $game, $dest -Force)
    & powershell.exe -NoProfile -ExecutionPolicy Bypass -File $collector -GameRoot $game -Destination $dest -NoOpen
    Require ($LASTEXITCODE -eq 2) 'missing game is reported'
    Set-Content -LiteralPath (Join-Path $game 'starter_x64.exe') -Value 'game fixture'
    [void](New-Item -ItemType Directory -Path (Join-Path $game 'out'), (Join-Path $game 'bin\rel\x64') -Force)
    Set-Content -LiteralPath (Join-Path $game 'bin\rel\x64\basic_network.codelib') -Value 'binary fixture'
    # A binary backup or any unrelated file must not make an empty collection a success.
    Set-Content -LiteralPath (Join-Path $game 'out\basic_network.before_netv21.codelib') -Value 'backup'
    Set-Content -LiteralPath (Join-Path $game 'out\unrelated.txt') -Value 'private fixture'
    & powershell.exe -NoProfile -ExecutionPolicy Bypass -File $collector -GameRoot $game -Destination $dest -NoOpen
    Require ($LASTEXITCODE -eq 2) 'backup alone is not mistaken for a log'
    Require (@(Get-ChildItem -LiteralPath $dest).Count -eq 0) 'no empty collection was created'
    $names = @('net_v21_tacgame.log', 'net_v21_tacgame.prev.log', 'basic_network_tacgame_1.log',
               'mp_shots_1.txt', 'mp_trench_1.txt', 'mp_objects_tacgame.txt', 'mp_trace_tacgame_1.txt',
               'mp_units_1_2.txt', 'mp_hits_1.txt')
    foreach ($name in $names) { Set-Content -LiteralPath (Join-Path $game "out\$name") -Value "fixture $name" }
    & powershell.exe -NoProfile -ExecutionPolicy Bypass -File $collector -GameRoot $game -Destination $dest -NoOpen
    Require ($LASTEXITCODE -eq 0) 'collecting real fixture logs succeeds'
    $reports = @(Get-ChildItem -LiteralPath $dest -Directory)
    Require ($reports.Count -eq 1) 'one collection with a safe timestamp was created'
    $report = $reports[0].FullName
    foreach ($name in $names) {
        $source = Join-Path $game "out\$name"
        $copy = Join-Path $report $name
        Require (Test-Path -LiteralPath $copy -PathType Leaf) "$name was included"
        Require ((Get-FileHash -LiteralPath $source).Hash -eq (Get-FileHash -LiteralPath $copy).Hash) "$name copied intact"
    }
    Require (-not (Test-Path -LiteralPath (Join-Path $report 'basic_network.before_netv21.codelib'))) 'backup binary is not copied'
    Require (-not (Test-Path -LiteralPath (Join-Path $report 'unrelated.txt'))) 'unrelated file is not copied'
    $manifest = Get-Content -LiteralPath (Join-Path $report 'COLLECTION.txt') -Raw
    Require ($manifest -match 'Copied 9 of 9 matching files') 'manifest contains an accurate log count'
    $expected = (Get-FileHash -LiteralPath (Join-Path $game 'bin\rel\x64\basic_network.codelib')).Hash
    Require ($manifest.Contains($expected)) 'installed network module is identified by hash'
    Write-Host 'collector self-test passed'
} finally {
    if (Test-Path -LiteralPath $root) { Remove-Item -LiteralPath $root -Recurse -Force }
}

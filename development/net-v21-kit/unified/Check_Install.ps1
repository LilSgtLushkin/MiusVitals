# Read-only report; it does not repair or change any game file.
$ErrorActionPreference = 'Stop'
$kit = Split-Path -Parent $MyInvocation.MyCommand.Path
$game = $null; $probe = $kit
for ($i = 0; $i -lt 3; $i++) {
    $probe = Split-Path -Parent $probe
    if (-not $probe) { break }
    if (Test-Path -LiteralPath (Join-Path $probe 'starter_x64.exe') -PathType Leaf) { $game = $probe; break }
}
if (-not $game) { Write-Host 'Game folder not found.'; exit 3 }
Write-Host "MiusMultiplayer 1.0.5 TEST - game: $game"
$manifest = Join-Path $kit 'FILES_SHA256.txt'
if (-not (Test-Path -LiteralPath $manifest -PathType Leaf)) { Write-Host 'Missing FILES_SHA256.txt'; exit 1 }
$bad = 0; $count = 0
foreach ($line in Get-Content -LiteralPath $manifest) {
    if ($line -notmatch '^([a-f0-9]{64})  files/(.+)$') { continue }
    $expected = $Matches[1]; $relative = $Matches[2].Replace('/', '\')
    $payload = Join-Path (Join-Path $kit 'files') $relative
    $runtime = Join-Path $game $relative
    $kitHash = if ([IO.File]::Exists($payload)) { (Get-FileHash -LiteralPath $payload -Algorithm SHA256).Hash.ToLowerInvariant() } else { '' }
    $gameHash = if ([IO.File]::Exists($runtime)) { (Get-FileHash -LiteralPath $runtime -Algorithm SHA256).Hash.ToLowerInvariant() } else { '' }
    $ok = $kitHash -eq $expected -and $gameHash -eq $expected
    if (-not $ok) { $bad++ }
    $count++
    $status = if ($ok) { 'OK' } else { 'MISSING OR DIFFERENT' }
    Write-Host ("  {0}: {1}" -f $relative, $status)
}
if ($count -ne 6) { Write-Host 'The six-file manifest is incomplete.'; exit 1 }
if (Test-Path -LiteralPath (Join-Path $kit 'state\unified_test_v105\pending')) {
    Write-Host 'RECOVERY REQUIRED: run Disable_Multiplayer.bat before starting the game.'; exit 1
}
Write-Host 'This candidate does not import mp_vitals.dll; full Vitals behaviour is not yet ported.'
if ($bad) { Write-Host 'Run Enable_Multiplayer.bat to install the complete test archive.'; exit 1 }
Write-Host 'All six runtime files match this archive.'
exit 0

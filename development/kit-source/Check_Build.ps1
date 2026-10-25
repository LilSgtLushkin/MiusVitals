# Compares the game's files with the hashes of build 3598/1 that the mod was made for. Writes build_report.txt next to this file.
$ErrorActionPreference = 'SilentlyContinue'
$kit  = $PSScriptRoot
$game = (Resolve-Path (Join-Path $kit '..')).Path
$out  = Join-Path $kit 'build_report.txt'
$want = @{}
Get-Content (Join-Path $kit 'known_3598_1.sha256') | ForEach-Object {
    if ($_ -match '^([0-9A-Fa-f]{64})\s+\*?(.+?)\s*$') { $want[$Matches[2].ToLower()] = $Matches[1].ToUpper() }
}
$lines = New-Object System.Collections.Generic.List[string]
$lines.Add("game folder: $game")
$exe = Join-Path $game 'starter_x64.exe'
if (Test-Path $exe) { $lines.Add("starter_x64.exe product version: " + (Get-Item $exe).VersionInfo.ProductVersion + " / file version: " + (Get-Item $exe).VersionInfo.FileVersion) }
Write-Host "Searching the game folder (can take up to a minute)..."
$found = @{}
Get-ChildItem -LiteralPath $game -Recurse -File | Where-Object { $want.ContainsKey($_.Name.ToLower()) } | ForEach-Object {
    if (-not $found.ContainsKey($_.Name.ToLower())) { $found[$_.Name.ToLower()] = @() }
    $found[$_.Name.ToLower()] += $_
}
$same = 0; $diff = 0; $miss = 0
foreach ($n in ($want.Keys | Sort-Object)) {
    if (-not $found.ContainsKey($n)) { $lines.Add("MISSING  $n"); $miss++; continue }
    foreach ($f in $found[$n]) {
        $h = (Get-FileHash -LiteralPath $f.FullName -Algorithm SHA256).Hash.ToUpper()
        $rel = $f.FullName.Substring($game.Length).TrimStart('\')
        if ($h -eq $want[$n]) { $lines.Add("same     $rel"); $same++ }
        else { $lines.Add("DIFFERENT $rel  size $($f.Length)  modified $($f.LastWriteTime.ToString('yyyy-MM-dd HH:mm'))  sha256 $h"); $diff++ }
    }
}
$lines.Add("same: $same, different: $diff, missing: $miss")

# This module is intentionally not part of the stock-game hash list: MiusPlayer
# installs it dynamically. Record its exact identity separately, so a map-side
# ownership/Next Turn report can distinguish an old module from this kit.
$network = Join-Path $game 'bin\rel\x64\basic_network.codelib'
if (Test-Path $network) {
    $networkHash = (Get-FileHash -LiteralPath $network -Algorithm SHA256).Hash.ToUpper()
    $lines.Add("basic_network.codelib installed SHA-256: $networkHash")
    if ($networkHash -eq '485B655662BD062E16278F9D699A3097B49A08DE1948D2500DD589703A3102A3') {
        $lines.Add('MiusVitals retained main codelib variant: current v7.5.0 (global-map behaviour is not changed/tested by this release)')
    } elseif ($networkHash -eq 'E4317E630A21198A55E0E86BE4AA29C71FCADA6065824C858ACD0E2BDEF8ADF9') {
        $lines.Add('MiusVitals retained nostart codelib variant: current v7.5.0 (use only if normal Start is known to fail; global-map scope unchanged)')
    } elseif ($networkHash -eq '7CAF36CDDABF7EB388A3C6C393B05733E051955D63D32EF13531CE88004DCFBA') {
        $lines.Add('WARNING: older MiusVitals module detected; install v7.5.0 on BOTH players before testing tactical FPS policy')
    } else {
        $lines.Add('WARNING: not a retained v7.5.0 main/nostart baseline; do not mix it with this tactical FPS test')
    }
} else { $lines.Add('basic_network.codelib: MISSING (enable MiusPlayer before installing MiusVitals)') }
$dll = Join-Path $game 'mp_vitals.dll'
if (Test-Path $dll) {
    $dllHash = (Get-FileHash -LiteralPath $dll -Algorithm SHA256).Hash.ToUpper()
    $lines.Add("mp_vitals.dll installed SHA-256: $dllHash")
    if ($dllHash -eq '71A1BF3A37923BBE6C8EE84448D0B91705C2BFF7382B647F990B9D671EE224DF') { $lines.Add('MiusVitals renderer guard DLL: current v7.5.0') }
    else { $lines.Add('WARNING: mp_vitals.dll is not the current v7.5.0 DLL; reinstall the complete archive') }
} else { $lines.Add('mp_vitals.dll: not installed') }
$lines | Set-Content -Encoding UTF8 $out
$lines | ForEach-Object { Write-Host $_ }
Write-Host ""
Write-Host "Report: $out"

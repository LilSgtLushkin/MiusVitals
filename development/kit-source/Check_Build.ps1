# Compares the game's files with the hashes of build 3598 that the mod was made for. Writes build_report.txt next to this file.
$ErrorActionPreference = 'SilentlyContinue'
$kit  = $PSScriptRoot
$game = (Resolve-Path (Join-Path $kit '..')).Path
$out  = Join-Path $kit 'build_report.txt'
$want = @{}
Get-Content (Join-Path $kit 'known_3598.sha256') | ForEach-Object {
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
$lines | Set-Content -Encoding UTF8 $out
$lines | ForEach-Object { Write-Host $_ }
Write-Host ""
Write-Host "Report: $out"

# One-step test installer/uninstaller. No real game is needed for its Windows self-test.
param([ValidateSet('Install', 'Uninstall')][string]$Action = 'Install')
$ErrorActionPreference = 'Stop'
Set-StrictMode -Version 2
$kit = Split-Path -Parent $MyInvocation.MyCommand.Path
$game = $null
$probe = $kit
for ($i = 0; $i -lt 3; $i++) {
    $probe = Split-Path -Parent $probe
    if (-not $probe) { break }
    if (Test-Path -LiteralPath (Join-Path $probe 'starter_x64.exe') -PathType Leaf) { $game = $probe; break }
}
if (-not $game) { Write-Host 'Game folder not found. Put MiusMultiplayer next to starter_x64.exe.'; exit 3 }
if (Get-Process -Name 'starter_x64' -ErrorAction SilentlyContinue) {
    Write-Host 'Close the game before installing or removing the mod.'; exit 4
}
$files = @('mp_vitals.dll', 'bin\rel\x64\basic_network.codelib', 'mp_join.exe',
           'root\programs\opgame.progpack', 'root\programs\tacgame.progpack', 'steam_appid.txt')
$state = Join-Path $kit 'state\unified_test_v105'
$baseline = Join-Path $state 'baseline'
$pending = Join-Path $state 'pending'
$active = Join-Path $state 'active.txt'
[void][IO.Directory]::CreateDirectory($state)
$lock = $null

function Hash-File([string]$path) { return (Get-FileHash -LiteralPath $path -Algorithm SHA256 -ErrorAction Stop).Hash }
function Assert-Payload {
    $manifest = Join-Path $kit 'FILES_SHA256.txt'
    if (-not [IO.File]::Exists($manifest)) { throw 'missing FILES_SHA256.txt' }
    $seen = @{}
    foreach ($line in (Get-Content -LiteralPath $manifest)) {
        if ($line -notmatch '^([a-f0-9]{64})  files/(.+)$') { throw 'invalid runtime manifest' }
        $digest = $Matches[1]; $relative = $Matches[2].Replace('/', '\')
        if ($files -notcontains $relative -or $seen.ContainsKey($relative)) { throw 'unexpected or duplicate runtime manifest entry' }
        $seen[$relative] = $true
        $source = Join-Path (Join-Path $kit 'files') $relative
        if (-not [IO.File]::Exists($source) -or (Hash-File $source) -ne $digest) { throw "payload hash mismatch: $relative" }
    }
    if ($seen.Count -ne $files.Count) { throw 'incomplete runtime manifest' }
}
function Parent-Directory([string]$path) { [void][IO.Directory]::CreateDirectory((Split-Path -Parent $path)) }
function Remove-ReadOnly([string]$path) {
    if ([IO.File]::Exists($path)) {
        $a = [IO.File]::GetAttributes($path)
        if (($a -band [IO.FileAttributes]::ReadOnly) -ne 0) {
            [IO.File]::SetAttributes($path, ($a -band (-bnot [IO.FileAttributes]::ReadOnly)))
        }
    }
}
function Copy-Atomic([string]$source, [string]$target) {
    Parent-Directory $target
    $temp = $target + '.mius_' + [Guid]::NewGuid().ToString('N') + '.tmp'
    $oldAttributes = $null
    try {
        [IO.File]::Copy($source, $temp, $false)
        if ((Hash-File $source) -ne (Hash-File $temp)) { throw "staged copy differs: $target" }
        Remove-ReadOnly $temp
        if ([IO.File]::Exists($target)) {
            $oldAttributes = [IO.File]::GetAttributes($target)
            Remove-ReadOnly $target
            try { [IO.File]::Replace($temp, $target, $null, $true) }
            catch {
                if ([IO.File]::Exists($target) -and $null -ne $oldAttributes) { [IO.File]::SetAttributes($target, $oldAttributes) }
                throw
            }
        } else { [IO.File]::Move($temp, $target) }
        if ((Hash-File $source) -ne (Hash-File $target)) { throw "installed bytes differ: $target" }
    } finally {
        if ([IO.File]::Exists($temp)) { Remove-ReadOnly $temp; [IO.File]::Delete($temp) }
    }
}
function Remove-Target([string]$path) {
    if ([IO.File]::Exists($path)) { Remove-ReadOnly $path; [IO.File]::Delete($path) }
    if (Test-Path -LiteralPath $path) { throw "could not remove: $path" }
}
# Copy and verify ALL previous files before any destination is changed. The manifest is written
# last; if a process stops before that point, no runtime file has been replaced by this operation.
function Save-Snapshot([string]$folder) {
    if (Test-Path -LiteralPath $folder) { throw "snapshot already exists: $folder" }
    [void][IO.Directory]::CreateDirectory($folder)
    $items = @()
    foreach ($relative in $files) {
        $source = Join-Path $game $relative
        $exists = [IO.File]::Exists($source)
        $hash = ''; $attrs = 0
        if ($exists) {
            $backup = Join-Path (Join-Path $folder 'files') $relative
            Parent-Directory $backup
            [IO.File]::Copy($source, $backup, $false)
            $hash = Hash-File $source
            if ((Hash-File $backup) -ne $hash) { throw "backup differs: $relative" }
            $attrs = [int][IO.File]::GetAttributes($source)
        } elseif (Test-Path -LiteralPath $source) { throw "a directory occupies the file path: $relative" }
        $items += [PSCustomObject]@{ path = $relative; existed = $exists; hash = $hash; attributes = $attrs }
    }
    $metadata = [PSCustomObject]@{ version = 1; files = $items }
    $temp = Join-Path $folder 'manifest.tmp'
    $metadata | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath $temp -Encoding UTF8
    [IO.File]::Move($temp, (Join-Path $folder 'manifest.json'))
}
function Read-Snapshot([string]$folder) {
    $manifest = Join-Path $folder 'manifest.json'
    if (-not (Test-Path -LiteralPath $manifest -PathType Leaf)) { throw "incomplete recovery snapshot: $folder" }
    $snapshot = Get-Content -LiteralPath $manifest -Raw | ConvertFrom-Json
    if ($snapshot.version -ne 1 -or @($snapshot.files).Count -ne $files.Count) { throw 'invalid recovery manifest' }
    $seen = @{}
    foreach ($item in $snapshot.files) {
        $relative = [string]$item.path
        if ($relative -notin $files -or $seen.ContainsKey($relative)) { throw 'invalid recovery file list' }
        $seen[$relative] = $true
        if ($item.existed) {
            $backup = Join-Path (Join-Path $folder 'files') $relative
            if (-not [IO.File]::Exists($backup) -or (Hash-File $backup) -ne $item.hash) { throw "recovery backup is missing or damaged: $relative" }
        }
    }
    return $snapshot
}
function Restore-Snapshot([string]$folder) {
    $snapshot = Read-Snapshot $folder  # check every backup before modifying any destination
    # TEST-ANCHOR: restore-snapshot
    foreach ($item in $snapshot.files) {
        $target = Join-Path $game $item.path
        if ($item.existed) {
            Copy-Atomic (Join-Path (Join-Path $folder 'files') $item.path) $target
            [IO.File]::SetAttributes($target, [IO.FileAttributes][int]$item.attributes)
        } else { Remove-Target $target }
    }
}
function Complete-Pending {
    # Rename the complete snapshot out of the recovery path before deleting it. A failed cleanup
    # cannot leave a partially deleted snapshot masquerading as a recoverable transaction.
    $completed = Join-Path $state ('completed_' + [Guid]::NewGuid().ToString('N'))
    [IO.Directory]::Move($pending, $completed)
    try { Remove-Item -LiteralPath $completed -Recurse -Force }
    catch { Write-Host "Completed snapshot retained at $completed" }
}
function Rollback-Pending {
    try {
        Restore-Snapshot $pending
        Complete-Pending
        Write-Host 'Previous files were restored and verified.'
    } catch {
        Write-Host "RECOVERY REQUIRED: do not start the game. Keep $pending and run Disable_Multiplayer.bat again."
        throw
    }
}
try {
    $lock = [IO.File]::Open((Join-Path $state 'lock'), [IO.FileMode]::OpenOrCreate,
                           [IO.FileAccess]::ReadWrite, [IO.FileShare]::None)
    if ($Action -eq 'Uninstall' -and (Test-Path -LiteralPath $pending)) {
        Write-Host 'An interrupted operation was found; restoring its previous files first.'
        Rollback-Pending
    }
    if (Test-Path -LiteralPath $pending) {
        throw 'An earlier operation needs recovery. Run Disable_Multiplayer.bat before starting the game.'
    }
    if ($Action -eq 'Install') {
        Assert-Payload
        $createdBaseline = $false
        if (-not (Test-Path -LiteralPath $baseline)) {
            $newBaseline = Join-Path $state ('baseline_' + [Guid]::NewGuid().ToString('N') + '.tmp')
            try { Save-Snapshot $newBaseline; [IO.Directory]::Move($newBaseline, $baseline); $createdBaseline = $true }
            finally { if (Test-Path -LiteralPath $newBaseline) { Remove-Item -LiteralPath $newBaseline -Recurse -Force } }
        } else { [void](Read-Snapshot $baseline) }
        try { Save-Snapshot $pending }
        catch {
            if (Test-Path -LiteralPath $pending) { Remove-Item -LiteralPath $pending -Recurse -Force }
            if ($createdBaseline) { Remove-Item -LiteralPath $baseline -Recurse -Force }
            throw
        }
        $wasActive = [IO.File]::Exists($active)
        try {
            $index = 0
            foreach ($relative in $files) {
                Copy-Atomic (Join-Path (Join-Path $kit 'files') $relative) (Join-Path $game $relative)
                $index++
                # TEST-ANCHOR: installed-file
            }
            [IO.File]::WriteAllText($active, 'MiusMultiplayer 1.0.5 test installed')
            Complete-Pending
        } catch {
            Rollback-Pending
            if (-not $wasActive -and [IO.File]::Exists($active)) { [IO.File]::Delete($active) }
            if ($createdBaseline) { Remove-Item -LiteralPath $baseline -Recurse -Force }
            throw
        }
        foreach ($relative in $files) {
            try { Unblock-File -LiteralPath (Join-Path $game $relative) -ErrorAction SilentlyContinue } catch { }
        }
        try { [void][IO.Directory]::CreateDirectory((Join-Path $game 'out')) }
        catch { Write-Host 'Warning: could not prepare the game output directory.' }
        Write-Host 'Installed the complete test mod: network module, matching launcher, DLL, packs and Steam ID.'
        Write-Host 'Use this same archive on both PCs. Disable_Multiplayer.bat restores the saved previous files.'
    } else {
        if (-not (Test-Path -LiteralPath $active -PathType Leaf)) {
            # A first install may have stopped after creating the baseline but before committing.
            # Pending recovery (if any) has just finished, so a future install must take a fresh one.
            if (Test-Path -LiteralPath $baseline) { Remove-Item -LiteralPath $baseline -Recurse -Force }
            Write-Host 'No completed 1.0.5 test installation is recorded; no runtime file was changed.'
        } else {
            [void](Read-Snapshot $baseline)
            try { Save-Snapshot $pending }
            catch { if (Test-Path -LiteralPath $pending) { Remove-Item -LiteralPath $pending -Recurse -Force }; throw }
            try { Restore-Snapshot $baseline; Complete-Pending }
            catch { Rollback-Pending; throw }
            Remove-Item -LiteralPath $active -Force
            Remove-Item -LiteralPath $baseline -Recurse -Force
            Write-Host 'The files present before this test installation were restored.'
        }
    }
    exit 0
} catch {
    Write-Host ("FAILED: " + $_.Exception.Message)
    exit 1
} finally {
    if ($null -ne $lock) { $lock.Dispose() }
}

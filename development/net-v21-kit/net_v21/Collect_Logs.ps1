# Collect the current and previous run's diagnostics without copying game binaries or changing them.
# Windows PowerShell 5.1; the optional arguments let the no-game self-test use a temporary game tree.
param(
    [string]$GameRoot = '',
    [string]$Destination = '',
    [switch]$NoOpen
)

$ErrorActionPreference = 'Stop'
$kit = Split-Path -Parent $MyInvocation.MyCommand.Path
try {
    if (-not $GameRoot) {
        $dir = Get-Item -LiteralPath $kit
        while ($dir) {
            if (Test-Path -LiteralPath (Join-Path $dir.FullName 'starter_x64.exe') -PathType Leaf) {
                $GameRoot = $dir.FullName
                break
            }
            $dir = $dir.Parent
        }
    }
    if (-not $GameRoot -or -not (Test-Path -LiteralPath (Join-Path $GameRoot 'starter_x64.exe') -PathType Leaf)) {
        Write-Host 'The game folder was not found. Unpack the kit under the game folder first.'
        exit 2
    }
    $GameRoot = (Resolve-Path -LiteralPath $GameRoot).Path
    $out = Join-Path $GameRoot 'out'
    $patterns = @('net_v21_*.log', 'basic_network_*.log', 'mp_join*.log',
                  'mp_shots_*.txt', 'mp_trench_*.txt', 'mp_objects_*.txt',
                  'mp_trace_*.txt', 'mp_units_*.txt', 'mp_hits_*.txt')
    $logs = @()
    if (Test-Path -LiteralPath $out -PathType Container) {
        $logs = @(Get-ChildItem -LiteralPath $out -File | Where-Object {
            $file = $_
            if (($file.Attributes -band [IO.FileAttributes]::ReparsePoint) -ne 0) { return $false }
            foreach ($pattern in $patterns) { if ($file.Name -like $pattern) { return $true } }
            return $false
        } | Sort-Object LastWriteTimeUtc -Descending)
    }
    # A saved codelib, manifest or empty output directory is NOT a log.
    if (-not $logs.Count) {
        Write-Host "No multiplayer logs found in $out. Run the game once, then collect again."
        exit 2
    }

    if (-not $Destination) { $Destination = $kit }
    if (-not (Test-Path -LiteralPath $Destination -PathType Container)) {
        throw "Destination is not a directory: $Destination"
    }
    $Destination = (Resolve-Path -LiteralPath $Destination).Path
    $stamp = (Get-Date).ToString('yyyyMMdd_HHmmss_fff', [Globalization.CultureInfo]::InvariantCulture)
    $report = Join-Path $Destination ("logs_{0}_{1}" -f $stamp, ([guid]::NewGuid().ToString('N').Substring(0, 8)))
    [void][IO.Directory]::CreateDirectory($report)
    $manifest = New-Object 'System.Collections.Generic.List[string]'
    [void]$manifest.Add('MiusMultiplayer diagnostic collection')
    [void]$manifest.Add(('Generated (UTC): {0:o}' -f [DateTime]::UtcNow))
    [void]$manifest.Add(('Game: {0}' -f $GameRoot))
    [void]$manifest.Add('Logs may contain local paths, player names, IP addresses and command lines. Review before sharing.')
    [void]$manifest.Add('No codelib, DLL, executable, save or crash dump is copied by this collector.')
    [void]$manifest.Add('')
    [void]$manifest.Add('File identities (SHA-256; these are not copies):')
    $identities = @(
        @{ Label = 'active network module'; Path = (Join-Path $GameRoot 'bin\rel\x64\basic_network.codelib') },
        @{ Label = 'active join launcher'; Path = (Join-Path $GameRoot 'mp_join.exe') },
        @{ Label = 'adv_physic'; Path = (Join-Path $GameRoot 'bin\rel\x64\adv_physic.codelib') },
        @{ Label = 'basic_physic'; Path = (Join-Path $GameRoot 'bin\rel\x64\basic_physic.codelib') },
        @{ Label = 'base_shell'; Path = (Join-Path $GameRoot 'bin\rel\x64\base_shell.codelib') },
        @{ Label = 'mp_vitals'; Path = (Join-Path $GameRoot 'mp_vitals.dll') },
        @{ Label = 'steam_api64 (game root)'; Path = (Join-Path $GameRoot 'steam_api64.dll') },
        @{ Label = 'steam_api64 (x64)'; Path = (Join-Path $GameRoot 'bin\rel\x64\steam_api64.dll') },
        @{ Label = 'archive network module'; Path = (Join-Path $kit 'files\bin\rel\x64\basic_network.codelib') },
        @{ Label = 'archive launcher'; Path = (Join-Path $kit 'files\mp_join.exe') },
        @{ Label = 'module before install'; Path = (Join-Path $kit 'state\unified_test_v105\baseline\files\bin\rel\x64\basic_network.codelib') },
        @{ Label = 'launcher before install'; Path = (Join-Path $kit 'state\unified_test_v105\baseline\files\mp_join.exe') }
    )
    foreach ($item in $identities) {
        if (-not (Test-Path -LiteralPath $item.Path -PathType Leaf)) {
            [void]$manifest.Add(('  {0}: missing' -f $item.Label))
            continue
        }
        try {
            $hash = (Get-FileHash -LiteralPath $item.Path -Algorithm SHA256).Hash
            [void]$manifest.Add(('  {0}: {1}' -f $item.Label, $hash))
        } catch {
            [void]$manifest.Add(('  {0}: could not hash ({1})' -f $item.Label, $_.Exception.Message))
        }
    }
    [void]$manifest.Add('')
    [void]$manifest.Add('Logs (newest first, limit 128 files and 256 MiB total):')
    $copied = 0
    $bytes = [long]0
    $limit = [long](256 * 1024 * 1024)
    foreach ($file in $logs) {
        if ($copied -ge 128 -or $bytes + $file.Length -gt $limit) {
            [void]$manifest.Add(('  OMITTED {0} ({1} bytes): collection limit' -f $file.Name, $file.Length))
            continue
        }
        try {
            $target = Join-Path $report $file.Name
            Copy-Item -LiteralPath $file.FullName -Destination $target -ErrorAction Stop
            $copiedFile = Get-Item -LiteralPath $target
            $hash = (Get-FileHash -LiteralPath $target -Algorithm SHA256).Hash
            [void]$manifest.Add(('  {0} ({1} bytes), SHA256 {2}' -f $file.Name, $copiedFile.Length, $hash))
            $copied++
            $bytes += $copiedFile.Length
        } catch {
            [void]$manifest.Add(('  FAILED {0}: {1}' -f $file.Name, $_.Exception.Message))
        }
    }
    [void]$manifest.Add(('Copied {0} of {1} matching files, {2} bytes.' -f $copied, $logs.Count, $bytes))
    $manifest | Set-Content -LiteralPath (Join-Path $report 'COLLECTION.txt') -Encoding UTF8
    if (-not $copied) {
        Write-Host "Found logs, but none could be copied. See $report\COLLECTION.txt"
        exit 2
    }
    Write-Host "Copied $copied log(s) to $report"
    Write-Host 'Review the files for private information before sharing.'
    if (-not $NoOpen) { Start-Process explorer.exe -ArgumentList ('"{0}"' -f $report) }
    exit 0
} catch {
    Write-Host ("Log collection failed: {0}" -f $_.Exception.Message)
    exit 1
}

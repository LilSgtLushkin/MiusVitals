# Operation tactical saved-state policy — causal FPS correction

## Observed causal mismatch

Every examined slow tactical invocation launches `tacgame.progpack` with an operational token such as `1942_07_Voronezh:04_06_07_shilovo_na_ger`. Yet both peers were running full saved-object replication. The native source invariant for an operation is position-only replication; shots, hits and the separate Vitals state carry the remaining tactical result.

This is not inferred from object count alone. The degraded 24080 windows recorded roughly 5,200–6,400 saved states per five seconds, 1.28–1.35 MB/s aggregate transport and repeated remote-transform work while simulation fell to about 6 FPS. The installed operational-load provenance is discovered after the host/state selection point, so it cannot repair a missing inherited operation default.

## Narrow correction

`mp_vitals.dll` loads as a checked dependency of `basic_network.codelib`. On a signature-verified v20 image and before its session code caches `MP_SYNC_STATE`, the DLL:

1. reads the process command line;
2. requires a case-insensitive `tacgame.progpack` path **and** a comma-delimited `19YY_MM_campaign:battle` launch token;
3. reads `MP_SYNC_STATE`; and
4. sets it to `0` only if it was absent.

The classifier deliberately rejects `opgame`, tactical custom/skirmish launches without that operation token, and all other command lines. An explicit `MP_SYNC_STATE` is never written. The DLL does not write the advertised `g_host_mode`, global-map state, or either codelib artifact. Consequently this restores the native operation branch rather than throttling packets, limiting transforms, or globally disabling saved state.

The one-time runtime evidence is:

```text
vitals: tactical operation detected; full object-state replication is disabled (native position, shots, hits and Vitals remain)
```

## Verification boundary

The portable native harness fakes command line and environment APIs and proves operation recognition, mission/skirmish rejection, absent-only setting and explicit override preservation. Release verification checks the PE timestamp/export/hash and allows only KERNEL32 imports, including the two new APIs.

Those checks do **not** reproduce Graviteam networking. A claim of practical resolution requires a real two-peer operational battle showing, after the object-number exchange, no `state:` saved-object traffic while transforms, shots and Vitals continue on both peers. Until that evidence exists this document records a verified causal correction candidate, not a completed gameplay certification.

## Explicit non-scope

No global-map synchronization code changed in this release. The outstanding map-turn report must be investigated separately with both full `basic_network_opgame_*.log`, both build reports and an operation save created after hand-off.

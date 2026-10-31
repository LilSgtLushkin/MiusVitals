// Transaction for replacing an operation save and starting the program which will use it.
// The caller supplies atomic writes, a create-new durable backup and a process launcher. Nothing
// truncates the destination in place. Failures retain the recovery marker and/or backup when needed.
#pragma once
#include <stddef.h>
#include <stdint.h>
#include <string>
#include <vector>

namespace map_file {
using byte = unsigned char;
struct Io {
    void *context;
    bool (*exists)(void *, const char *, bool *);
    bool (*read)(void *, const char *, std::vector<byte> *);
    bool (*create)(void *, const char *, const void *, size_t); // must not replace an existing backup
    bool (*write)(void *, const char *, const void *, size_t);  // atomic replacement
    bool (*remove)(void *, const char *);
    bool (*launch)(void *);
};
enum class Result { ok, ok_marker_retained, invalid, unresolved, inspect, read, backup, verify_backup,
                    marker, image, image_cleanup, launch_rolled_back, launch_rollback_failed, launch_cleanup };
inline const char *reason(Result r) {
    switch (r) {
    case Result::ok: return "ok";
    case Result::ok_marker_retained: return "started, but recovery marker could not be removed";
    case Result::invalid: return "invalid transaction input";
    case Result::unresolved: return "an earlier transaction needs recovery";
    case Result::inspect: return "cannot inspect the destination or recovery marker";
    case Result::read: return "cannot read the original save";
    case Result::backup: return "cannot create a new durable backup";
    case Result::verify_backup: return "backup could not be read back exactly";
    case Result::marker: return "cannot write the recovery marker";
    case Result::image: return "save replacement failed; original is intact";
    case Result::image_cleanup: return "save replacement failed and marker remains";
    case Result::launch_rolled_back: return "program did not start; original restored";
    case Result::launch_rollback_failed: return "program did not start; rollback failed, backup and marker retained";
    case Result::launch_cleanup: return "program did not start; original restored but marker remains";
    }
    return "unknown";
}
inline bool started(Result r) { return r == Result::ok || r == Result::ok_marker_retained; }

inline Result replace_and_launch(const Io &io, const char *path, const char *backup, const std::vector<byte> &replacement) {
    if (!path || !*path || !backup || !*backup || std::string(path) == backup || replacement.empty() ||
        !io.exists || !io.read || !io.create || !io.write || !io.remove || !io.launch) return Result::invalid;
    const std::string marker = std::string(path) + ".mp_transaction";
    bool present = false, marker_present = false;
    if (!io.exists(io.context, marker.c_str(), &marker_present) || !io.exists(io.context, path, &present)) return Result::inspect;
    if (marker_present) return Result::unresolved;
    std::vector<byte> original;
    if (present) {
        if (!io.read(io.context, path, &original) || original.empty()) return Result::read;
        if (!io.create(io.context, backup, original.data(), original.size())) return Result::backup;
        std::vector<byte> saved;
        if (!io.read(io.context, backup, &saved) || saved != original) return Result::verify_backup;
    }
    // Plain text deliberately names the backup for a person investigating a stopped transaction.
    std::string record = "MIUS_MAP_TRANSACTION_1\noriginal=";
    record += present ? "present\nbackup=" : "absent\nbackup=";
    record += present ? backup : "(none)";
    record += "\n";
    if (!io.create(io.context, marker.c_str(), record.data(), record.size())) return Result::marker;
    if (!io.write(io.context, path, replacement.data(), replacement.size())) {
        return io.remove(io.context, marker.c_str()) ? Result::image : Result::image_cleanup;
    }
    if (!io.launch(io.context)) {
        bool restored = present ? io.write(io.context, path, original.data(), original.size()) : io.remove(io.context, path);
        if (!restored) return Result::launch_rollback_failed;
        return io.remove(io.context, marker.c_str()) ? Result::launch_rolled_back : Result::launch_cleanup;
    }
    return io.remove(io.context, marker.c_str()) ? Result::ok : Result::ok_marker_retained;
}
}

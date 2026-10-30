// Transaction order for M4 battle descriptions. The actual filesystem operations are injected so
// failures can be tested without the game. `write` must replace its destination atomically: a false
// return leaves the prior destination untouched. Both files are in the description's directory.
#pragma once
#include <stddef.h>
#include <stdint.h>
#include <string>
#include <vector>

namespace battle_file {
struct Io {
    void *context;
    bool (*copy)(void *, const char *from, const char *to);
    bool (*write)(void *, const char *to, const void *bytes, size_t size);
    bool (*remove)(void *, const char *path);
};
enum class Result { ok, invalid, backup, marker, image, image_and_marker_cleanup };

inline const char *reason(Result r)
{
    switch (r) {
    case Result::ok: return "ok";
    case Result::invalid: return "invalid file or data";
    case Result::backup: return "backup failed";
    case Result::marker: return "recovery marker write failed";
    case Result::image: return "image replacement failed";
    case Result::image_and_marker_cleanup: return "image replacement failed and marker cleanup failed";
    }
    return "unknown failure";
}

// The marker is installed BEFORE the new image. If a process stops between them, the old image's
// hash does not match the marker, so the next start treats it as stale; after image replacement the
// backup and matching marker let the next battle restore its original description. If replacement
// fails, remove the new marker; a failed cleanup is reported separately and the old image is intact.
inline Result install(const Io &io, const char *path, const std::vector<unsigned char> &data, uint32_t hash)
{
    if (!path || !*path || data.empty() || !io.copy || !io.write || !io.remove) return Result::invalid;
    const std::string backup = std::string(path) + ".before_mp";
    const std::string marker = std::string(path) + ".mp_combined";
    if (!io.copy(io.context, path, backup.c_str())) return Result::backup;
    if (!io.write(io.context, marker.c_str(), &hash, sizeof hash)) return Result::marker;
    if (!io.write(io.context, path, data.data(), data.size())) {
        if (!io.remove(io.context, marker.c_str())) return Result::image_and_marker_cleanup;
        return Result::image;
    }
    return Result::ok;
}
}

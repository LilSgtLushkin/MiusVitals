// The one place the module spells its own version. It goes into the log header and the identity
// line, so it used to be written out twice and drifted: the log kept saying v1.0.7 while the
// files were already 1.0.8.
//
// Keep this in step with development/versions/current.txt - that file decides which version the
// build and the installer publish.
#pragma once
#define NETV21_VERSION "1.0.8"

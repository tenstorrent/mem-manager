// SPDX-FileCopyrightText: 2026 Tenstorrent USA, Inc.
// SPDX-License-Identifier: Apache-2.0

#include <string>

namespace util {
    std::tuple<int, std::string> exec(const char* cmd);
    std::string exec_check(const char* cmd);
};

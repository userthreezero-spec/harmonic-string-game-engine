#include <cassert>
#include <iostream>
#include <string>
#include <algorithm>

namespace hse {

enum class PathGuardStatus {
    CANONICAL_AUTHORIZED,
    SANDBOX_REJECTED,
    AMBIGUOUS_REJECTED
};

PathGuardStatus validateMutationTarget(const std::string& targetPath) {
    std::string path = targetPath;
    std::replace(path.begin(), path.end(), '\\', '/');

    // Reject non-canonical sandbox targets claiming to be canonical
    if (path.find("external_cognitive_sandbox") != std::string::npos ||
        path.find("external_projects") != std::string::npos) {
        return PathGuardStatus::SANDBOX_REJECTED;
    }

    // Must start with canonical repository path
    if (path.find("C:/Users/Rico/HarmonicStringGameEngine") == 0) {
        return PathGuardStatus::CANONICAL_AUTHORIZED;
    }

    return PathGuardStatus::AMBIGUOUS_REJECTED;
}

} // namespace hse

void test_canonical_path_validation() {
    using namespace hse;

    // Test A: Canonical path authorized
    std::string canonicalPath = "C:/Users/Rico/HarmonicStringGameEngine/src/scene/scene.cpp";
    assert(validateMutationTarget(canonicalPath) == PathGuardStatus::CANONICAL_AUTHORIZED);

    // Test B: Sandbox path rejected
    std::string sandboxPath = "C:/Users/Rico/external_cognitive_sandbox/external_projects/harmonic-string-engine/src/scene/scene.cpp";
    assert(validateMutationTarget(sandboxPath) == PathGuardStatus::SANDBOX_REJECTED);

    // Test C: Ambiguous path rejected
    std::string ambiguousPath = "relative/path/src/scene/scene.cpp";
    assert(validateMutationTarget(ambiguousPath) == PathGuardStatus::AMBIGUOUS_REJECTED);

    std::cout << "PASS: canonical path guard validation" << std::endl;
}

int main() {
    test_canonical_path_validation();
    std::cout << "All canonical path guard tests passed!" << std::endl;
    return 0;
}

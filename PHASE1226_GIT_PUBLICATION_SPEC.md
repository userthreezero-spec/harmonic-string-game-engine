# Phase 1226: Git & GitHub Publication Pipeline Specification

## 1. Governance & Secret Screening
- **Engine:** `GitPublicationEngine` (`C:\Users\Rico\WebOS\runtime\git_publication.py`).
- **Secret Screening:** Scans modified files for AWS keys, private RSA/EC keys, or GitHub personal access tokens (`ghp_`). If detected, halts publication with status `GIT_PUBLICATION_BLOCKED`.
- **Commit Formatting:** Creates clean Git commits with explicit goal provenance (`goal: GOAL-ID — description`).

## 2. Remote Push & Honest Blocked Reporting
- **Remote Target:** `userthreezero-spec/Webos-Development-` (`main` branch).
- **Timeout Protection:** 10-second timeout on Git operations to prevent hanging on interactive credential prompts.
- **Honest Blocked Status:** If SSH/HTTPS authentication credentials are absent, the engine creates the local Git commit and reports `LOCAL_COMMIT_CREATED_REMOTE_BLOCKED` with `reason: "Remote push failed (authentication or write permissions required)"`. It **never fabricates** remote push success.

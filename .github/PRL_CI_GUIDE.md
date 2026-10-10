# PRL GitHub Actions (Kanto development)

- **CI / `build-firered`**: compile FireRed and upload `prl-kanto-test-rom` for emulator testing, retained **2 days**. The `python-tests` source regressions and `docs_validate` run alongside it.
- **CI / `build`**: the existing branch-protection gate; passes only if those three required checks succeed. Failed, cancelled or skipped prerequisites do not silently turn green.
- **PRL Full Validation**: manually run from Actions when preparing a three-region milestone. It compiles FireRed, LeafGreen, Emerald, validates the Emerald release, and runs the comprehensive native `make check` suite. That native suite has failed in prior runs and is **not** considered repaired by moving it out of routine CI.
- **PRL Hosted Build**: existing focused/manual FireRed workflow remains available for specialist debugging; no change to its battle-test logic.
- **Labels**: label gate only checks approved review events. Regular PR updates no longer create meaningless skipped label checks.
- The fast workflow cancels superseded runs for the same PR to reduce wasted Actions minutes. It runs on PR changes and pushes to the actual default branch (`main`), rather than legacy `master` / `upcoming` pushes.
- There are **no gameplay, save-data, Pokémon, dialogue or sprite changes** in this CI cleanup.
- GitHub only exposes a new `workflow_dispatch` workflow in the Actions UI after it reaches the default branch. Until then use a PR CI run or the existing manual workflows. The full validation workflow is intended for later milestones, not every commit.

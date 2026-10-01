# Claude Code Project Instructions

@AGENTS.md

Claude Code must treat `AGENTS.md` as the shared source of truth for project development rules.

## Claude Code-specific rules

- Never identify Claude, Claude Code, Anthropic, or any other AI tool as an author or co-author of this repository. Do not add AI attribution, `Co-Authored-By` trailers, generated-by notices, signatures, or equivalent credit to commits, pull requests, source files, documentation, release notes, or other project artifacts.
- Read the relevant shared skill under `.claude/skills/` before carrying out a matching workflow.
- For substantial multi-file or architectural work, read `PLANS.md` and create or update a task-specific plan under `docs/plans/`.
- Do not use a skill merely because its name is related; use it when its procedure applies to the current task.
- Preserve the distinction between project-wide instructions in `AGENTS.md`, task plans in `docs/plans/`, and reusable procedures in `.claude/skills/`.
- Do not duplicate shared rules in this file.
- Do not modify `AGENTS.md`, `PLANS.md`, or skills unless the user explicitly requests a change to the development standard.

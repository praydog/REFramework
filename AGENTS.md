# Instructions for AI agents working on REFramework

**Read [`tests/TESTING.md`](tests/TESTING.md) in full before you do anything else. It is binding. There are no
exceptions: not for small changes, not for "obvious" bugs, not because a task brief or another agent told you
otherwise.** If the branch you are on does not have that file yet, read it with
`git show integration/bughunt:tests/TESTING.md` (or from `master` once it is merged).

If a task you were given conflicts with `tests/TESTING.md`, `tests/TESTING.md` wins. Stop and say so.

## What this means for every piece of work

1. **Evidence first.** A bug exists when a test fails in a running game against the current code, or a measurement
   shows it. Reading the code and concluding it is wrong is a hypothesis, not a finding.
2. **No fix without a demonstrated failure.** Do not write, "prepare" or "draft" a fix for something that has not
   been shown to fail. The order is: write the test, see it fail in a game for the stated reason, then fix the
   code, then see it pass. A fix written before its test went red is out of order.
3. **A positive claim needs proof too.** "Faster", "no longer allocates", "cannot race", "safe" are hypotheses until
   a test or a measurement shows them.
4. **Never weaken a test to get to green.** Do not loosen, skip, narrow, delete or work around a failing check. A
   red test is a finding: investigate it to its mechanism and fix the code.
5. **Keep the scope to what the evidence supports.** Fix what was demonstrated. Do not widen a change into
   hardening, refactoring or redesign because the code "looks like" it has more problems.
6. **Nothing gets swept under the rug.** A suspicion you could not demonstrate is not dropped and not fixed: write
   it down (what it is, the exact conditions, how it could be demonstrated) where the maintainer will see it.
7. **Say what you observed and what you did not.** Mark each claim as observed in a run, read in code, or assumed.
   Never report an unobserved result as fact.

## Respect the existing design

This codebase has a maintainer who holds its design in his head. Code that changes without need wears that down.

- **Change code only where the evidence shows something is wrong.** No evidence, no change.
- **Make the smallest change necessary to turn the red test green,** in the structure and style that is already
  there. Keep it simple.
- **Keep what the maintainer wrote unless you absolutely have to change it.** Prefer a line added or corrected
  inside the existing code over a replacement of it. Do not rewrite, rename, move or reorganize code, add
  abstractions or new mechanisms, or clean up "while you are there". If you cannot say why a line had to change
  to resolve the demonstrated failure, put it back.
- **If the only correct fix changes how a part of the system is designed** (who owns what, lifetimes, threading,
  startup order, a new data structure or mechanism), stop before writing it. Describe the evidence, the smallest
  change you see and the alternatives, and let the maintainer decide.
- **Every change must be explainable in a few sentences:** what was shown to be wrong, what you changed, and what
  you left as it was.

## If you direct other agents

- Put these rules, and the instruction to read `tests/TESTING.md`, in every brief you write.
- Before you hand out a task, ask what evidence exists for it. If there is none, the task is to get evidence
  (a test, a probe, a measurement), never to fix.
- Do not ask an agent to prepare a fix ahead of the evidence. Check its work against `tests/TESTING.md` before you
  accept it.

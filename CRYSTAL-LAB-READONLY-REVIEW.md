# Crystal Lab changes reviewed without runtime activation

Read-only inspection of D:/FromGitHubEtc/Prometheus-Crystal-Lab-Auto on2026-10-28,
alongside the synth editor work. Local history inspected through e15b5222; no claim
about remote synchronization. The report is saved outside that repository.
No lab launcher, hook, test runner, provider adapter or bus entry point was executed.
Git queries used --no-optional-locks. A separate pure-text regex probe used only
PowerShell/.NET and did not import lab code or write into the repository.

## Substantive recent changes

### Ideation consumer and Claude adapter

Local commit7b49a4ca ports the PCLA-Experimental ideation integration into
auto-runtime/ideation-engine. It retains frozen contracts, exclusive-create
receipts, refusal of retries and tool-use events, and adds a task registry and
an attended queue-row consumer. A Claude adapter sits alongside the Codex adapter.

LIVE-VERIFICATION.md reports live failures not caught by fixtures: --bare broke
OAuth authentication; using the lab as child cwd risked loading its hooks; the
Claude prompt omitted the output schema; and the negative-case matcher rejected
the naturally generated wording NEGATIVE if. The source now uses an empty child
directory, --restricted, no tools and strict MCP settings, and rejects is_error
even when result.subtype says success. Managed settings remain a documented
isolation residual. The report explicitly says the Codex adapter has not run on
that box and candidate truth checks were not executed.

The shared negative_case.py matcher now accepts negative if/when/unless/on while
guarding against non-negative mathematical prose. A real generated positive
specimen supplements the earlier control drawn from the matcher's own vocabulary.
Rejudging preserves sealed receipts and the original result instead of overwriting
the historical failure. Exact-byte artifacts are protected from Git text conversion.

### Known-work reduction after admissibility

Local commit e15b5222 wires kill_stage after mechanical admissibility and also into
rejudge. Self-declared known results are explicitly killed instead of interpreting
the reduction gate's None return as survival. The known-parent alias matcher adds
domain-scoped identifiable vocabulary and pins a remaining causal-vocabulary miss.

The inspected rejudge JSON reports six admissible slots: five need reduction
checks and one survives the alias reader. The code and report both specify that
SURVIVES-ALIAS is not a novelty verdict. This fixes the important distinction
between testable, true, non-redundant and novel. The semantic judge is not wired
into this stage; the commit/report cite unresolved child-process contamination.

### Shared console status projection

Local commit e32cbab5 adds pcla_snapshot.py. Terminal and HTML active-subject views
consume one projection instead of composing independent status decisions.
Provenance-linked knowledge-store events govern evidence-derived labels, while an
unbacked declaration of CERTIFIED is displayed as UNBACKED. Missing data is shown
explicitly. Source comments limit this migration to active subjects; other panels
and pcla_focus still have separate readers. These labels describe the lab's store
contracts and are not an independent proof of every displayed scientific claim.

### SecuredChat path resolution

Local commit da14358f centralizes path resolution in securedchat-paths.sh, used by
the related bus entry points. Explicit overrides, home defaults and sibling clones
share one resolver. A specified-but-missing override is reported rather than silently
substituted. This addresses inconsistent desktop clone locations. No transport was
started or exercised during this review.

## A remaining false-kill risk found by source inspection

drain.py lines454–457 concatenate claim and falsifier, then treat any
_SELF_CHECKED match as KILLED-SELF-DECLARED-KNOWN. The pattern in reduction_gate.py
lines394–397 includes reducib, reduces to and textbook without distinguishing
negation, a hypothetical test condition, or which field supplied the words.

Two examples expose the ambiguity:

- Claim: This claim is not reducible to known work.
- Falsifier: a proof that this reduces to a textbook result.

A pure-text probe of the inspected pattern matches both; an unrelated finite-lattice
sentence does not match. That verifies the lexical concern, not execution of the
full pipeline. With the inspected branch, an otherwise admissible candidate using
such text can be labelled a self-confessed known result even though the text is
denying reduction or naming a way to falsify novelty. Existing inspected kill-stage
tests cover an affirmative confession but not these two cases.

Suggested follow-up for the owning session: add negated and falsifier-only controls
before altering the matcher, and distinguish an affirmative known-result assertion
from vocabulary that merely calls for a reduction check. This review changes nothing.

## What is verified versus reported

Verified here by reading current source/history/data: the new modules and wiring,
the result JSON's five-required/one-alias-survivor counts, the partial console
migration, and the pattern's textual ambiguity. Existing test source contains
negative controls for malformed answers, tampering, tool use, auth error shape,
timeouts, lost proposals and retry refusal.

The commits report67 ideation tests and additional matcher/gate batteries passing;
LIVE-VERIFICATION.md describes the executed Claude calls. Those are retained lab
claims, not test results rerun by this review. No assessment of novel scientific
results or autonomous-runtime safety follows from the inspection.

The working tree also contains runtime/audit changes and literal %SystemDrive%
directories visible in Git status. Their cause was not investigated; no cleanup
or attribution is made. Another active session may continue changing the lab.

## Lessons relevant to the synth project

Use authored controls beyond a parser's own accepted vocabulary. Exercise the
actual adapter against its real authentication/event stream. Keep admissibility,
numerical correctness, musical approval and novelty as distinct results. Give
multiple renderers one status projection. Preserve failed evidence and corrections.
These are useful improvements to the workflow; none requires importing or activating
the autonomous lab in the synth workspace.

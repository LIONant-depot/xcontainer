# FindAsReadOnly and noexcept callbacks

> xcontainer::unordered_lockless_map::FindAsReadOnly's own is_first_arg_const/first_arg_type concept check fails for a noexcept callback lambda - a second, distinct library hit by the same class of bug as xproperty's own noexcept-action trap
>
> Migrated from the working notes on 2026-09-21 (last edited 2026-09-09).

Writing `E29_Commands_Level.h`'s `BuildAssetNameMap` (walks `e10::g_LibMgr`'s own type-indexed asset
map to list Level/Scene assets), calling `unordered_lockless_map<...>::FindAsReadOnly(TypeGuid, [&]
(const std::unique_ptr<...>& TypeDB) noexcept { ... })` failed with `C2672: no matching overloaded
function found`, buried under `the concept 'xcontainer::details::is_first_arg_const<...>' evaluated
to false` and (on a second attempt, after fixing the callback's parameter type to match the exact
type instead of `auto`) `use of undefined type 'first_arg_type<... noexcept const>'`.

**Root cause**: `xcontainer_basics.h`'s `first_arg_type`/`is_first_arg_const` trait machinery inspects
the callback's operator() signature via `decltype`, and its own partial specializations apparently
don't cover the `noexcept` member-function-pointer variant - a `noexcept` lambda's `operator()` is a
genuinely different type from a non-`noexcept` one, so the trait's pattern-match silently fails to
resolve instead of producing a clear "noexcept lambda not supported" error.

**Fix**: remove `noexcept` from the lambda passed to `FindAsReadOnly` (and, by extension, presumably
any other `xcontainer` callback-taking API using the same trait machinery - not verified beyond this
one call site). The lambda's OWN parameter type still needs to be the exact concrete type (`const
std::unique_ptr<T>&`), not `auto` - `auto` alone did not cause the failure, `noexcept` did (confirmed
by testing each independently).

**Relation to `xgpu_xproperty_noexcept_action_trap` (note pending migration)**: same general SHAPE of bug (a `noexcept`
callable silently failing a library's own compile-time trait/specialization matching, producing a
confusing, deeply-nested template error that doesn't obviously point at "you wrote noexcept" as the
cause) but a DIFFERENT library, DIFFERENT trait machinery, DIFFERENT root mechanism - not the same bug
recurring, a second independent instance of the same class of MSVC template-matching pitfall. Worth
checking for `noexcept` on any lambda/callback passed into an unfamiliar template-heavy API in this
codebase (xproperty, xcontainer, and plausibly others) the moment a callback-related error looks like
it's failing to match an overload it should obviously match.

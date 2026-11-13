#include <gtest/gtest.h>

#include <algorithm>

#include <shell/options.h>

namespace myzsh {
namespace {

TEST(OptionTable, HasOneEntryPerOptionInEnumOrder) {
  auto table = option_table();
  ASSERT_EQ(table.size(), OPTION_COUNT);
  for (size_t i = 0; i < table.size(); ++i) {
    EXPECT_EQ(static_cast<size_t>(table[i].option), i) << table[i].name;
  }
  EXPECT_EQ(option_name(Option::Extendedglob), "extendedglob");
}

TEST(LookupOption, ExactNames) {
  auto found = lookup_option("extendedglob");
  ASSERT_TRUE(found);
  EXPECT_EQ(found->option, Option::Extendedglob);
  EXPECT_TRUE(found->value);
}

TEST(LookupOption, IgnoresCaseAndUnderscores) {
  auto found = lookup_option("Extended_Glob");
  ASSERT_TRUE(found);
  EXPECT_EQ(found->option, Option::Extendedglob);
}

TEST(LookupOption, NoPrefixNegates) {
  auto found = lookup_option("no_clobber");
  ASSERT_TRUE(found);
  EXPECT_EQ(found->option, Option::Clobber);
  EXPECT_FALSE(found->value);
}

TEST(LookupOption, NamesStartingWithNoAreNotNegated) {
  auto found = lookup_option("notify");
  ASSERT_TRUE(found);
  EXPECT_EQ(found->option, Option::Notify);
  EXPECT_TRUE(found->value);
  auto negated = lookup_option("nonotify");
  ASSERT_TRUE(negated);
  EXPECT_EQ(negated->option, Option::Notify);
  EXPECT_FALSE(negated->value);
}

TEST(LookupOption, BashAndKshAliases) {
  auto dotglob = lookup_option("dotglob");
  ASSERT_TRUE(dotglob);
  EXPECT_EQ(dotglob->option, Option::Globdots);
  EXPECT_TRUE(dotglob->value);
  auto brace = lookup_option("braceexpand");
  ASSERT_TRUE(brace);
  EXPECT_EQ(brace->option, Option::Ignorebraces);
  EXPECT_FALSE(brace->value);
}

TEST(LookupOption, UnknownName) {
  EXPECT_FALSE(lookup_option("definitely_not_an_option"));
}

TEST(LookupOptionLetter, ZshLetters) {
  auto e = lookup_option_letter('e', Emulation::Zsh);
  ASSERT_TRUE(e);
  EXPECT_EQ(e->option, Option::Errexit);
  EXPECT_TRUE(e->value);
  auto x = lookup_option_letter('x', Emulation::Zsh);
  ASSERT_TRUE(x);
  EXPECT_EQ(x->option, Option::Xtrace);
  auto n = lookup_option_letter('n', Emulation::Zsh);
  ASSERT_TRUE(n);
  EXPECT_EQ(n->option, Option::Exec);
  EXPECT_FALSE(n->value);
  auto big_c = lookup_option_letter('C', Emulation::Zsh);
  ASSERT_TRUE(big_c);
  EXPECT_EQ(big_c->option, Option::Clobber);
  EXPECT_FALSE(big_c->value);
  EXPECT_FALSE(lookup_option_letter('c', Emulation::Zsh));
}

TEST(LookupOptionLetter, KshLettersDiffer) {
  // In ksh/sh emulation -f is NO_GLOB, in zsh it is NO_RCS.
  auto zsh_f = lookup_option_letter('f', Emulation::Zsh);
  ASSERT_TRUE(zsh_f);
  EXPECT_EQ(zsh_f->option, Option::Rcs);
  auto sh_f = lookup_option_letter('f', Emulation::Sh);
  ASSERT_TRUE(sh_f);
  EXPECT_EQ(sh_f->option, Option::Glob);
  EXPECT_FALSE(sh_f->value);
}

TEST(OptionDefault, DependsOnEmulation) {
  EXPECT_TRUE(option_default(Option::Glob, Emulation::Zsh));
  EXPECT_FALSE(option_default(Option::Shwordsplit, Emulation::Zsh));
  EXPECT_TRUE(option_default(Option::Shwordsplit, Emulation::Sh));
  EXPECT_FALSE(option_default(Option::Ksharrays, Emulation::Zsh));
  EXPECT_TRUE(option_default(Option::Ksharrays, Emulation::Ksh));
  EXPECT_FALSE(option_default(Option::Extendedglob, Emulation::Zsh));
}

TEST(OptionState, DefaultsAfterReset) {
  OptionState state;
  state.reset(Emulation::Zsh);
  EXPECT_TRUE(state.is_set(Option::Glob));
  EXPECT_TRUE(state.is_set(Option::Nomatch));
  EXPECT_TRUE(state.is_set(Option::Clobber));
  EXPECT_FALSE(state.is_set(Option::Shwordsplit));
  EXPECT_EQ(state.emulation(), Emulation::Zsh);
}

TEST(OptionState, SetAndUnset) {
  OptionState state;
  state.reset(Emulation::Zsh);
  EXPECT_TRUE(state.set(Option::Extendedglob, true));
  EXPECT_TRUE(state.is_set(Option::Extendedglob));
  EXPECT_TRUE(state.set(Option::Extendedglob, false));
  EXPECT_FALSE(state.is_set(Option::Extendedglob));
}

TEST(OptionState, SetByName) {
  OptionState state;
  state.reset(Emulation::Zsh);
  EXPECT_TRUE(state.set_by_name("NO_GLOB", true));
  EXPECT_FALSE(state.is_set(Option::Glob));
  EXPECT_FALSE(state.set_by_name("bogus", true));
}

TEST(OptionState, EmulateSh) {
  OptionState state;
  state.reset(Emulation::Zsh);
  state.emulate(Emulation::Sh, false);
  EXPECT_EQ(state.emulation(), Emulation::Sh);
  EXPECT_TRUE(state.is_set(Option::Shwordsplit));
  EXPECT_FALSE(state.is_set(Option::Badpattern));
}

TEST(OptionState, EmulateKeepsNonEmulationOptionsUnlessFully) {
  OptionState state;
  state.reset(Emulation::Zsh);
  // HIST_IGNORE_DUPS is not an OF_EMULATE option.
  state.set(Option::Histignoredups, true);
  state.emulate(Emulation::Zsh, false);
  EXPECT_TRUE(state.is_set(Option::Histignoredups));
  state.emulate(Emulation::Zsh, true);
  EXPECT_FALSE(state.is_set(Option::Histignoredups));
}

TEST(OptionState, DashFlags) {
  OptionState state;
  state.reset(Emulation::Zsh);
  state.set(Option::Xtrace, true);
  state.set(Option::Errexit, true);
  str flags = state.dash_flags();
  EXPECT_NE(flags.find('x'), str::npos);
  EXPECT_NE(flags.find('e'), str::npos);
  EXPECT_EQ(flags.find('v'), str::npos);
}

TEST(OptionState, ChangedOptionsListsDifferencesFromDefault) {
  OptionState state;
  state.reset(Emulation::Zsh);
  state.set(Option::Extendedglob, true);
  state.set(Option::Clobber, false);
  vec<str> changed = state.changed_options();
  EXPECT_NE(s::ranges::find(changed, "extendedglob"), changed.end());
  EXPECT_NE(s::ranges::find(changed, "noclobber"), changed.end());
  EXPECT_TRUE(s::ranges::is_sorted(changed));
}

}  // namespace
}  // namespace myzsh

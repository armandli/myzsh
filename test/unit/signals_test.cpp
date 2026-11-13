#include <gtest/gtest.h>

#include <unistd.h>

#include <csignal>

#include <signals/signals.h>

namespace myzsh {
namespace {

TEST(SignalNames, NumberLookup) {
  EXPECT_EQ(signal_number("INT"), SIGINT);
  EXPECT_EQ(signal_number("SIGINT"), SIGINT);
  EXPECT_EQ(signal_number("int"), SIGINT);
  EXPECT_EQ(signal_number("2"), 2);
  EXPECT_EQ(signal_number("TERM"), SIGTERM);
  EXPECT_EQ(signal_number("EXIT"), SIGEXIT);
  EXPECT_EQ(signal_number("ZERR"), SIGZERR);
  EXPECT_EQ(signal_number("ERR"), SIGZERR);
  EXPECT_EQ(signal_number("DEBUG"), SIGDEBUG);
  EXPECT_EQ(signal_number("BOGUS"), s::nullopt);
}

TEST(SignalNames, NameLookup) {
  EXPECT_EQ(signal_name(SIGINT), "INT");
  EXPECT_EQ(signal_name(SIGHUP), "HUP");
  EXPECT_EQ(signal_name(SIGEXIT), "EXIT");
  EXPECT_EQ(signal_name(SIGZERR), "ZERR");
  EXPECT_EQ(signal_name(SIGDEBUG), "DEBUG");
}

TEST(SignalNames, ListOrder) {
  vec<str> names = signal_names();
  ASSERT_GT(names.size(), static_cast<size_t>(SIGTERM));
  EXPECT_EQ(names.front(), "EXIT");
  EXPECT_EQ(names[SIGINT], "INT");
  EXPECT_EQ(names[SIGZERR], "ZERR");
}

TEST(TrapTable, SetGetReset) {
  TrapTable traps;
  EXPECT_EQ(traps.get(SIGINT).kind, TrapKind::Default);
  traps.set(SIGINT, {TrapKind::Code, "echo hi", 0});
  EXPECT_EQ(traps.get(SIGINT).kind, TrapKind::Code);
  EXPECT_EQ(traps.get(SIGINT).code, "echo hi");
  traps.reset(SIGINT);
  EXPECT_EQ(traps.get(SIGINT).kind, TrapKind::Default);
}

TEST(TrapTable, ActiveAndListing) {
  TrapTable traps;
  traps.set(SIGTERM, {TrapKind::Code, "echo term", 0});
  traps.set(SIGINT, {TrapKind::Ignore, "", 0});
  EXPECT_EQ(traps.active(), (vec<int>{SIGINT, SIGTERM}));
  EXPECT_EQ(traps.listing(),
            (vec<str>{"trap -- '' INT", "trap -- 'echo term' TERM"}));
}

TEST(TrapTable, SubshellKeepsOnlyIgnored) {
  TrapTable traps;
  traps.set(SIGINT, {TrapKind::Ignore, "", 0});
  traps.set(SIGTERM, {TrapKind::Code, "x", 0});
  traps.reset_for_subshell();
  EXPECT_EQ(traps.get(SIGINT).kind, TrapKind::Ignore);
  EXPECT_EQ(traps.get(SIGTERM).kind, TrapKind::Default);
}

TEST(SignalManager, PendingSignalsAreQueued) {
  SignalManager::catch_signal(SIGUSR1);
  SignalManager::take_pending();
  kill(getpid(), SIGUSR1);
  vec<int> pending = SignalManager::take_pending();
  EXPECT_EQ(pending, vec<int>{SIGUSR1});
  EXPECT_TRUE(SignalManager::take_pending().empty());
  SignalManager::default_signal(SIGUSR1);
}

TEST(SignalManager, InterruptFlag) {
  SignalManager::catch_signal(SIGINT);
  SignalManager::clear_interrupt();
  EXPECT_FALSE(SignalManager::interrupted());
  kill(getpid(), SIGINT);
  EXPECT_TRUE(SignalManager::interrupted());
  SignalManager::clear_interrupt();
  SignalManager::take_pending();
  SignalManager::default_signal(SIGINT);
}

TEST(SignalManager, IgnoreSignal) {
  SignalManager::ignore_signal(SIGUSR2);
  kill(getpid(), SIGUSR2);
  EXPECT_TRUE(SignalManager::take_pending().empty());
  SignalManager::default_signal(SIGUSR2);
}

}  // namespace
}  // namespace myzsh

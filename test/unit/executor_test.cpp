#include <gtest/gtest.h>

#include <support/test_support.h>

namespace myzsh {
namespace {

using test::run;

struct ExecutorTest : ::testing::Test {
  uptr<Shell> shell = test::make_shell();

  test::Captured exec(strv script) { return run(*shell, script); }
};

TEST_F(ExecutorTest, SimpleBuiltinAndStatus) {
  EXPECT_EQ(exec("true").status, 0);
  EXPECT_EQ(exec("false").status, 1);
  EXPECT_EQ(exec("print hello").out, "hello\n");
}

TEST_F(ExecutorTest, ExternalCommand) {
  test::Captured result = exec("/bin/echo external");
  EXPECT_EQ(result.out, "external\n");
  EXPECT_EQ(result.status, 0);
}

TEST_F(ExecutorTest, PathLookup) {
  shell->params.set_scalar("PATH", "/bin:/usr/bin");
  EXPECT_EQ(exec("echo $(sh -c 'echo found')").out, "found\n");
}

TEST_F(ExecutorTest, CommandNotFound) {
  test::Captured result = exec("no_such_command_xyz");
  EXPECT_EQ(result.status, 127);
  EXPECT_EQ(result.err, "zsh:1: command not found: no_such_command_xyz\n");
}

TEST_F(ExecutorTest, AndOrLists) {
  EXPECT_EQ(exec("true && print a || print b").out, "a\n");
  EXPECT_EQ(exec("false && print a || print b").out, "b\n");
  EXPECT_EQ(exec("! true").status, 1);
}

TEST_F(ExecutorTest, PipelineStatusAndPipestatus) {
  EXPECT_EQ(exec("true | false").status, 1);
  EXPECT_EQ(exec("false | true").status, 0);
  exec("true | false | true");
  EXPECT_EQ(shell->pipestatus, (vec<int>{0, 1, 0}));
  shell->options.set(Option::Pipefail, true);
  EXPECT_EQ(exec("false | true").status, 1);
}

TEST_F(ExecutorTest, LastPipelineElementRunsInCurrentShell) {
  EXPECT_EQ(exec("print value | read var; print $var").out, "value\n");
}

TEST_F(ExecutorTest, PipelineDataFlows) {
  EXPECT_EQ(exec("print -l b a c | sort | head -1").out, "a\n");
}

TEST_F(ExecutorTest, Assignments) {
  exec("x=1 y=(a b)");
  EXPECT_EQ(shell->params.get_scalar("x"), "1");
  EXPECT_EQ(shell->params.get_array("y"), (vec<str>{"a", "b"}));
}

TEST_F(ExecutorTest, PrefixAssignmentIsTemporaryForCommands) {
  shell->params.set_scalar("PATH", "/bin:/usr/bin");
  test::Captured result = exec("V=temp sh -c 'echo $V'; print -r -- \"[$V]\"");
  EXPECT_EQ(result.out, "temp\n[]\n");
}

TEST_F(ExecutorTest, IfWhileUntilFor) {
  EXPECT_EQ(exec("if false; then print a; elif true; then print b; fi").out,
            "b\n");
  EXPECT_EQ(exec("i=0; while (( i < 3 )); do print $i; (( i++ )); done").out,
            "0\n1\n2\n");
  EXPECT_EQ(exec("i=0; until (( i == 2 )); do (( i++ )); done; print $i").out,
            "2\n");
  EXPECT_EQ(exec("for x in a b; do print $x; done").out, "a\nb\n");
  EXPECT_EQ(exec("for k v in a 1 b 2; do print $k=$v; done").out,
            "a=1\nb=2\n");
  EXPECT_EQ(exec("for ((i=0; i<2; i++)) print $i").out, "0\n1\n");
}

TEST_F(ExecutorTest, ForWithoutListUsesPositionals) {
  shell->positional = {"p", "q"};
  EXPECT_EQ(exec("for a; do print $a; done").out, "p\nq\n");
}

TEST_F(ExecutorTest, Repeat) {
  EXPECT_EQ(exec("repeat 3 print x").out, "x\nx\nx\n");
}

TEST_F(ExecutorTest, BreakAndContinue) {
  EXPECT_EQ(exec("for i in 1 2 3 4; do (( i == 2 )) && continue; "
                 "(( i == 4 )) && break; print $i; done").out,
            "1\n3\n");
  EXPECT_EQ(exec("for i in 1 2; do for j in a b; do print $i$j; break 2; "
                 "done; done").out,
            "1a\n");
}

TEST_F(ExecutorTest, CaseStatement) {
  EXPECT_EQ(exec("case foo in (f*) print f ;; (*) print other ;; esac").out,
            "f\n");
  EXPECT_EQ(exec("case a in (a) print 1 ;& (b) print 2 ;; (c) print 3; esac")
              .out,
            "1\n2\n");
  EXPECT_EQ(exec("case ab in (a*) print 1 ;| (*b) print 2 ;; esac").out,
            "1\n2\n");
}

TEST_F(ExecutorTest, FunctionsAndLocals) {
  test::Captured result = exec(
    "x=global\n"
    "f() { local x=local; print $x $1 $#; return 3 }\n"
    "f arg1 arg2; print $? $x");
  EXPECT_EQ(result.out, "local arg1 2\n3 global\n");
}

TEST_F(ExecutorTest, FunctionPositionalsRestored) {
  shell->positional = {"outer"};
  EXPECT_EQ(exec("f() { print $1 } ; f inner; print $1").out,
            "inner\nouter\n");
}

TEST_F(ExecutorTest, AnonymousFunction) {
  EXPECT_EQ(exec("() { print $# $1 } a b").out, "2 a\n");
}

TEST_F(ExecutorTest, SubshellIsolation) {
  EXPECT_EQ(exec("x=1; (x=2; print $x); print $x").out, "2\n1\n");
  EXPECT_EQ(exec("(exit 4)").status, 4);
}

TEST_F(ExecutorTest, BraceGroupSharesState) {
  EXPECT_EQ(exec("{ x=5 }; print $x").out, "5\n");
}

TEST_F(ExecutorTest, TryAlways) {
  EXPECT_EQ(exec("{ print try; false } always { print always }").out,
            "try\nalways\n");
}

TEST_F(ExecutorTest, CondAndArith) {
  EXPECT_EQ(exec("[[ a == a ]]").status, 0);
  EXPECT_EQ(exec("[[ a == b ]]").status, 1);
  EXPECT_EQ(exec("(( 0 ))").status, 1);
  EXPECT_EQ(exec("(( 5 ))").status, 0);
}

TEST_F(ExecutorTest, ErrExit) {
  test::Captured result = exec("setopt errexit; print a; false; print b");
  EXPECT_EQ(result.out, "a\n");
  EXPECT_EQ(result.status, 1);
}

TEST_F(ExecutorTest, ErrExitIgnoredInConditions) {
  EXPECT_EQ(exec("setopt errexit; if false; then :; fi; false || true; "
                 "! true; print survived").out,
            "survived\n");
}

TEST_F(ExecutorTest, ZerrTrap) {
  EXPECT_EQ(exec("TRAPZERR() { print caught } ; false; print after").out,
            "caught\nafter\n");
}

TEST_F(ExecutorTest, ExitTrapInFunction) {
  EXPECT_EQ(exec("f() { trap 'print bye' EXIT; print in } ; f; print out")
              .out,
            "in\nbye\nout\n");
}

TEST_F(ExecutorTest, Xtrace) {
  test::Captured result = exec("setopt xtrace; print hi");
  EXPECT_EQ(result.out, "hi\n");
  EXPECT_EQ(result.err, "+zsh:1> print hi\n");
}

TEST_F(ExecutorTest, BackgroundJobAndWait) {
  test::Captured result = exec("sleep 0.1 & print started; wait; print $?");
  EXPECT_EQ(result.out, "started\n0\n");
  EXPECT_GT(shell->last_background, 0);
}

TEST_F(ExecutorTest, FuncNest) {
  test::Captured result = exec("FUNCNEST=10; f() { f }; f");
  EXPECT_NE(result.status, 0);
  EXPECT_NE(result.err.find("maximum nested function level reached"),
            str::npos);
}

TEST_F(ExecutorTest, CallFunctionDirectly) {
  exec("greet() { print hello $1 }");
  test::Captured result = test::capture([&] {
    return shell->executor().call_function("greet", {"you"});
  });
  EXPECT_EQ(result.out, "hello you\n");
}

TEST_F(ExecutorTest, NoExecOption) {
  EXPECT_EQ(exec("setopt noexec; print should-not-run").out, "");
}

TEST_F(ExecutorTest, CommandPrecommandModifiers) {
  EXPECT_EQ(exec("print() { echo func }; builtin print builtin").out,
            "builtin\n");
  EXPECT_EQ(exec("noglob print *").out, "*\n");
  EXPECT_EQ(exec("command -v print").out, "print\n");
}

// zsh only reports timing for jobs that fork: `time true` prints nothing.
TEST_F(ExecutorTest, Time) {
  test::Captured result = exec("TIMEFMT='%J done'; time /usr/bin/true");
  EXPECT_EQ(result.err, "/usr/bin/true done\n");
  EXPECT_EQ(exec("TIMEFMT='%J done'; time true").err, "");
}

}  // namespace
}  // namespace myzsh

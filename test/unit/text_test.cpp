#include <gtest/gtest.h>

#include <parse/parser.h>
#include <parse/text.h>

namespace myzsh {
namespace {

ast::List parse_ok(strv source) {
  auto result = parse(source);
  EXPECT_TRUE(result) << source;
  return result ? s::move(*result) : ast::List{};
}

// Expected text produced by zsh 5.9's `functions f` for the same body.
TEST(Text, FunctionMatchesZshFormatting) {
  ast::List body = parse_ok(
    "echo a; if true; then b; fi; for i in 1 2; do echo $i; done; "
    "case $x in (a|b) echo y ;; (*) ;& esac; while false; do :; done; "
    "{ x } always { y }; [[ -n $x && $y == z* ]]; (( i++ )); "
    "a | b && c || d &; ls > f 2>&1");
  EXPECT_EQ(function_to_text("f", body),
            "f () {\n"
            "\techo a\n"
            "\tif true\n"
            "\tthen\n"
            "\t\tb\n"
            "\tfi\n"
            "\tfor i in 1 2\n"
            "\tdo\n"
            "\t\techo $i\n"
            "\tdone\n"
            "\tcase $x in\n"
            "\t\t(a|b) echo y ;;\n"
            "\t\t(*)  ;&\n"
            "\tesac\n"
            "\twhile false\n"
            "\tdo\n"
            "\t\t:\n"
            "\tdone\n"
            "\t{\n"
            "\t\tx\n"
            "\t} always {\n"
            "\t\ty\n"
            "\t}\n"
            "\t[[ -n $x && $y == z* ]]\n"
            "\t(( i++ ))\n"
            "\ta | b && c || d &\n"
            "\tls > f 2>&1\n"
            "}");
}

TEST(Text, ListAtTopLevel) {
  ast::List list = parse_ok("echo a; echo b");
  EXPECT_EQ(list_to_text(list), "echo a\necho b");
}

TEST(Text, IfElse) {
  ast::List list = parse_ok("if a; then b; elif c; then d; else e; fi");
  EXPECT_EQ(list_to_text(list),
            "if a\nthen\n\tb\nelif c\nthen\n\td\nelse\n\te\nfi");
}

TEST(Text, SubshellAndAssignments) {
  ast::List list = parse_ok("(x=1 y=(a b) cmd)");
  EXPECT_EQ(list_to_text(list), "(\n\tx=1 y=(a b) cmd\n)");
}

// zsh prints |& as an explicit 2>&1 before the pipe.
TEST(Text, PipelineSingleLine) {
  ast::List list = parse_ok("ls -l | grep x |& wc -l");
  EXPECT_EQ(pipeline_to_text(list.items[0].sublist.items[0].pipeline),
            "ls -l | grep x 2>&1 | wc -l");
}

TEST(Text, RoundTripReparses) {
  for (strv source :
       {"while true; do break; done", "for ((i=0; i<3; i++)) do echo; done",
        "repeat 2 echo x", "f() { g; }", "case a in (a) ;; esac",
        "cat <<< $x > out", "! true && false"}) {
    ast::List list = parse_ok(source);
    str text = list_to_text(list);
    auto again = parse(text);
    ASSERT_TRUE(again) << text;
    EXPECT_EQ(list_to_text(*again), text);
  }
}

}  // namespace
}  // namespace myzsh

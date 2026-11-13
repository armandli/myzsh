#include <gtest/gtest.h>

#include <parse/parser.h>

namespace myzsh {
namespace {

using namespace myzsh::ast;

List parse_ok(strv source) {
  auto result = parse(source);
  if (not result) {
    ADD_FAILURE() << "parse failed: " << result.error().message;
    return {};
  }
  return s::move(*result);
}

const Command& first_command(const List& list) {
  return list.items.at(0).sublist.items.at(0).pipeline.commands.at(0);
}

template<typename T>
const T& first_as(const List& list) {
  const Command& command = first_command(list);
  EXPECT_TRUE(s::holds_alternative<T>(command.node));
  return s::get<T>(command.node);
}

vec<str> word_texts(const vec<Word>& words) {
  vec<str> result;
  for (const Word& word : words) {
    result.push_back(word.text);
  }
  return result;
}

TEST(Parser, EmptyInput) {
  List list = parse_ok("");
  EXPECT_TRUE(list.items.empty());
  EXPECT_TRUE(parse_ok("\n\n  # only a comment\n").items.empty());
}

TEST(Parser, SimpleCommand) {
  List list = parse_ok("echo hello 'world'");
  const auto& simple = first_as<SimpleCommand>(list);
  EXPECT_EQ(word_texts(simple.words),
            (vec<str>{"echo", "hello", "'world'"}));
  EXPECT_TRUE(simple.assignments.empty());
}

TEST(Parser, AssignmentsBeforeCommand) {
  List list = parse_ok("A=1 B+=x cmd arg");
  const auto& simple = first_as<SimpleCommand>(list);
  ASSERT_EQ(simple.assignments.size(), 2u);
  EXPECT_EQ(simple.assignments[0].name, "A");
  EXPECT_EQ(simple.assignments[0].value.text, "1");
  EXPECT_FALSE(simple.assignments[0].append);
  EXPECT_EQ(simple.assignments[1].name, "B");
  EXPECT_TRUE(simple.assignments[1].append);
  EXPECT_EQ(word_texts(simple.words), (vec<str>{"cmd", "arg"}));
}

TEST(Parser, ArrayAssignment) {
  List list = parse_ok("arr=(one 'two three' $x)");
  const auto& simple = first_as<SimpleCommand>(list);
  ASSERT_EQ(simple.assignments.size(), 1u);
  EXPECT_TRUE(simple.assignments[0].is_array);
  EXPECT_EQ(word_texts(simple.assignments[0].elements),
            (vec<str>{"one", "'two three'", "$x"}));
  EXPECT_TRUE(simple.words.empty());
}

TEST(Parser, SubscriptedAssignment) {
  List list = parse_ok("a[2]=x h[key]=y");
  const auto& simple = first_as<SimpleCommand>(list);
  ASSERT_EQ(simple.assignments.size(), 2u);
  EXPECT_EQ(simple.assignments[0].name, "a");
  EXPECT_EQ(simple.assignments[0].subscript, "2");
  EXPECT_EQ(simple.assignments[1].subscript, "key");
}

TEST(Parser, TypesetKeepsAssignmentsAfterCommandWord) {
  List list = parse_ok("local a=1 b=(x y)");
  const auto& simple = first_as<SimpleCommand>(list);
  EXPECT_EQ(word_texts(simple.words), vec<str>{"local"});
  ASSERT_EQ(simple.assignments.size(), 2u);
  EXPECT_TRUE(simple.assignments[1].is_array);
}

TEST(Parser, Redirections) {
  List list = parse_ok("cmd <in >out 2>>err 3<&0 4>&- &>both");
  const auto& simple = first_as<SimpleCommand>(list);
  ASSERT_EQ(simple.redirects.size(), 6u);
  EXPECT_EQ(simple.redirects[0].type, RedirType::Read);
  EXPECT_EQ(simple.redirects[0].target.text, "in");
  EXPECT_EQ(simple.redirects[1].type, RedirType::Write);
  EXPECT_EQ(simple.redirects[2].type, RedirType::Append);
  EXPECT_EQ(simple.redirects[2].fd, 2);
  EXPECT_EQ(simple.redirects[3].type, RedirType::MergeIn);
  EXPECT_EQ(simple.redirects[3].fd, 3);
  EXPECT_EQ(simple.redirects[3].target.text, "0");
  EXPECT_EQ(simple.redirects[4].type, RedirType::Close);
  EXPECT_EQ(simple.redirects[5].type, RedirType::ErrWrite);
}

TEST(Parser, CshStyleErrorRedirection) {
  List list = parse_ok("cmd >& file");
  const auto& simple = first_as<SimpleCommand>(list);
  ASSERT_EQ(simple.redirects.size(), 1u);
  EXPECT_EQ(simple.redirects[0].type, RedirType::ErrWrite);
}

TEST(Parser, RedirectionWithFdVariable) {
  List list = parse_ok("exec {fd}>file");
  const auto& simple = first_as<SimpleCommand>(list);
  ASSERT_EQ(simple.redirects.size(), 1u);
  EXPECT_EQ(simple.redirects[0].fd_variable, "fd");
  EXPECT_EQ(word_texts(simple.words), vec<str>{"exec"});
}

TEST(Parser, HereDocument) {
  List list = parse_ok("cat <<EOF\nhello $x\nEOF\n");
  const auto& simple = first_as<SimpleCommand>(list);
  ASSERT_EQ(simple.redirects.size(), 1u);
  EXPECT_EQ(simple.redirects[0].type, RedirType::HereDoc);
  ASSERT_TRUE(simple.redirects[0].heredoc);
  EXPECT_EQ(simple.redirects[0].heredoc->body, "hello $x\n");
}

TEST(Parser, HereString) {
  List list = parse_ok("cat <<< \"$x\"");
  const auto& simple = first_as<SimpleCommand>(list);
  ASSERT_EQ(simple.redirects.size(), 1u);
  EXPECT_EQ(simple.redirects[0].type, RedirType::HereString);
  EXPECT_EQ(simple.redirects[0].target.text, "\"$x\"");
}

TEST(Parser, PipelinesAndSublists) {
  List list = parse_ok("a | b |& c && ! d || coproc e");
  ASSERT_EQ(list.items.size(), 1u);
  const Sublist& sublist = list.items[0].sublist;
  ASSERT_EQ(sublist.items.size(), 3u);
  EXPECT_EQ(sublist.items[0].pipeline.commands.size(), 3u);
  EXPECT_EQ(sublist.items[0].pipeline.pipe_stderr, (vec<bool>{false, true}));
  EXPECT_EQ(sublist.items[0].connector, Connector::And);
  EXPECT_TRUE(sublist.items[1].negated);
  EXPECT_EQ(sublist.items[1].connector, Connector::Or);
  EXPECT_TRUE(sublist.items[2].coproc);
  EXPECT_EQ(sublist.items[2].connector, Connector::None);
}

TEST(Parser, ListTerminators) {
  List list = parse_ok("a & b; c &| d &!\ne");
  ASSERT_EQ(list.items.size(), 5u);
  EXPECT_EQ(list.items[0].terminator, ListTerminator::Async);
  EXPECT_EQ(list.items[1].terminator, ListTerminator::Sync);
  EXPECT_EQ(list.items[2].terminator, ListTerminator::Disown);
  EXPECT_EQ(list.items[3].terminator, ListTerminator::Disown);
  EXPECT_EQ(list.items[4].terminator, ListTerminator::Sync);
}

TEST(Parser, PipelineContinuesAfterNewline) {
  List list = parse_ok("a |\n b &&\n c");
  ASSERT_EQ(list.items.size(), 1u);
  EXPECT_EQ(list.items[0].sublist.items.size(), 2u);
}

TEST(Parser, SubshellAndBraceGroup) {
  List list = parse_ok("(a; b) > f; { c; d } 2>&1");
  ASSERT_EQ(list.items.size(), 2u);
  const auto& subshell = first_as<Subshell>(list);
  EXPECT_EQ(subshell.body->items.size(), 2u);
  EXPECT_EQ(first_command(list).redirects.size(), 1u);
  const Command& brace =
    list.items[1].sublist.items[0].pipeline.commands[0];
  EXPECT_TRUE(s::holds_alternative<BraceGroup>(brace.node));
  EXPECT_EQ(brace.redirects.size(), 1u);
}

TEST(Parser, TryAlways) {
  List list = parse_ok("{ a } always { b }");
  const auto& node = first_as<Try>(list);
  EXPECT_EQ(node.body->items.size(), 1u);
  EXPECT_EQ(node.always->items.size(), 1u);
}

TEST(Parser, IfElifElse) {
  List list = parse_ok("if a; then b; elif c; then d; else e; fi");
  const auto& node = first_as<If>(list);
  EXPECT_EQ(node.clauses.size(), 2u);
  ASSERT_TRUE(node.else_body);
  EXPECT_EQ(node.else_body->items.size(), 1u);
}

TEST(Parser, IfShortForms) {
  List brace = parse_ok("if [[ -n x ]] { a } elif true { b } else { c }");
  EXPECT_EQ(first_as<If>(brace).clauses.size(), 2u);
  List shortform = parse_ok("if true; echo yes");
  EXPECT_EQ(first_as<If>(shortform).clauses.size(), 1u);
}

TEST(Parser, ForLoops) {
  List list = parse_ok("for i in a b c; do echo $i; done");
  const auto& node = first_as<For>(list);
  EXPECT_EQ(node.variables, vec<str>{"i"});
  ASSERT_TRUE(node.words);
  EXPECT_EQ(word_texts(*node.words), (vec<str>{"a", "b", "c"}));
}

TEST(Parser, ForWithoutInIteratesPositionals) {
  List list = parse_ok("for i; do :; done");
  EXPECT_FALSE(first_as<For>(list).words.has_value());
}

TEST(Parser, ForMultipleVariablesAndShortForms) {
  List multi = parse_ok("for k v in a 1 b 2; do :; done");
  EXPECT_EQ(first_as<For>(multi).variables, (vec<str>{"k", "v"}));
  List paren = parse_ok("for i (x y) echo $i");
  EXPECT_EQ(word_texts(*first_as<For>(paren).words), (vec<str>{"x", "y"}));
  List foreach = parse_ok("foreach i (x y z)\necho $i\nend");
  EXPECT_EQ(first_as<For>(foreach).words->size(), 3u);
}

TEST(Parser, ArithmeticFor) {
  List list = parse_ok("for ((i = 0; i < 3; i++)) { echo $i }");
  const auto& node = first_as<ArithFor>(list);
  EXPECT_EQ(node.init, "i = 0");
  EXPECT_EQ(node.condition, " i < 3");
  EXPECT_EQ(node.step, " i++");
}

TEST(Parser, SelectLoop) {
  List list = parse_ok("select x in a b; do break; done");
  const auto& node = first_as<Select>(list);
  EXPECT_EQ(node.variable, "x");
  EXPECT_EQ(node.words->size(), 2u);
}

TEST(Parser, WhileAndUntil) {
  List w = parse_ok("while a; do b; done");
  EXPECT_FALSE(first_as<While>(w).until);
  List u = parse_ok("until a; do b; done");
  EXPECT_TRUE(first_as<While>(u).until);
  List brace = parse_ok("while false { : }");
  EXPECT_FALSE(first_as<While>(brace).until);
}

TEST(Parser, Repeat) {
  List list = parse_ok("repeat 3 echo hi");
  EXPECT_EQ(first_as<Repeat>(list).count.text, "3");
}

TEST(Parser, Case) {
  List list = parse_ok(
    "case $x in\n"
    "  a|b) echo ab ;;\n"
    "  (c) echo c ;&\n"
    "  *) echo other ;|\n"
    "esac");
  const auto& node = first_as<Case>(list);
  EXPECT_EQ(node.subject.text, "$x");
  ASSERT_EQ(node.items.size(), 3u);
  EXPECT_EQ(word_texts(node.items[0].patterns), (vec<str>{"a", "b"}));
  EXPECT_EQ(node.items[0].terminator, CaseTerminator::Break);
  EXPECT_EQ(word_texts(node.items[1].patterns), vec<str>{"c"});
  EXPECT_EQ(node.items[1].terminator, CaseTerminator::FallThrough);
  EXPECT_EQ(node.items[2].terminator, CaseTerminator::TestNext);
}

TEST(Parser, CaseLastItemNeedsNoTerminator) {
  List list = parse_ok("case x in x) echo x\nesac");
  EXPECT_EQ(first_as<Case>(list).items.size(), 1u);
}

TEST(Parser, FunctionDefinitions) {
  List posix = parse_ok("f() { echo hi; }");
  const auto& f = first_as<FuncDef>(posix);
  EXPECT_EQ(word_texts(f.names), vec<str>{"f"});
  ASSERT_TRUE(f.body);
  List keyword = parse_ok("function g h { :; }");
  EXPECT_EQ(word_texts(first_as<FuncDef>(keyword).names),
            (vec<str>{"g", "h"}));
  List both = parse_ok("function k() echo x");
  EXPECT_EQ(word_texts(first_as<FuncDef>(both).names), vec<str>{"k"});
}

TEST(Parser, AnonymousFunction) {
  List list = parse_ok("() { echo $1 } a b");
  const auto& f = first_as<FuncDef>(list);
  EXPECT_TRUE(f.anonymous);
  EXPECT_EQ(word_texts(f.anonymous_args), (vec<str>{"a", "b"}));
}

TEST(Parser, CondExpression) {
  List list = parse_ok("[[ -f file && ( $a == b* || ! -z $c ) ]]");
  const auto& node = first_as<Cond>(list);
  ASSERT_TRUE(node.expression);
  EXPECT_EQ(node.expression->kind, CondKind::And);
  EXPECT_EQ(node.expression->left->kind, CondKind::Unary);
  EXPECT_EQ(node.expression->left->op, "-f");
  EXPECT_EQ(node.expression->right->kind, CondKind::Or);
  EXPECT_EQ(node.expression->right->left->kind, CondKind::Binary);
  EXPECT_EQ(node.expression->right->left->op, "==");
  EXPECT_EQ(node.expression->right->right->kind, CondKind::Not);
}

TEST(Parser, CondSingleWordAndRegex) {
  List single = parse_ok("[[ $x ]]");
  EXPECT_EQ(first_as<Cond>(single).expression->kind, CondKind::String);
  List regex = parse_ok("[[ abc =~ ^a(b)c$ ]]");
  EXPECT_EQ(first_as<Cond>(regex).expression->op, "=~");
}

TEST(Parser, Arithmetic) {
  List list = parse_ok("(( x = 1 + 2 ))");
  EXPECT_EQ(first_as<Arith>(list).expression, " x = 1 + 2 ");
}

TEST(Parser, Time) {
  List list = parse_ok("time a | b");
  const auto& node = first_as<Time>(list);
  ASSERT_TRUE(node.pipeline);
  EXPECT_EQ(node.pipeline->commands.size(), 2u);
  List bare = parse_ok("time");
  EXPECT_FALSE(first_as<Time>(bare).pipeline);
}

TEST(Parser, LineNumbersAreRecorded) {
  List list = parse_ok("a\n\nb");
  ASSERT_EQ(list.items.size(), 2u);
  EXPECT_EQ(first_command(list).line, 1);
  EXPECT_EQ(list.items[1].sublist.items[0].pipeline.commands[0].line, 3);
}

TEST(ParserErrors, UnexpectedToken) {
  auto result = parse("x=(; echo hi");
  ASSERT_FALSE(result);
  EXPECT_EQ(result.error().message, "parse error near `hi'");
  EXPECT_FALSE(result.error().incomplete);
}

TEST(ParserErrors, StrayKeyword) {
  auto result = parse("fi");
  ASSERT_FALSE(result);
  EXPECT_EQ(result.error().message, "parse error near `fi'");
}

TEST(ParserErrors, IncompleteConstructs) {
  for (strv source : {"if true; then", "while true; do", "echo 'abc",
                      "f() {", "case x in", "echo $(ls", "a &&", "a |"}) {
    auto result = parse(source);
    ASSERT_FALSE(result) << source;
    EXPECT_TRUE(result.error().incomplete) << source;
  }
}

TEST(ParserErrors, ReportsLine) {
  auto result = parse("echo ok\necho ok\n)");
  ASSERT_FALSE(result);
  EXPECT_EQ(result.error().line, 3);
}

TEST(Parser, ParseLineStopsAtNewline) {
  Lexer lexer("echo a; echo b\necho c\n");
  Parser parser(lexer);
  auto first = parser.parse_line();
  ASSERT_TRUE(first);
  ASSERT_TRUE(first->has_value());
  EXPECT_EQ((*first)->items.size(), 2u);
  auto second = parser.parse_line();
  ASSERT_TRUE(second);
  ASSERT_TRUE(second->has_value());
  EXPECT_EQ((*second)->items.size(), 1u);
  auto end = parser.parse_line();
  ASSERT_TRUE(end);
  EXPECT_FALSE(end->has_value());
}

}  // namespace
}  // namespace myzsh

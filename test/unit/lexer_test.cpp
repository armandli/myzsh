#include <gtest/gtest.h>

#include <parse/lexer.h>

namespace myzsh {
namespace {

using enum TokenType;

vec<TokenType> types(strv source) {
  vec<TokenType> result;
  for (const Token& token : tokenize(source)) {
    result.push_back(token.type);
  }
  return result;
}

vec<str> texts(strv source) {
  vec<str> result;
  for (const Token& token : tokenize(source)) {
    if (token.type == String or token.type == EnvString) {
      result.push_back(token.text);
    }
  }
  return result;
}

TEST(TokenHelpers, ReservedWords) {
  EXPECT_EQ(reserved_word("if"), If);
  EXPECT_EQ(reserved_word("then"), Then);
  EXPECT_EQ(reserved_word("{"), InBrace);
  EXPECT_EQ(reserved_word("[["), DInBrack);
  EXPECT_EQ(reserved_word("!"), Bang);
  EXPECT_EQ(reserved_word("foreach"), Foreach);
  EXPECT_EQ(reserved_word("typeset"), Typeset);
  EXPECT_EQ(reserved_word("local"), Typeset);
  EXPECT_EQ(reserved_word("echo"), s::nullopt);
}

TEST(TokenHelpers, IsRedirection) {
  EXPECT_TRUE(is_redirection(OutAng));
  EXPECT_TRUE(is_redirection(TrInAng));
  EXPECT_TRUE(is_redirection(DInAngDash));
  EXPECT_FALSE(is_redirection(Bar));
  EXPECT_FALSE(is_redirection(String));
}

TEST(TokenHelpers, NamesAreNonEmpty) {
  EXPECT_FALSE(token_type_name(String).empty());
  EXPECT_FALSE(token_type_name(DSemi).empty());
}

TEST(Lexer, SimpleWords) {
  EXPECT_EQ(types("echo hello world"),
            (vec<TokenType>{String, String, String, EndInput}));
  EXPECT_EQ(texts("echo hello world"),
            (vec<str>{"echo", "hello", "world"}));
}

TEST(Lexer, Separators) {
  EXPECT_EQ(types("a; b & c\n"),
            (vec<TokenType>{String, Semi, String, Amper, String, Newline,
                            EndInput}));
}

TEST(Lexer, ListOperators) {
  EXPECT_EQ(types("a && b || c | d |& e"),
            (vec<TokenType>{String, DAmper, String, DBar, String, Bar, String,
                            BarAmp, String, EndInput}));
}

TEST(Lexer, DisownOperators) {
  EXPECT_EQ(types("a &| b &!"),
            (vec<TokenType>{String, AmperBang, String, AmperBang, EndInput}));
}

TEST(Lexer, RedirectionOperators) {
  EXPECT_EQ(types("a > f >> f < f <> f >| f >! f"),
            (vec<TokenType>{String, OutAng, String, DOutAng, String, InAng,
                            String, InOutAng, String, OutAngBang, String,
                            OutAngBang, String, EndInput}));
  EXPECT_EQ(types("a <<< s >& f &> f >>& f &>> f <& 3"),
            (vec<TokenType>{String, TrInAng, String, OutAngAmp, String,
                            AmpOutAng, String, DOutAngAmp, String, DOutAngAmp,
                            String, InAngAmp, String, EndInput}));
}

TEST(Lexer, FdNumberBeforeRedirection) {
  vec<Token> tokens = tokenize("cmd 2>err 10<in");
  ASSERT_GE(tokens.size(), 6u);
  EXPECT_EQ(tokens[1].type, OutAng);
  EXPECT_EQ(tokens[1].fd, 2);
  EXPECT_EQ(tokens[3].type, InAng);
  EXPECT_EQ(tokens[3].fd, 10);
}

TEST(Lexer, DigitsNotAdjacentToRedirectionAreWords) {
  vec<Token> tokens = tokenize("echo 2 >f");
  ASSERT_GE(tokens.size(), 4u);
  EXPECT_EQ(tokens[1].type, String);
  EXPECT_EQ(tokens[1].text, "2");
  EXPECT_EQ(tokens[2].fd, -1);
}

TEST(Lexer, ReservedWordsOnlyInCommandPosition) {
  EXPECT_EQ(types("if true; then echo if; fi"),
            (vec<TokenType>{If, String, Semi, Then, String, String, Semi, Fi,
                            EndInput}));
}

TEST(Lexer, CaseTerminators) {
  EXPECT_EQ(types("case x in a) b;; c) d;& e) f;| esac"),
            (vec<TokenType>{Case, String, String, String, OutPar, String,
                            DSemi, String, OutPar, String, SemiAmp, String,
                            OutPar, String, SemiBar, Esac, EndInput}));
}

TEST(Lexer, QuotedWordsStayWhole) {
  EXPECT_EQ(texts("echo 'a b' \"c d\" e\\ f $'g h'"),
            (vec<str>{"echo", "'a b'", "\"c d\"", "e\\ f", "$'g h'"}));
}

TEST(Lexer, SubstitutionsStayInsideWords) {
  EXPECT_EQ(texts("echo $(a b) ${x:-y z} $((1 + 2)) `c d` x$(e)y"),
            (vec<str>{"echo", "$(a b)", "${x:-y z}", "$((1 + 2))", "`c d`",
                      "x$(e)y"}));
}

TEST(Lexer, NestedCommandSubstitutionWithParentheses) {
  EXPECT_EQ(texts("echo $(case x in (a) echo ')' ;; esac)"),
            (vec<str>{"echo", "$(case x in (a) echo ')' ;; esac)"}));
}

TEST(Lexer, ProcessSubstitutionIsAWord) {
  EXPECT_EQ(texts("diff <(ls a) >(cat) =(ls)"),
            (vec<str>{"diff", "<(ls a)", ">(cat)", "=(ls)"}));
}

TEST(Lexer, GlobParenthesesStayInWord) {
  EXPECT_EQ(texts("ls (a|b).c *(.)"),
            (vec<str>{"ls", "(a|b).c", "*(.)"}));
}

TEST(Lexer, Assignments) {
  EXPECT_EQ(types("a=1 b+=2 c=(x y) cmd"),
            (vec<TokenType>{EnvString, EnvString, EnvArray, String, String,
                            OutPar, String, EndInput}));
}

TEST(Lexer, AssignmentOnlyBeforeCommandWord) {
  EXPECT_EQ(types("echo a=1"), (vec<TokenType>{String, String, EndInput}));
}

TEST(Lexer, FunctionDefinitionParens) {
  EXPECT_EQ(types("f() { :; }"),
            (vec<TokenType>{String, InOutPar, InBrace, String, Semi, OutBrace,
                            EndInput}));
}

TEST(Lexer, DoubleParenAndBracket) {
  EXPECT_EQ(types("(( x )) && [[ a ]]"),
            (vec<TokenType>{DInPar, String, DOutPar, DAmper, DInBrack, String,
                            DOutBrack, EndInput}));
}

TEST(Lexer, Comments) {
  EXPECT_EQ(texts("echo a # comment here\necho b#notcomment"),
            (vec<str>{"echo", "a", "echo", "b#notcomment"}));
}

TEST(Lexer, BackslashNewlineJoinsLines) {
  EXPECT_EQ(texts("echo a\\\nb"), (vec<str>{"echo", "ab"}));
}

TEST(Lexer, LineNumbers) {
  vec<Token> tokens = tokenize("a\nb\n\nc");
  ASSERT_GE(tokens.size(), 5u);
  EXPECT_EQ(tokens[0].line, 1);
  EXPECT_EQ(tokens[2].line, 2);
  EXPECT_EQ(tokens.back().type, EndInput);
  EXPECT_EQ(tokens[tokens.size() - 2].line, 4);
}

TEST(Lexer, UnterminatedQuoteIsIncomplete) {
  Lexer lexer("echo 'abc");
  lexer.next();
  Token token = lexer.next();
  EXPECT_EQ(token.type, LexErr);
  EXPECT_TRUE(lexer.incomplete());
}

TEST(Lexer, UnterminatedBraceExpansionIsError) {
  Lexer lexer("echo ${x");
  lexer.next();
  EXPECT_EQ(lexer.next().type, LexErr);
}

TEST(Lexer, HereDocumentBodyIsReadAfterNewline) {
  auto doc = s::make_shared<ast::HereDoc>();
  Lexer lexer("cat <<EOF\nline 1\n$x\nEOF\necho after\n");
  EXPECT_EQ(lexer.next().type, String);
  EXPECT_EQ(lexer.next().type, DInAng);
  Token terminator = lexer.next();
  EXPECT_EQ(terminator.text, "EOF");
  lexer.queue_heredoc(doc, terminator.text, false);
  EXPECT_EQ(lexer.next().type, Newline);
  EXPECT_EQ(doc->body, "line 1\n$x\n");
  EXPECT_FALSE(doc->quoted);
  Token after = lexer.next();
  EXPECT_EQ(after.text, "echo");
}

TEST(Lexer, QuotedHereDocTerminatorAndTabStripping) {
  auto doc = s::make_shared<ast::HereDoc>();
  Lexer lexer("cat <<-'EOF'\n\tindented $x\n\tEOF\n");
  lexer.next();
  lexer.next();
  Token terminator = lexer.next();
  lexer.queue_heredoc(doc, terminator.text, true);
  lexer.next();
  EXPECT_EQ(doc->body, "indented $x\n");
  EXPECT_TRUE(doc->quoted);
}

TEST(Lexer, AliasExpansionInCommandPosition) {
  Lexer lexer("ll -a");
  lexer.set_alias_expander([](strv word, bool command_position) -> opt<str> {
    if (word == "ll" and command_position) {
      return str("ls -l");
    }
    return s::nullopt;
  });
  EXPECT_EQ(lexer.next().text, "ls");
  EXPECT_EQ(lexer.next().text, "-l");
  EXPECT_EQ(lexer.next().text, "-a");
}

TEST(Lexer, RecursiveAliasExpandsOnce) {
  Lexer lexer("ls x");
  lexer.set_alias_expander([](strv word, bool) -> opt<str> {
    if (word == "ls") {
      return str("ls -F");
    }
    return s::nullopt;
  });
  EXPECT_EQ(lexer.next().text, "ls");
  EXPECT_EQ(lexer.next().text, "-F");
  EXPECT_EQ(lexer.next().text, "x");
}

TEST(Lexer, ArithmeticIsOneStringUpToMatchingParens) {
  vec<Token> tokens = tokenize("(( (a + b) * 2 )) rest");
  ASSERT_EQ(tokens.size(), 5u);
  EXPECT_EQ(tokens[0].type, DInPar);
  EXPECT_EQ(tokens[1].type, String);
  EXPECT_EQ(tokens[1].text, " (a + b) * 2 ");
  EXPECT_EQ(tokens[2].type, DOutPar);
  EXPECT_EQ(tokens[3].text, "rest");
}

TEST(Lexer, ArithmeticForHeader) {
  vec<Token> tokens = tokenize("for ((i=0; i<3; i++)) echo $i");
  ASSERT_GE(tokens.size(), 4u);
  EXPECT_EQ(tokens[0].type, For);
  EXPECT_EQ(tokens[1].type, DInPar);
  EXPECT_EQ(tokens[2].text, "i=0; i<3; i++");
  EXPECT_EQ(tokens[3].type, DOutPar);
}

TEST(Lexer, CondModeTreatsAngleBracketsAsWords) {
  vec<Token> tokens = tokenize("[[ a < b ]]");
  ASSERT_EQ(tokens.size(), 6u);
  EXPECT_EQ(tokens[2].type, String);
  EXPECT_EQ(tokens[2].text, "<");
  EXPECT_EQ(tokens[4].type, DOutBrack);
}

TEST(Lexer, PeekDoesNotConsume) {
  Lexer lexer("a b");
  EXPECT_EQ(lexer.peek().text, "a");
  EXPECT_EQ(lexer.next().text, "a");
  EXPECT_EQ(lexer.next().text, "b");
  EXPECT_TRUE(lexer.at_end() or lexer.peek().type == EndInput);
}

}  // namespace
}  // namespace myzsh

#ifndef AST_H
#define AST_H

#include <variant>

#include <util/aliases.h>

// Tree representation of a parsed program. zsh compiles to flat "wordcode"
// (Src/parse.c) for compactness and zcompile; a tree is used here instead
// because it is easier to build, inspect and test. Words stay as raw source
// text and are parsed by the expander at execution time, as zsh does.
namespace myzsh::ast {

struct List;
struct Pipeline;
struct CondNode;
using ListPtr = uptr<List>;

struct Word {
  str text;
  int line = 0;

  bool operator==(const Word&) const = default;
};

struct Assignment {
  str name;
  // Raw text of [subscript] in "a[sub]=v", empty if none.
  str subscript;
  bool append = false;   // +=
  bool is_array = false; // name=( ... )
  Word value;            // scalar form
  vec<Word> elements;    // array form
};

enum class RedirType : int {
  Write,        // >
  WriteNow,     // >| >!
  Append,       // >>
  AppendNow,    // >>| >>!
  ErrWrite,     // &> >&
  ErrWriteNow,  // >&| &>|
  ErrAppend,    // >>& &>>
  ErrAppendNow, // >>&| &>>|
  ReadWrite,    // <>
  Read,         // <
  HereDoc,      // <<
  HereDocDash,  // <<-
  HereString,   // <<<
  MergeIn,      // <&n
  MergeOut,     // >&n
  Close,        // <&- >&-
  InPipe,       // < <(...)
  OutPipe,      // > >(...)
};

struct HereDoc {
  // Filled in by the lexer when it reaches the end of the line containing <<.
  str body;
  // Terminator was quoted: body is used literally, no expansion.
  bool quoted = false;
};

struct Redirect {
  RedirType type = RedirType::Write;
  int fd = -1;        // -1: default for the operator (0 or 1)
  str fd_variable;    // {var}>file form, empty if none
  Word target;        // file name, fd number, or here-doc terminator
  sptr<HereDoc> heredoc;
};

struct SimpleCommand {
  vec<Assignment> assignments;
  vec<Word> words;
  vec<Redirect> redirects;
};

struct Subshell {
  ListPtr body;
};

struct BraceGroup {
  ListPtr body;
};

// { try } always { always }
struct Try {
  ListPtr body;
  ListPtr always;
};

struct For {
  vec<str> variables; // zsh allows "for a b c in ..."
  // nullopt means no "in" clause: iterate over "$@".
  opt<vec<Word>> words;
  ListPtr body;
};

// for (( init; cond; step ))
struct ArithFor {
  str init;
  str condition;
  str step;
  ListPtr body;
};

struct Select {
  str variable;
  opt<vec<Word>> words;
  ListPtr body;
};

struct While {
  bool until = false;
  ListPtr condition;
  ListPtr body;
};

struct Repeat {
  Word count;
  ListPtr body;
};

struct IfClause {
  ListPtr condition;
  ListPtr body;
};

struct If {
  vec<IfClause> clauses; // if + elifs
  ListPtr else_body;     // may be null
};

enum class CaseTerminator : int {
  Break,       // ;;
  FallThrough, // ;&
  TestNext,    // ;|
};

struct CaseItem {
  vec<Word> patterns;
  ListPtr body;
  CaseTerminator terminator = CaseTerminator::Break;
};

struct Case {
  Word subject;
  vec<CaseItem> items;
};

struct FuncDef {
  vec<Word> names;
  // Shared with the function table, which outlives the parse tree.
  sptr<const List> body;
  // "() { ... } args": anonymous function run immediately.
  bool anonymous = false;
  vec<Word> anonymous_args;
};

enum class CondKind : int {
  And,
  Or,
  Not,
  Unary,  // -f file, -n str, ...
  Binary, // a = b, a -nt b, ...
  String, // [[ word ]]  (non-empty test)
};

struct CondNode {
  CondKind kind = CondKind::String;
  str op;                 // "-f", "==", "=~", "-nt", ...
  vec<Word> operands;
  uptr<CondNode> left;    // And/Or/Not
  uptr<CondNode> right;   // And/Or
};

struct Cond {
  uptr<CondNode> expression;
};

// (( expression ))
struct Arith {
  str expression;
};

struct Time {
  uptr<Pipeline> pipeline; // null for a bare `time`
};

using CommandNode = s::variant<
  SimpleCommand,
  Subshell,
  BraceGroup,
  Try,
  For,
  ArithFor,
  Select,
  While,
  Repeat,
  If,
  Case,
  FuncDef,
  Cond,
  Arith,
  Time>;

struct Command {
  CommandNode node;
  // Redirections attached to a compound command: "{ ... } > file".
  vec<Redirect> redirects;
  int line = 0;
};

struct Pipeline {
  vec<Command> commands;
  // pipe_stderr[i]: commands[i] is joined to commands[i+1] by |& (size is
  // commands.size() - 1).
  vec<bool> pipe_stderr;
};

enum class Connector : int {
  None, // last pipeline of the sublist
  And,  // &&
  Or,   // ||
};

struct SublistItem {
  Pipeline pipeline;
  bool negated = false; // ! pipeline
  bool coproc = false;  // coproc pipeline
  Connector connector = Connector::None;
};

struct Sublist {
  vec<SublistItem> items;
};

enum class ListTerminator : int {
  Sync,   // ; or newline
  Async,  // &
  Disown, // &| &!
};

struct ListItem {
  Sublist sublist;
  ListTerminator terminator = ListTerminator::Sync;
};

struct List {
  vec<ListItem> items;
};

}  // namespace myzsh::ast

#endif  // AST_H

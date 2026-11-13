#include <gtest/gtest.h>

#include <exec/functions.h>
#include <parse/parser.h>
#include <support/test_support.h>

namespace myzsh {
namespace {

sptr<const ast::List> body(strv source) {
  auto list = parse(source);
  EXPECT_TRUE(list);
  return s::make_shared<const ast::List>(s::move(*list));
}

TEST(FunctionTable, DefineFindRemove) {
  FunctionTable table;
  table.define({"f", body("echo hi"), FF_NONE, "", 0});
  auto found = table.find("f");
  ASSERT_TRUE(found);
  EXPECT_EQ(found->get().name, "f");
  EXPECT_TRUE(table.remove("f"));
  EXPECT_FALSE(table.find("f"));
  EXPECT_FALSE(table.remove("f"));
}

TEST(FunctionTable, RedefinitionReplaces) {
  FunctionTable table;
  auto first = body("echo 1");
  auto second = body("echo 2");
  table.define({"f", first, FF_NONE, "", 0});
  table.define({"f", second, FF_NONE, "", 0});
  EXPECT_EQ(table.find("f")->get().body, second);
}

TEST(FunctionTable, NamesSorted) {
  FunctionTable table;
  table.define({"zeta", body(":"), FF_NONE, "", 0});
  table.define({"alpha", body(":"), FF_NONE, "", 0});
  EXPECT_EQ(table.names(), (vec<str>{"alpha", "zeta"}));
}

TEST(FunctionTable, AutoloadStubs) {
  FunctionTable table;
  table.mark_autoload("lazy", FF_NO_ALIAS);
  auto stub = table.find("lazy");
  ASSERT_TRUE(stub);
  EXPECT_TRUE(stub->get().flags & FF_AUTOLOAD);
  EXPECT_TRUE(stub->get().flags & FF_NO_ALIAS);
  EXPECT_FALSE(stub->get().body);
  table.define({"defined", body(":"), FF_NONE, "", 0});
  table.mark_autoload("defined", FF_NONE);
  EXPECT_FALSE(table.find("defined")->get().flags & FF_AUTOLOAD);
}

TEST(FunctionTable, Flags) {
  FunctionTable table;
  table.define({"f", body(":"), FF_NONE, "", 0});
  table.set_flags("f", FF_TRACE, FF_NONE);
  EXPECT_TRUE(table.find("f")->get().flags & FF_TRACE);
  table.set_flags("f", FF_NONE, FF_TRACE);
  EXPECT_FALSE(table.find("f")->get().flags & FF_TRACE);
}

TEST(LoadAutoload, ZshStyleFileIsTheBody) {
  test::TempDir dir;
  dir.write("fns/greet", "echo hello $1\n");
  auto result = load_autoload("greet", {(dir / "missing").string(),
                                        (dir / "fns").string()});
  ASSERT_TRUE(result);
  EXPECT_FALSE(result->ksh_style);
  EXPECT_EQ(result->filename, (dir / "fns/greet").string());
  ASSERT_TRUE(result->body);
  EXPECT_EQ(result->body->items.size(), 1u);
}

TEST(LoadAutoload, KshStyleFileDefinesTheFunction) {
  test::TempDir dir;
  dir.write("greet", "greet() { echo hi; }\n");
  auto result = load_autoload("greet", {dir.path().string()});
  ASSERT_TRUE(result);
  EXPECT_TRUE(result->ksh_style);
}

TEST(LoadAutoload, NotFound) {
  test::TempDir dir;
  auto result = load_autoload("nothing", {dir.path().string()});
  ASSERT_FALSE(result);
  EXPECT_EQ(result.error(), "function definition file not found");
}

}  // namespace
}  // namespace myzsh

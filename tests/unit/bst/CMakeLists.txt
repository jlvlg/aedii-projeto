add_executable(bst bst.c)
target_link_libraries(bst Tree Types Util BST)

add_test(NAME bst-insert COMMAND bst 1 int 50 3)
set_tests_properties(bst-insert PROPERTIES PASS_REGULAR_EXPRESSION "50")

add_test(NAME bst-multiple-insert COMMAND bst 1 int 50 1 int 25 1 int 75 1 int 100 1 int 0 3)
set_tests_properties(bst-multiple-insert PROPERTIES PASS_REGULAR_EXPRESSION "50 25 0 75 100")

add_test(NAME bst-duplicate-insert COMMAND bst 1 int 50 1 int 25 1 int 75 1 int 50 1 int 25 1 int 2 1 int 1 1 int 3 1 int 2 3)
set_tests_properties(bst-duplicate-insert PROPERTIES PASS_REGULAR_EXPRESSION "50 25 25 2 1 2 3 50 75")

add_test(NAME bst-int-char COMMAND bst 1 int 5 1 int 2 1 char a 1 int 10 1 char c 1 char b 1 int 1 3)
set_tests_properties(bst-int-char PROPERTIES PASS_REGULAR_EXPRESSION "5 2 1 a 10 c b")

add_test(NAME bst-int-str COMMAND bst 1 int 5 1 str abc 1 str aaa 1 str 1ab 1 str 25 1 int 25 3)
set_tests_properties(bst-int-str PROPERTIES PASS_REGULAR_EXPRESSION "5 1ab 25 abc aaa 25")

add_test(NAME bst-char-str COMMAND bst 1 char a 1 char 4 1 str 1ab 1 str aaa 1 str 111 1 char n 3)
set_tests_properties(bst-char-str PROPERTIES PASS_REGULAR_EXPRESSION "a 4 1ab 111 aaa n")

add_test(NAME bst-remove-no-children COMMAND bst 1 int 50 1 int 25 1 int 75 1 int 100 1 int 0 2 int 0 3)
set_tests_properties(bst-remove-no-children PROPERTIES PASS_REGULAR_EXPRESSION "50 25 75 100")

add_test(NAME bst-remove-left-children COMMAND bst 1 int 50 1 int 25 1 int 75 1 int 100 1 int 0 2 int 25 3)
set_tests_properties(bst-remove-left-children PROPERTIES PASS_REGULAR_EXPRESSION "50 0 75 100" FAIL_REGULAR_EXPRESSION "25")

add_test(NAME bst-remove-right-children COMMAND bst 1 int 50 1 int 25 1 int 75 1 int 100 1 int 0 2 int 75 3)
set_tests_properties(bst-remove-right-children PROPERTIES PASS_REGULAR_EXPRESSION "50 25 0 100" FAIL_REGULAR_EXPRESSION "75")

add_test(NAME bst-remove-both-children COMMAND bst 1 int 50 1 int 25 1 int 75 1 int 100 1 int 0 2 int 50 3)
set_tests_properties(bst-remove-both-children PROPERTIES PASS_REGULAR_EXPRESSION "25 0 75 100" FAIL_REGULAR_EXPRESSION "50")

add_test(NAME bst-remove-all-nodes COMMAND bst 1 int 50 1 int 25 1 int 75 1 int 100 1 int 0 2 int 50 2 int 25 2 int 0 2 int 75 2 int 100 3)
set_tests_properties(bst-remove-all-nodes PROPERTIES FAIL_REGULAR_EXPRESSION "0;25;50;75;100")

add_test(NAME bst-remove-from-empty COMMAND bst 1 int 1000 2 int 1000 2 int 0 3)
set_tests_properties(bst-remove-from-empty PROPERTIES PASS_REGULAR_EXPRESSION "\n" FAIL_REGULAR_EXPRESSION "1000;0")

add_test(NAME bst-remove-then-insert COMMAND bst 1 int 50 1 int 25 1 int 75 1 int 37 1 int 62 1 int 100 1 int 0 2 int 50 2 int 62 2 int 100 2 int 0 1 int 35 1 int 47 1 int 96 3)
set_tests_properties(bst-remove-then-insert PROPERTIES PASS_REGULAR_EXPRESSION "37 25 35 75 47 96")

add_test(NAME bst-remove-duplicate COMMAND bst 1 int 50 1 int 75 1 str 50 1 int 50 1 str 75 2 str 50 2 int 50 2 int 75 3)
set_tests_properties(bst-remove-duplicate PROPERTIES PASS_REGULAR_EXPRESSION "50 75")
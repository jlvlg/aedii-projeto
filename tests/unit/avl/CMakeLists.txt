add_executable(avl avl.c)
target_link_libraries(avl AVL)

add_test(NAME avl-insert COMMAND avl 1 int 50 3)
set_tests_properties(avl-insert PROPERTIES PASS_REGULAR_EXPRESSION "50:0")

add_test(NAME avl-multiple-insert COMMAND avl 1 int 50 1 int 25 1 int 75 1 int 100 1 int 0 3)
set_tests_properties(avl-multiple-insert PROPERTIES PASS_REGULAR_EXPRESSION "50:0 25:-1 0:0 75:1 100:0")

add_test(NAME avl-int-char COMMAND avl 1 int 5 1 int 2 1 char a 1 int 10 1 char c 1 char b 1 int 1 3)
set_tests_properties(avl-int-char PROPERTIES PASS_REGULAR_EXPRESSION "a:-1 5:-1 2:-1 1:0 10:0 c:-1 b:0")

add_test(NAME avl-int-str COMMAND avl 1 int 5 1 str abc 1 str aaa 1 str 1ab 1 str 25 3)
set_tests_properties(avl-int-str PROPERTIES PASS_REGULAR_EXPRESSION "aaa:-1 25:0 1ab:0 5:0 abc:0")

add_test(NAME avl-char-str COMMAND avl 1 char a 1 char 4 1 str 1ab 1 str aaa 1 str 111 1 char n 3)
set_tests_properties(avl-char-str PROPERTIES PASS_REGULAR_EXPRESSION "4:0 1ab:-1 111:0 aaa:0 a:0 n:0")

add_test(NAME avl-remove-no-children COMMAND avl 1 int 50 1 int 25 1 int 75 1 int 100 1 int 0 2 int 0 3)
set_tests_properties(avl-remove-no-children PROPERTIES PASS_REGULAR_EXPRESSION "50:1 25:0 75:1 100:0")

add_test(NAME avl-remove-left-children COMMAND avl 1 int 50 1 int 25 1 int 75 1 int 100 1 int 0 2 int 25 3)
set_tests_properties(avl-remove-left-children PROPERTIES PASS_REGULAR_EXPRESSION "50:1 0:0 75:1 100:0" FAIL_REGULAR_EXPRESSION "25")

add_test(NAME avl-remove-right-children COMMAND avl 1 int 50 1 int 25 1 int 75 1 int 100 1 int 0 2 int 75 3)
set_tests_properties(avl-remove-right-children PROPERTIES PASS_REGULAR_EXPRESSION "50:-1 25:-1 0:0 100:0" FAIL_REGULAR_EXPRESSION "75")

add_test(NAME avl-remove-both-children COMMAND avl 1 int 50 1 int 25 1 int 75 1 int 100 1 int 0 2 int 50 3)
set_tests_properties(avl-remove-both-children PROPERTIES PASS_REGULAR_EXPRESSION "25:1 0:0 75:1 100:0" FAIL_REGULAR_EXPRESSION "50")

add_test(NAME avl-remove-all-nodes COMMAND avl 1 int 50 1 int 25 1 int 75 1 int 100 1 int 0 2 int 50 2 int 25 2 int 0 2 int 75 2 int 100 3)
set_tests_properties(avl-remove-all-nodes PROPERTIES FAIL_REGULAR_EXPRESSION "0;25;50;75;100")

add_test(NAME avl-remove-from-empty COMMAND avl 1 int 1000 2 int 1000 2 int 0 3)
set_tests_properties(avl-remove-from-empty PROPERTIES PASS_REGULAR_EXPRESSION "\n" FAIL_REGULAR_EXPRESSION "1000;0")

add_test(NAME avl-remove-then-insert COMMAND avl 1 int 50 1 int 25 1 int 75 1 int 37 1 int 62 1 int 100 1 int 0 2 int 50 2 int 62 2 int 100 2 int 0 1 int 35 1 int 47 1 int 96 3)
set_tests_properties(avl-remove-then-insert PROPERTIES PASS_REGULAR_EXPRESSION "37:0 25:1 35:0 75:0 47:0 96:0")

add_test(NAME avl-balance-l-remove COMMAND avl 1 int 10 1 int 5 1 int 15 1 int 7 2 int 15 4)
set_tests_properties(avl-balance-l-remove PROPERTIES PASS_REGULAR_EXPRESSION "2")

add_test(NAME avl-balance-r-remove COMMAND avl 1 int 10 1 int 5 1 int 15 1 int 13 2 int 5 4)
set_tests_properties(avl-balance-r-remove PROPERTIES PASS_REGULAR_EXPRESSION "2")

add_test(NAME avl-balance-l0-remove COMMAND avl 1 int 10 1 int 5 1 int 15 1 int 0 1 int 7 2 int 15 4)
set_tests_properties(avl-balance-l0-remove PROPERTIES PASS_REGULAR_EXPRESSION "3")

add_test(NAME avl-balance-r0-remove COMMAND avl 1 int 10 1 int 5 1 int 15 1 int 13 1 int 20 2 int 5 4)
set_tests_properties(avl-balance-r0-remove PROPERTIES PASS_REGULAR_EXPRESSION "3")

add_test(NAME avl-slr-insert COMMAND avl 1 int 10 1 int 15 1 int 20 3)
set_tests_properties(avl-slr-insert PROPERTIES PASS_REGULAR_EXPRESSION "15:0 10:0 20:0")

add_test(NAME avl-slr-remove COMMAND avl 1 int 10 1 int 5 1 int 15 1 int 20 2 int 5 3)
set_tests_properties(avl-slr-remove PROPERTIES PASS_REGULAR_EXPRESSION "15:0 10:0 20:0")

add_test(NAME avl-slr-0-remove COMMAND avl 1 int 10 1 int 15 1 int 5 1 int 13 1 int 20 2 int 5 3)
set_tests_properties(avl-slr-0-remove PROPERTIES PASS_REGULAR_EXPRESSION "15:-1 10:1 13:0 20:0")

add_test(NAME avl-srr-insert COMMAND avl 1 int 10 1 int 5 1 int 0 3)
set_tests_properties(avl-srr-insert PROPERTIES PASS_REGULAR_EXPRESSION "5:0 0:0 10:0")

add_test(NAME avl-srr-remove COMMAND avl 1 int 10 1 int 5 1 int 15 1 int 0 2 int 15 3)
set_tests_properties(avl-srr-remove PROPERTIES PASS_REGULAR_EXPRESSION "5:0 0:0 10:0")

add_test(NAME avl-srr-0-remove COMMAND avl 1 int 10 1 int 5 1 int 15 1 int 0 1 int 7 2 int 15 3)
set_tests_properties(avl-srr-0-remove PROPERTIES PASS_REGULAR_EXPRESSION "5:1 0:0 10:-1 7:0")

add_test(NAME avl-dlr--1-insert COMMAND avl 1 int 10 1 int 5 1 int 16 1 int 6 1 int 13 1 int 20 1 int 12 1 int 14 1 int 22 1 int 11 3)
set_tests_properties(avl-dlr--1-insert PROPERTIES PASS_REGULAR_EXPRESSION "13:0 10:0 5:1 6:0 12:-1 11:0 16:1 14:0 20:1 22:0")

add_test(NAME avl-dlr-1-insert COMMAND avl 1 int 10 1 int 5 1 int 16 1 int 6 1 int 13 1 int 20 1 int 12 1 int 14 1 int 22 1 int 15 3)
set_tests_properties(avl-dlr-1-insert PROPERTIES PASS_REGULAR_EXPRESSION "13:0 10:-1 5:1 6:0 12:0 16:0 14:1 15:0 20:1 22:0")

add_test(NAME avl-dlr--1-remove COMMAND avl 1 int 10 1 int 5 1 int 16 1 int 6 1 int 13 1 int 20 1 int 12 2 int 6 3)
set_tests_properties(avl-dlr--1-remove PROPERTIES PASS_REGULAR_EXPRESSION "13:0 10:0 5:0 12:0 16:1 20:0")

add_test(NAME avl-dlr-0-remove COMMAND avl 1 int 10 1 int 5 1 int 16 1 int 6 1 int 13 1 int 20 1 int 14 2 int 6 3)
set_tests_properties(avl-dlr-0-remove PROPERTIES PASS_REGULAR_EXPRESSION "13:0 10:-1 5:0 16:0 14:0 20:0")

add_test(NAME avl-dlr-1-remove COMMAND avl 1 int 10 1 int 5 1 int 16 1 int 6 1 int 13 1 int 20 1 int 12 1 int 14 2 int 6 3)
set_tests_properties(avl-dlr-1-remove PROPERTIES PASS_REGULAR_EXPRESSION "13:0 10:0 5:0 12:0 16:0 14:0 20:0")

add_test(NAME avl-drr--1-insert COMMAND avl 1 int 10 1 int 4 1 int 15 1 int 0 1 int 7 1 int 13 1 int 6 1 int 8 1 int 3 1 int 5 3)
set_tests_properties(avl-drr--1-insert PROPERTIES PASS_REGULAR_EXPRESSION "7:0 4:0 0:1 3:0 6:-1 5:0 10:1 8:0 15:-1 13:0")

add_test(NAME avl-drr-1-insert COMMAND avl 1 int 10 1 int 4 1 int 15 1 int 0 1 int 7 1 int 13 1 int 6 1 int 8 1 int 3 1 int 9 3)
set_tests_properties(avl-drr-1-insert PROPERTIES PASS_REGULAR_EXPRESSION "7:0 4:-1 0:1 3:0 6:0 10:0 8:1 9:0 15:-1 13:0")

add_test(NAME avl-drr--1-remove COMMAND avl 1 int 10 1 int 4 1 int 15 1 int 0 1 int 7 1 int 13 1 int 6 2 int 13 3)
set_tests_properties(avl-drr--1-remove PROPERTIES PASS_REGULAR_EXPRESSION "7:0 4:0 0:0 6:0 10:1 15:0")

add_test(NAME avl-drr-0-remove COMMAND avl 1 int 10 1 int 4 1 int 15 1 int 0 1 int 7 1 int 13 1 int 8 2 int 13 3)
set_tests_properties(avl-drr-0-remove PROPERTIES PASS_REGULAR_EXPRESSION "7:0 4:-1 0:0 10:0 8:0 15:0")

add_test(NAME avl-drr-1-remove COMMAND avl 1 int 10 1 int 4 1 int 15 1 int 0 1 int 7 1 int 13 1 int 6 1 int 8 2 int 13 3)
set_tests_properties(avl-drr-1-remove PROPERTIES PASS_REGULAR_EXPRESSION "7:0 4:0 0:0 6:0 10:0 8:0 15:0")
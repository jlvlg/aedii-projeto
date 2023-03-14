add_executable(rb rb.c)
target_link_libraries(rb RB)

add_test(NAME rb-insert COMMAND rb 1 int 50 3)
set_tests_properties(rb-insert PROPERTIES PASS_REGULAR_EXPRESSION "50:b:r:nil")

add_test(NAME rb-multiple-insert COMMAND rb 1 int 50 1 int 25 1 int 75 1 int 100 1 int 0 3)
set_tests_properties(rb-multiple-insert PROPERTIES PASS_REGULAR_EXPRESSION "50:b:r:nil 25:b:l:50 0:r:l:25 75:b:r:50 100:r:r:75")

add_test(NAME rb-int-char COMMAND rb 1 int 5 1 int 2 1 char a 1 int 10 1 char c 1 char b 1 int 1 3)
set_tests_properties(rb-int-char PROPERTIES PASS_REGULAR_EXPRESSION "5:b:r:nil 2:b:l:5 1:r:l:2 a:r:r:5 10:b:l:a c:b:r:a b:r:l:c")

add_test(NAME rb-int-str COMMAND rb 1 int 5 1 str abc 1 str aaa 1 str 1ab 1 str 25 3)
set_tests_properties(rb-int-str PROPERTIES PASS_REGULAR_EXPRESSION "aaa:b:r:nil 25:b:l:aaa 1ab:r:l:25 5:r:r:25 abc:b:r:aaa")

add_test(NAME rb-char-str COMMAND rb 1 char a 1 char 4 1 str 1ab 1 str aaa 1 str 111 1 char n 3)
set_tests_properties(rb-char-str PROPERTIES PASS_REGULAR_EXPRESSION "4:b:r:nil 1ab:b:l:4 111:r:l:1ab aaa:b:r:4 a:r:l:aaa n:r:r:aaa")

add_test(NAME rb-remove-no-children COMMAND rb 1 int 50 1 int 25 1 int 75 1 int 100 1 int 0 2 int 0 3)
set_tests_properties(rb-remove-no-children PROPERTIES PASS_REGULAR_EXPRESSION "50:b:r:nil 25:b:l:50 75:b:r:50 100:r:r:75")

add_test(NAME rb-remove-left-children COMMAND rb 1 int 50 1 int 25 1 int 75 1 int 100 1 int 0 2 int 25 3)
set_tests_properties(rb-remove-left-children PROPERTIES PASS_REGULAR_EXPRESSION "50:b:r:nil 0:b:l:50 75:b:r:50 100:r:r:75" FAIL_REGULAR_EXPRESSION "25")

add_test(NAME rb-remove-right-children COMMAND rb 1 int 50 1 int 25 1 int 75 1 int 100 1 int 0 2 int 75 3)
set_tests_properties(rb-remove-right-children PROPERTIES PASS_REGULAR_EXPRESSION "50:b:r:nil 25:b:l:50 0:r:l:25 100:b:r:50" FAIL_REGULAR_EXPRESSION "75")

add_test(NAME rb-remove-both-children COMMAND rb 1 int 50 1 int 25 1 int 75 1 int 100 1 int 0 2 int 50 3)
set_tests_properties(rb-remove-both-children PROPERTIES PASS_REGULAR_EXPRESSION "25:b:r:nil 0:b:l:25 75:b:r:25 100:r:r:75" FAIL_REGULAR_EXPRESSION "50")

add_test(NAME rb-remove-all-nodes COMMAND rb 1 int 50 1 int 25 1 int 75 1 int 100 1 int 0 2 int 50 2 int 25 2 int 0 2 int 75 2 int 100 3)
set_tests_properties(rb-remove-all-nodes PROPERTIES FAIL_REGULAR_EXPRESSION "0;25;50;75;100")

add_test(NAME rb-remove-from-empty COMMAND rb 1 int 1000 2 int 1000 2 int 0 3)
set_tests_properties(rb-remove-from-empty PROPERTIES PASS_REGULAR_EXPRESSION "\n" FAIL_REGULAR_EXPRESSION "1000;0")

add_test(NAME rb-remove-then-insert COMMAND rb 1 int 50 1 int 25 1 int 75 1 int 37 1 int 62 1 int 100 1 int 0 2 int 50 2 int 62 2 int 100 2 int 0 1 int 35 1 int 47 1 int 96 3)
set_tests_properties(rb-remove-then-insert PROPERTIES PASS_REGULAR_EXPRESSION "37:b:r:nil 25:b:l:37 35:r:r:25 75:b:r:37 47:r:l:75 96:r:r:75")
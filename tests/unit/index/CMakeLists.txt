add_executable(garbage garbage.c)
target_link_libraries(garbage Index)

add_test(NAME dump COMMAND garbage 1 100 0 3)
set_tests_properties(dump PROPERTIES PASS_REGULAR_EXPRESSION "^0:100 \n$")

add_test(NAME dump-multiple COMMAND garbage 1 100 0 1 300 101 1 200 402 3)
set_tests_properties(dump-multiple PROPERTIES PASS_REGULAR_EXPRESSION "^101:401 402:602 0:100 \n$")

add_test(NAME dump-left-merge COMMAND garbage 1 100 0 1 200 100 3)
set_tests_properties(dump-left-merge PROPERTIES PASS_REGULAR_EXPRESSION "^0:300 \n$")

add_test(NAME dump-right-merge COMMAND garbage 1 100 100 1 100 0 3)
set_tests_properties(dump-right-merge PROPERTIES PASS_REGULAR_EXPRESSION "^0:200 \n$")

add_test(NAME dump-both-merge COMMAND garbage 1 100 0 1 100 200 1 100 100 3)
set_tests_properties(dump-both-merge PROPERTIES PASS_REGULAR_EXPRESSION "^0:300 \n$")

add_test(NAME recycle-perfect COMMAND garbage 1 100 0 1 200 101 1 300 302 3 2 300 3)
set_tests_properties(recycle-perfect PROPERTIES PASS_REGULAR_EXPRESSION "^302:602 101:301 0:100 \n302\n101:301 0:100 \n$")

add_test(NAME recycle-imperfect COMMAND garbage 1 100 0 1 200 101 1 300 302 3 2 100 3)
set_tests_properties(recycle-imperfect PROPERTIES PASS_REGULAR_EXPRESSION "^302:602 101:301 0:100 \n302\n402:602 101:301 0:100 \n$")

add_test(NAME recycle-impossible COMMAND garbage 1 100 0 1 200 101 1 300 302 3 2 400 3)
set_tests_properties(recycle-impossible PROPERTIES PASS_REGULAR_EXPRESSION "^302:602 101:301 0:100 \n-1\n302:602 101:301 0:100 \n$")

add_test(NAME restore-perfect COMMAND garbage 1 100 0 1 200 101 1 300 302 3 2 300 3 1 300 302 3)
set_tests_properties(restore-perfect PROPERTIES PASS_REGULAR_EXPRESSION "^302:602 101:301 0:100 \n302\n101:301 0:100 \n302:602 101:301 0:100 \n$")

add_test(NAME restore-imperfect COMMAND garbage 1 100 0 1 200 101 1 300 302 3 2 100 3 1 100 302 3)
set_tests_properties(restore-imperfect PROPERTIES PASS_REGULAR_EXPRESSION "^302:602 101:301 0:100 \n302\n402:602 101:301 0:100 \n302:602 101:301 0:100 \n$")
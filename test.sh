#!/bin/bash

gcc -Wall -Wextra -Werror microshell_orig.c -o microshell_orig
./microshell_orig /bin/ls "|" /usr/bin/grep test.sh "|" /usr/bin/wc -c ";" /bin/echo i love my microshell ";" \
	cd /bin ";" /bin/pwd > orig_out \
&& ./microshell_orig a		2>> orig_out \
&& ./microshell_orig cd a b 2>> orig_out \
&& ./microshell_orig cd a 	2>> orig_out \
&& ./microshell_orig /bin/ls .. ";" ";" ";" /bin/ls .. >> orig_out

gcc -Wall -Wextra -Werror microshell.c -o microshell
./microshell /bin/ls "|" /usr/bin/grep test.sh "|" /usr/bin/wc -c ";" /bin/echo i love my microshell ";" \
	cd /bin ";" /bin/pwd > my_out \
&& ./microshell a		2>> my_out \
&& ./microshell cd a b 	2>> my_out \
&& ./microshell cd a 	2>> my_out \
&& ./microshell /bin/ls .. ";" ";" ";" /bin/ls .. >> my_out

# leaks --atExit -- ./microshell /bin/ls "|" /usr/bin/grep test.sh "|" /usr/bin/wc -c ";" /bin/echo i love my microshell ";" \
# 	cd /bin ";" /bin/pwd

diff -s orig_out my_out

rm orig_out my_out microshell microshell_orig

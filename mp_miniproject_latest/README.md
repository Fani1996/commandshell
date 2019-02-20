### TESTING STEP 1 ###
### BASIC FUNCTION ###


1. check whether print prompt ("myShell$").
   simply run ./myShell, and check if myShell$ is in the output, and the program is taking stdin.
   also, it will show current directory before $. (e.g. myShell:/home/yx139/ece551/mp_miniproject $ )


2. after running ./myShell,
   simply hit ENTER key, it will do nothing,
   and show a new line of myShell$ in the output, ready to take stdin.


3. different return status.

3.1 normal return, status 0.
    run 'ls'.

    EXPECTED:
    the command propely shows the list of files and directories of current directory.
    and print out 'Program exited with status 0'.
    
    This is correct.
    Becasue myShell match ls in the PATH, and run the program, which lists all the files and directory.
    As long as the 'ls' runs correctly, there should be no error, which corresponds to status 0.

    After exit status is printed and program exits,
    myShell will print new line prompt, waiting for input.

3.2 error return, status 1.
    run 'cp'.

    EXPECTED:
    '''
    cp: missing file operand
    Try 'cp --help' for more information.
    Program exited with status 1
    '''

    This is correct.
    Because we try to run 'cp' without other required arguments, thus program produces an error.
    Therefore return status 1, indicating an error occurs.
    Even though the child process produces an error, myShell is not affected and print a new line of prompt, waiting for input.

3.3 signal 11.
    run './killedby11'.

    EXPECTED:
    '''
    Program was killed by signal 11.
    '''

    This is correct.
    Because, './killedby11' will produce segment faults,
    and will be killed by SIGSEGV, indicating by signal 11.




### TESTING STEP  2 ###
### RUNNING COMMAND ###


4. directly run binary executable with path.
   run '/bin/ls'.

   EXPECTED:
   same as 'ls' performs.

   This is correct.
   Because with '/' in the command, the shell should not seach executable in PATH,
   and will directly run the program.


5. attempt to run binary executable with path that does not exist.
   run '/bin/lsssss'.

   EXPEXTED:
   '''
   Command /bin/lsssss not found
   '''

   This is correct.
   Because the input contains '/', so myShell will try to match command in the current directory.
   Since the command does not exist in '/bin/', then myShell should output an error.


6. run binary executable (without path).
   run 'cat README'.

   EXPECTED:
   the content of README is printed in the shell.

   This is correct.
   Because myShell manages to match 'cat' function in PATH, and process the command,
   adding the abusolute path in front of command. So we can run program in PATH without specific adding its path.


7. run binary executable (without path) that does not exist.
   run 'lsssss'.

   EXPECTED:
   '''
   Command lsssss not found
   '''

   This is correct.
   Because when input string contains no '/', it will try to search in PATH.
   When there is no match, it will print out 'command not found'.


8. attempt to run command not exist.
   run '.' or '..'.

   EXPECTED:
   '''
   Command . not found.
   '''

   This is correct.
   Because when try to access command path that does not exist.
   myShell will output 'command not found'.


9. attempt to run file that are not executable.
   run './README'.

   EXPECTED:
   '''
   command found.
   Program exited with status 1   
   '''

   This is correct.
   Because REAME do exist in current directory, but REAME does not have permission to execute.
   So even though command was found, program will exit with status 1.




### TESTING STEP 3 ###
### BASIC FUNCTION ###


10. run command without arguments.
    run 'top'.

    EXPECTED:
    shows the current processes and system resource usage on linux.

    This is correct.
    Since 'top' is a program that does not require arguments,
    so we can run it without arguments,
    and myShell will match the program and execute it.


11. command that does not exist.
    this is shown above in test case #7.


12. exit or EOF.

12.1 exit.
    run 'exit'.

    EXPECTED:
    myShell exits.

    This is correct.
    Because when input string matches with 'exit',
    it will break the loop and exit the program.

12.2 hit 'Ctrl+D'.
    whenever running the program, hit 'Ctrl+D'.

    EXPECTED:
    the program exits and prints out 'EOF Reached'.

    This is correct.
    Because when there is no input, hit ctrl+D will mean there exists EOF, and make myShell exit.     

12.3 pass in an empty file to './myShell'.
    when running './myShell' in the bash, run './myShell < empty.txt'.

    EXPECTED:
    the program exits and prints out 'EOF Reached'.

    This is correct.
    Because since there is no content in the empty.txt,
    so myShell will encounter EOF, and should exit.
    

13. run command with argument.
    run 'cat README'.

    EXPECTED:
    output the content of README.

    This is correct.
    Becuase myShell will parse the input string to command and its argument, and pass into the execute.


14. run command with multiple arguments.
    run './test-stat README commandShell.txt'

    EXPECTED:
    output the stat of both README and commandShell.txt

    This is correct.
    Because myShell will parse the input string,
    and into command and two arguments.
    Then stat will take in two arguments and show their stat accordingly.


15. skip the spaces.
    run 'echo hello,    world'.

    EXPECTED:
    '''
    hello, world
    '''

    This is correct.
    Because consecutive spaces is equivalent to a single space when parsing.
    So myShell will skip these spaces.


16. escape the '\ '.
    run 'echo hello,\ \ world'.

    EXPECTED:
    '''
    hello,  world
    '''

    This is correct.
    Because '\ ' will escape the space and backslash,
    making them into a space in the output string.


17. escape the '\ '.
    run 'echo hello,\      world'.

    EXPECTED:
    '''
    hello, world
    '''

    This is correct.
    Because '\ ' will escape one space, and the rest of the space is skipped.


18. escape the '\\'.
    run 'echo \\'.

    EXPECTED:
    '''
    \
    '''

    This is correct.
    Because '\\' will be translated into '\' in the output.


19. '\' being the last character in the input.
    run 'echo \'

    EXPECTED:
    '''

    '''

    This is correct.
    Because since '\' being the last character in the output,
    it will not escape any other character, and will be removed.
    So the output will end up with a blank line.


20. arguments with whitespace.
    run './myProgram         a               b     23'.

    EXPECTED:
    '''
    args:./myProgram.
    args:a.
    args:b.
    args:23.
    '''

    This is correct.
    Because the spaces are equivalent to a single space,
    so myShell will parse the input string, and ignore extra whitespaces.


21. arguments with whitespaces and '\'.
    run './myProgram  a\ b c\ \ d'.

    EXPECTED:
    '''
    args:./myProgram.
    args:a b.
    args:c  d.
    '''

    This is correct.
    Because the '\ ' will be escaped and translated into whitespaces in the final arguments,
    and the rest whitespace will devide arguments in the correct order.


22. arguments with whitespaces and '\'.
    run './myProgram a\  b c \ d'.

    EXPECTED:
    '''
    args:./myProgram.
    args:a .
    args:b.
    args:c.
    args: d.
    '''

    This is correct.
    Because myShell will escape the '\ ', and the rest of whitespace will divide arguments.




### TESTING STEP  4 ###
### CD AND VARIABLE ###


23. change directory.
    run 'cd /bin'

    EXPECTED:
    the directory in front of the myShell prompt will change to '/bin'.

    This is correct.
    Because when input command is 'cd', it will be matched by myShell.
    And the following argument will be passed in and used to change to that directory.


24. change directory.
    run 'cd ../'

    EXPECTED:
    the directory in front of the myShell prompt will change to '/home/yx139/ece551'.

    This is correct.
    Because when input command is 'cd', and argument is '..'.
    myShell will match it to chdir, and pass in '..' to chdir to change to its upper directory.


25. change directory.
    run 'cd'.

    EXPECTED:
    the directory in front of the myShell prompt will change to '/home/yx139'.

    This is correct.
    Because when input command is 'cd', and no argument is passed in.
    myShell will match chdir, and set the target directory to be 'HOME'.
    So it will change directory to HOME, which is '/home/yx139/' in this case.


26. change directory.
    run 'cd /bin /home'

    EXPECTED:
    cd error: too many arguments to call.

    This is correct.
    Because the 'cd' command should only take in only one or less argument.
    If two or more arguments are passed in, myShell should output an error.


27. change directory.
    run 'cd /root'

    EXPECTED:
    failed to open directory : Permission denied

    This is correct.
    Because myShell does not have the permission to change to '/root',
    so it reports an error and current directory is not changed.


28. set variable.
    run
    '''
    set name README
    cat $name
    '''

    EXPECTED:
    output the content of README.

    This is correct.
    Because when executing 'set name README', the value of name is set of 'README';
    when executing 'cat $name', the value of '$name' is replaced to 'README', and then execute the command.


29. set variable.
    run
    '''
    set command cat
    $command README
    '''

    EXPECTED:
    output the content of README.

    This is correct.
    Because after setting command as cat,
    the input string will be replaced by $command's actual content, which is 'cat',
    and then pass into parse() to be parsed and excuted.


30. set variable.
    run
    '''
    set name     READ    ME
    echo $name
    '''

    EXPECTED:
    '''
    READ ME
    '''

    This is correct.
    Because whitespaces are equivalent to a single whitespace,
    and therefore the output will skip extra whitespace.


31. set variable.
    run
    '''
    set x x
    set x $x$x
    echo $x
    '''

    EXPECTED:
    '''
    xx
    '''

    This is correct.
    Because the after 'set x x', variable x is set to x.
    The second command, each '$x' is replaced with x, then $x becomes 'xx'.
    So the output should be 'xx'.


32. set variable.
    run
    '''
    set x x
    set $x $x$x
    echo $x
    '''

    EXPECTED:
    '''
    xx
    '''

    This is correct.
    Because the after setting $x to be x, the second command will first replace $x to 'x',
    and then set x to 'xx',
    so the output will be 'xx'.


33. set variable.
    run 'echo $z'

    EXPECTED:
    '''
    $z
    '''

    This is correct.
    Becuase without set any variable, the variable $x should be empty,
    so '$' will become plain character, input will not be replaced,
    so echo will output '$z'.


34. set variable.
    run
    '''
    set _name README
    cat $_name
    '''

    EXPECTED:
    output the conten of README.

    This is correct.
    Because the the variable name contains numbers, characters, and '_'.
    So variable name containing '_' is also valid name.


35. set variable.
    run
    '''
    set 17 README
    echo $17
    '''

    EXPECTED:
    output the conten of README.

    This is correct.
    Because the the variable name contains numbers, characters, and '_'.
    So variable name containing numerical numbers is also valid name.



36. set variable.
    run
    '''
    set	3\\4 README
    '''

    EXPECTED:
    set var error: key contains invalid characters.

    This is correct.
    Because the inputstring contains '\', which is not a valid character in variable name.
    So myShell should output an error.


37. set variable.
    run
    '''
    set x y
    echo $x...
    '''

    EXPECTED:
    '''
    y...
    '''

    This is correct.
    Because myShell will match the longest sequence that satisifies the valid rule of variable name,
    and replace its value.


38. set variable.
    run
    '''
    set var 123
    set varstring 456
    echo $varstring
    '''

    EXPECTED:
    '''
    456
    '''

    This is correct.
    Because myShell will match the longest variable that is valid in input,
    so in this case, the longest valid variable name is 'varstring', instead of 'var'.


39. set variable.
    run
    '''
    set x y
    set x \$x
    echo $x
    '''

    EXPECTED:
    '''
    $x
    '''

    This is correct.
    Because the second command will translate \$ into plain dollar sign,
    so it will not match variable, therefore x is set to '$x'.
    So the output will be '$x'.


40. set variable
    run
    '''
    set x x
    echo $$x
    '''

    EXPECTED:
    '''
    $x
    '''

    This is correct.
    Because the first $ in input string does not match a variable,
    so only the second one replace a variable, and the first one serves as plain character.


42. export variable.
    run
    '''
    set x x
    env
    '''

    EXPECTED:
    env does not contain variable x.

    This is correct.
    Because without export x, the variable only lives in shell itself,
    and not export to the system env.
    

43. export variable.
    run
    '''
    set x x
    export x
    env
    '''

    EXPECTED:
    '''
    x=x
    '''

    This is correct.
    Because after 'export', myShell will add x into system env.
    so we can check x's value in the system env.


44. export variable.
    run
    '''
    set x x
    export x
    env
    set x y
    env
    '''

    EXPECTED:
    '''
    x=x
    x=y
    '''

    This is correct.
    Because after 'export', myShell will add x into system env.
    and when we modify the value of x, its value should be updated as well in system env.


45. export variable.
    run
    '''
    export x y
    '''

    EXPECTED:
    export var error: invalid arguments number.

    This is correct.
    Because 'export' should only take in one argument,
    so if passing in multiple arguments, myShell should output an error.


46. export variable.
    run
    '''
    export x\\
    '''

    EXPECTED:
    export var error: key contains invalid characters.

    This is correct.
    Because the variable name contains invalid character,
    so myShell would check if variable name is invalid first,
    and output an error.


47. inc variable.
    run
    '''
    set x 1
    export x
    env
    inc x
    env
    '''

    EXPECTED:
    '''
    x=1
    x=2
    '''

    This is correct.
    Because variable x is a number,
    so 'inc' would increse the value of x,
    and update its value accordingly.


48. inc variable.
    '''
    set x README
    export x
    env
    inc x
    env
    '''

    EXPECTED:
    '''
    x=README
    x=1
    '''

    This is correct.
    Because variable x is not number,
    so 'inc' would first make variable x's value become 0,
    then myShell would increase x's value to 1,
    and update its value correspondingly.


49. set variable with decimal number.
    run
    '''
    set x 1.5
    export x
    env
    inc x
    env
    '''

    EXPECTED:
    '''
    x=1.5
    x=1
    '''

    This is correct.
    Becuase if x's value is not int, then it is treated as string,
    so when 'inc x', x's value will be set to 0,
    and then increase to 1.




### TESTING STEP5 ###
###  REDIRECTION  ###


50. redirection input.
    run 'cat < README'.
    
    EXPECTED:
    output the content of README.

    This is correct.
    Because the content of README is redirected to stdin,
    and 'cat' will output the content from stdin to stdout when there is no argument.
    So this will output the content of README.


51. redirection output.
    run
    '''
    ls > output.txt
    cat output.txt
    '''

    EXPECTED:
    no output from 'ls', and the 'output.txt' contains the output instead.
    
    This is correct.
    Because the output of 'ls' is redirected to file 'output.txt',
    so there should be no output from 'ls',
    and myShell should produce the 'output.txt' file containing the content of 'ls'.


52. redirection output when error occurs.
    run
    '''
    ping > output.txt
    cat output.txt
    '''

    EXPECTED:
    The error message of 'ping' will show up in the stdout,
    and only regular stdout will show up in the output.txt,
    instead of error message.

    This is correct.
    Because when passing no arguments to 'ping',
    'ping' would produce error message,
    however, since input string only uses '>' to redirect output,
    so error message will still show up in stdout, and only stdout in output.txt.


53. redirection error.
    run
    '''
    ping 2> err.txt
    cat err.txt
    '''

    EXPECTED:
    Only exit status will show up in stdout,
    and error messages from 'ping' will be in output.txt.

    This is correct.
    Becuase '2>' will redirect error message into 'err.txt',
    so only exit status will still be outputed.
    And 'err.txt' contains error message from 'ping'.


54. redirection.
    run
    '''
    wc -l < test-wc.txt > output.txt
    cat output.txt
    '''

    EXPECTED:
    the wc information of test-wc.txt should be inside 'output.txt'

    This is correct.
    Becuase stat reads from 'test-wc.txt', and writes to 'output.txt'.


55. redirection.
    run
    '''
    wc -l<test-wc.txt>output.txt
    cat output.txt
    '''

    EXPECTED:
    the wc information of test-wc.txt should be inside 'output.txt'

    This is correct.
    Becuase stat reads from 'test-wc.txt', and writes to 'output.txt'.
    And the whitespaces do not matter.


56. redirection.
    run
    '''
    wc <test-wc.txt >output.txt -l
    cat output.txt
    '''

    EXPECTED:
    the wc information of test-wc.txt should be inside 'output.txt'

    This is correct.
    Becuase stat reads from 'test-wc.txt', and writes to 'output.txt'.
    And the order of redirections and arguments do not matter.


57. redirection.
    run
    '''
    <test-wc.txt >output.txt wc -l
    cat output.txt
    '''

    EXPECTED:
    the wc information of test-wc.txt should be inside 'output.txt'

    This is correct.
    Becuase stat reads from 'test-wc.txt', and writes to 'output.txt'.
    And the order of redirections and arguments do not matter.


58. wrong usage of redirection.
    run
    '''
    wc <test-wc.txt <README
    '''

    EXPECTED:
    '''
    redirection error: invalid format.
    '''

    This is correct.
    Because if there is more than two input redirection,
    then the input string should be invalid,
    myShell will output an error.
    

59. wrong usage of redirection.
    run
    '''
    ls >> output.txt
    '''

    EXPECTED:
    '''
    redirection error: invalid format.
    '''

    This is correct.
    Because if there is more than two input redirection,
    then the input string should be invalid,
    myShell will output an error.


60. wrong usage of redirection.
    run
    '''
    cat <
    '''

    EXPECTED:
    run 'cat' with no input file redirected.
    'cat' waits for users' input.

    This is correct.
    Because since there is no input file specificed after '<',
    then the input should not be redirected.
    so cat will run as there is no input file.
    

61. wrong usage of redirection.
    run
    '''
    < README
    '''

    EXPECTED:
    '''
    parse error: empty input string.
    '''

    This is correct.
    Because even thought the redirection part is correct,
    there is no command in the input string,
    so myShell should output an error and indicates empty input.
    

62. wrong usage of redirection.
    run
    '''
    ping 3> output.txt
    '''

    EXPECTED:
    '''
    connect: Invalid argument.   
    Program exited with status 2
    '''

    This is correct.
    Becuase if input string contains invalid error redirection '3>',
    then '3' will be considered as the argument, and output file will be set to 'output.txt',
    since '3' becomes the argument,
    'ping' will output an error indicating invalid argument.


63. escape the redirection.
    run
    '''
    cat README\>output.txt
    '''

    EXPECTED:
    output is redirected to 'output.txt',
    and the content of README is stored in 'output.txt'.

    This is correct.
    Because we parse the redirection first, so redireciton becomes output.txt,
    and then input string becomes 'cat README\',
    so the output from 'cat README' will be redirected to 'output.txt'.


64. redirection to non-existing file.
    run
    '''
    cat < READMEEE
    '''

    EXPECTED:
    '''
    failed to open directory : No such file or directory    
    '''

    This is correct.
    Because 'READMEEE' does not exist in the current directory,
    so redirection will never success,
    myShell will first check if parsed files exist, and then actually set up the redirection.




### TESTING STEP6 ###
### COMMAND  PIPE ###


65. basic pipe function.
    run
    '''
    ls | cat
    '''

    EXPECTED:
    output the content from 'ls' command.

    This is correct.
    Because pipe sign '|' will make the output of 'ls' function into 'cat' function.
    So this will output the output information from 'ls'.


66. basic pipe function.
    run
    '''
    cat < README | wc > output.txt
    '''

    EXPECTED:
    the output of 'cat < README' will not show,
    and its output will become the input of 'wc',
    so the output of 'wc' will be stored in 'output.txt'.

    This is correct.
    Becuase pipe will pass the output of 'cat' into the 'wc',
    so the output of word count will show up in the 'output.txt'.
    

67. basic pipe function.
    run
    '''
    cat < README | cat | wc > output.txt
    '''

    EXPECTED:
    the output of 'cat README' will not show,
    and its output will become the input of next 'cat',
    and then the output of the second 'cat' will become the input of 'wc',
    also inside the 'output.txt', the word count number should be larger than test case 65,

    This is correct.
    Because the output of the first cat function will be passed to the second cat function,
    and then passed into the wc function, so no ouput from cat will show up,
    and the output of wc will be stored in output.txt,
    also the output information of the first and second 'cat' is also calculated,
    so the total word count increses compared to test case 65.


68. basic pipe function (handle error).
    run
    '''
    cat README |
    '''

    EXPECTED:
    '''
    pipe error: invalid pip format.
    '''

    This is correct.
    Because there is no valid command behing the '|',
    so myShell will ignore this input and output an error.


69. basic pipe function (handle error).
    run
    '''
    cat README | <README
    '''

    EXPECTED:
    '''
    

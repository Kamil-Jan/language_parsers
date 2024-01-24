#!/bin/bash

for (( i=1; ; i++ )) do
    python gen_grammar.py > grammar_file

    for (( j=0; j < 100; j++)) do
        echo "$i - $j"
        python gen_seq.py > word_file

        ../bin/run -g grammar_file -w word_file -p lr1 > fast 2> fast_err
        python slow_parser.py > sl

        grep_res=$(grep "LR(1)" fast_err)
        if [ $? -eq 0 ]
        then
            break
        fi

        result=$(diff fast sl)
        if [ $? -eq 1 ]
        then
            cat grammar_file
            cat word_file
            echo ----
            cat fast
            cat sl
            break
        fi
    done
done


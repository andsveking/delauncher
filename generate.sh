if [ "$(uname)" == "Darwin" ]; then
    genie gmake
elif [ "$(expr substr $(uname -s) 1 10)" == "MINGW64_NT" ]; then
    genie vs2015
else
    genie gmake
fi

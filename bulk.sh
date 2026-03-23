TIMEFORMAT='%R'
for name in bmh bmo dmb kmp kra
do
    time ./tests.sh ./bin/$name
done


counter=0
totalCounter=0
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'
while [ "$totalCounter" -lt 60 ]
do 
    totalCounter=$((totalCounter+1))
    inputfile="testing/test_cases/inputs/input$totalCounter.txt"
    outputfile="testing/test_cases/outputs/output$totalCounter.txt"
    start=$(date +%s)
    python3 homework.py "$inputfile"
    end=$(date +%s)
    output=$(head -n 1 $outputfile)
    ans=$(head -n 1 output.txt)
    if [ $ans == $output ]; then
        counter=$((counter+1))
        echo "Test $totalCounter ${GREEN}passed${NC}     \t| Time: $(($end-$start)) seconds"
    else 
        echo "Test $totalCounter ${RED}failed${NC}       \t| Time: $(($end-$start)) seconds"
    fi
done
echo "---------------------------------"
echo "Test $counter/$totalCounter passed"
echo "---------------------------------"
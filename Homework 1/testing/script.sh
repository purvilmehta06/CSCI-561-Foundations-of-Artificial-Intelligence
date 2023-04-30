counter=0
totalCounter=0
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'
while [ "$totalCounter" -lt 50 ]
do 
    totalCounter=$((totalCounter+1))
    inputfile="testing/test_cases/inputs/input$totalCounter.txt"
    outputfile="testing/test_cases/outputs/output$totalCounter.txt"
    algorithm=$(head -n 1 $inputfile)
    costsActual=`python3 testing/check_for_cost.py $outputfile $algorithm`
    start=$(date +%s)
    python3 homework.py "$inputfile"
    costs=`python3 testing/check_for_cost.py output.txt $algorithm`
    end=$(date +%s)
    if [ "$costs" == "$costsActual" ]; then
        counter=$((counter+1))
        echo "Test $totalCounter ${GREEN}passed${NC}     \t| Time: $(($end-$start)) seconds"
    else 
        echo "Test $totalCounter ${RED}failed${NC}       \t| Time: $(($end-$start)) seconds"
    fi
done
echo "---------------------------------"
echo "Test $counter/$totalCounter passed"
echo "---------------------------------"
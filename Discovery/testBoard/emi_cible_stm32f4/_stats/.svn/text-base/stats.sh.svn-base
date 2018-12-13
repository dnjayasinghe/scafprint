#!/bin/bash

LOG=${1:?LOG_FILE}
RSZ=${2:?R_SIZE}
R=${3:?R}

./format_result.pl "$LOG" "$R" > res_formatted_"$RSZ"

echo -n "Total : "
TTL=$(wc -l res_formatted_"$RSZ" | cut -d' ' -f1)
echo $TTL

echo ###########################################################

grep -E '0$' res_formatted_"$RSZ" > detect_"$RSZ"
while read line; do
  l=($line)
  grep ${l[1]} resultats_non_protege_formatted
done < detect_"$RSZ" > detect_"$RSZ"_ref

echo -n "Positives : "
echo $(wc -l detect_"$RSZ" | cut -d' ' -f1)

sort detect_"$RSZ"_ref | cut -d' ' -f1-5 | sponge detect_"$RSZ"_ref
sort detect_"$RSZ" | cut -d' ' -f1-5 | sponge detect_"$RSZ"

echo -n "True : "
TP=$(comm -23 detect_"$RSZ" detect_"$RSZ"_ref | wc -l | cut -d' ' -f1)
echo $TP" ("$(bc -l <<<"$TP / $TTL * 100.0")" %)"

echo -n "False : "
FP=$(comm -12 detect_"$RSZ" detect_"$RSZ"_ref | wc -l | cut -d' ' -f1)
echo $FP" ("$(bc -l <<<"$FP / $TTL * 100.0")" %)"

echo ###########################################################

grep -E '1$' res_formatted_"$RSZ" > nodetect_"$RSZ"
while read line; do
  l=($line)
  grep ${l[1]} resultats_non_protege_formatted
done < nodetect_"$RSZ" > nodetect_"$RSZ"_ref

echo -n "Negatives : "
echo $(wc -l nodetect_"$RSZ" | cut -d' ' -f1)

sort nodetect_"$RSZ"_ref | cut -d' ' -f1-5 | sponge nodetect_"$RSZ"_ref
sort nodetect_"$RSZ" | cut -d' ' -f1-5 | sponge nodetect_"$RSZ"

echo -n "True : "
TN=$(comm -12 nodetect_"$RSZ" nodetect_"$RSZ"_ref | wc -l | cut -d' ' -f1)
echo $TN" ("$(bc -l <<<"$TN / $TTL * 100.0")" %)"

echo -n "False : "
FN=$(comm -23 nodetect_"$RSZ" nodetect_"$RSZ"_ref | wc -l | cut -d' ' -f1)
echo $FN" ("$(bc -l <<<"$FN / $TTL * 100.0")" %)"

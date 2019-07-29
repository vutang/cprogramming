
a=1.435
b=1.5656

awk 'BEGIN { print ($a > $b) ? "1" : "0" }'


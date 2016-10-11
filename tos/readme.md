# 0w0
## What can it do?

![abby](./2471.png)

## Requirement:

FIY

## usage:

```bash

./getMapInfo.sh |./gmi2dot.sh |dot -Tpng | display -

```

multiple


```bash

curl -sv "http://www.tosbase.com/database/maps/?name=&order=Level&type=Field&page=5" |&grep -o "game/world-map/[0-9]*/" |sed 's/[^0-9]//g'|xargs -n1 -P0 ./getMapInfo.sh 

```


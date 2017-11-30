UserAgent="Mozilla/5.0 (X11; Linux x86_64; rv:52.0) Gecko/20100101 Firefox/52.0"
ConcurrentRequest=8
Id="$1"
getItemsByIdAndPages(){
  curl -s "http://class.ruten.com.tw/user/index00.php?s=$1&p=$2" -H 'Host: class.ruten.com.tw' -H "User-Agent: $UserAgent" -H 'Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8' -H 'Accept-Language: en-US,en;q=0.5'-H 'Connection: keep-alive'|sed -n '/<a href=\"http:\/\/goods.*\/a>/ {s/[^<]*<a href="\([^"]*\)">\(.*\)<\/a>/\1 "\2"/;h};/<span class="item-direct-price"/ {n;n;s/^[ \t]*//g;H;x;s/\n/ /;p}'
}
export -f getItemsByIdAndPages
export Id

items=`curl -s "http://class.ruten.com.tw/user/class_frame.php?sid=$1&classid=" -H 'Host: class.ruten.com.tw' -H "User-Agent: $UserAgent" -H 'Accept: application/json, text/javascript, */*; q=0.01' -H 'Accept-Language: en-US,en;q=0.5' -H 'x-requested-with: XMLHttpRequest' -H "Referer: http://class.ruten.com.tw/user/index00.php?s=$1" -H 'Connection: keep-alive'|sed -n 's/{"total\":\([0-9]*\),.*/\1/p'`
pages=`expr $items / 30 + 1`

seq $pages | xargs -n 1 -P "$ConcurrentRequest" -I {} bash -c 'getItemsByIdAndPages "$Id" "$@"' _ {}


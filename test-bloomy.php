<?php

$bf = new BloomFilter(100);
print_r($bf->getInfo());

$words = array(
    'Hello', 'World', 'Good', 'Better', 'Best',
    '中国', '世界', '公司', '旅游', '电影', 'PHP是世界上最好的语言'
);

foreach ($words as $item) {
    echo 'add ', $item, "\n";
    $bf->add($item);
}

print_r($bf->getInfo());

foreach ($words as $item) {
    echo 'has ', $item, ' ? ';
    if ($bf->has($item)) {
        echo 'yes';
    } else {
        echo 'no';
    }
    echo "\n";
}

echo "\n\nserialize to /tmp/test-bloomy.data\n";
file_put_contents('/tmp/test-bloomy.data', serialize($bf));
unset($bf);

echo "unserialize and test again\n";
$bf2 = unserialize(file_get_contents('/tmp/test-bloomy.data'));
print_r($bf2->getInfo());
foreach ($words as $item) {
    echo 'has ', $item, ' ? ';
    if ($bf2->has($item)) {
        echo 'yes';
    } else {
        echo 'no';
    }
    echo "\n";
}

--TEST--
add() test
--SKIPIF--
<?php if (!extension_loaded("bloomy")) print "skip"; ?>
--FILE--
<?php 
$bf = new BloomFilter(100, 0.01);

var_dump($bf->add(""));

var_dump($bf->add("foo"));

?>
--EXPECTF--
Warning: BloomFilter::add(): could not add data to filter in %s on line %d
bool(false)
bool(true)

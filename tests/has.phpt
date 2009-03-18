--TEST--
has() test
--SKIPIF--
<?php if (!extension_loaded("bloomy")) print "skip"; ?>
--FILE--
<?php 
$bf = new BloomFilter(100, 0.01);

$bf->add("foo");

var_dump($bf->has("foo")); // must have it

?>
--EXPECTF--
bool(true)

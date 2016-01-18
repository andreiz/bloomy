--TEST--
unserialize with integer overflow test
--SKIPIF--
<?php if (!extension_loaded("bloomy")) print "skip"; ?>
--FILE--
<?php
$bf = new BloomFilter(100, 0.01);

$bf->add("foo");

var_dump(unserialize(serialize($bf)));

$int = 9223372036854775808;

?>
--EXPECTF--
object(BloomFilter)#%d (0) {
}

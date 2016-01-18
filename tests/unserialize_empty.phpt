--TEST--
unserialize empty BloomFilter test
--SKIPIF--
<?php if (!extension_loaded("bloomy")) print "skip"; ?>
--FILE--
<?php
$bf = new BloomFilter(100, 0.01);

var_dump(unserialize(serialize($bf)));

?>
--EXPECTF--
object(BloomFilter)#%d (0) {
}

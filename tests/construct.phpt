--TEST--
BloomFilter constructor
--SKIPIF--
<?php if (!extension_loaded("bloomy")) print "skip"; ?>
--FILE--
<?php 
$bf = new BloomFilter(0);
$bf = new BloomFilter(100, 0.0);
$bf = new BloomFilter(100, 1.0);

$bf = new BloomFilter(100, 0.01);
echo get_class($bf);

?>
--EXPECTF--
Warning: BloomFilter::__construct(): bad filter parameters in %s on line %d

Warning: BloomFilter::__construct(): bad filter parameters in %s on line %d

Warning: BloomFilter::__construct(): bad filter parameters in %s on line %d
BloomFilter

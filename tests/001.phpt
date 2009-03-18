--TEST--
Check for bloomy presence
--SKIPIF--
<?php if (!extension_loaded("bloomy")) print "skip"; ?>
--FILE--
<?php 
echo "bloomy extension is available";
?>
--EXPECT--
bloomy extension is available

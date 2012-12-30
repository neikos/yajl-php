--TEST--
null
--SKIPIF--
<?php if (!extension_loaded("yajl")) print "skip"; ?>
--FILE--
<?php
function test_yajl_null($parser)
{
    echo "null\n";
}

function test_yajl_boolean($parser, $boolVal)
{
    printf("bool: %s\n", $boolVal?"true":"false");
}

function test_yajl_number($parser, $numberVal)
{
    $numberType = gettype($numberVal);

    if ( $numberType == 'integer' )
    {
        printf("integer: %ld\n", $numberVal);
    }
    else
    {
        printf("double: %g\n", $numberVal);
    }
}

function test_yajl_string($parser, $stringVal)
{
    printf("string: '%s'\n",$stringVal);
}

function test_yajl_map_key($parser, $stringVal)
{
    printf("key: '%s'\n",$stringVal);
}

function test_yajl_start_map($parser)
{
    printf("map open '{'\n");
}

function test_yajl_end_map($parser)
{
    printf("map close '}'\n");
}

function test_yajl_start_array($parser)
{
    printf("array open '['\n");
}

function test_yajl_end_array($parser)
{
    printf("array close ']'\n");
}

$parser = yajl_parser_create();

yajl_set_null_handler($parser, 'test_yajl_null');
yajl_set_boolean_handler($parser, 'test_yajl_boolean');
yajl_set_number_handler($parser, 'test_yajl_number');
yajl_set_string_handler($parser, 'test_yajl_string');
yajl_set_map_key_handler($parser, 'test_yajl_map_key');
yajl_set_start_map_handler($parser, 'test_yajl_start_map');
yajl_set_end_map_handler($parser, 'test_yajl_end_map');
yajl_set_start_array_handler($parser, 'test_yajl_start_array');
yajl_set_end_array_handler($parser, 'test_yajl_end_array');

$test_case = basename($_SERVER['PHP_SELF'],'.php');

$hJson = fopen('tests'.DIRECTORY_SEPARATOR.'cases'.DIRECTORY_SEPARATOR.$test_case.'.json', 'rb');

$data = '';
$parse_success = 0;

while ( ! feof($hJson) )
{
    $data = fread($hJson,512);
    if ( ! $parse_success = yajl_parse($parser,$data) ) break;
}

$parse_success = yajl_complete_parse($parser);

if ( ! $parse_success ) echo yajl_get_error($parser)."\n";

fclose($hJson);
unset($data);
yajl_parser_free($parser);
?>
--EXPECT--
null

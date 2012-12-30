--TEST--
ac_difficult_json_c_test_case_with_comments
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

yajl_parser_set_option($parser,YAJL_ALLOW_COMMENTS,1);

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
map open '{'
key: 'glossary'
map open '{'
key: 'title'
string: 'example glossary'
key: 'GlossDiv'
map open '{'
key: 'title'
string: 'S'
key: 'GlossList'
array open '['
map open '{'
key: 'ID'
string: 'SGML'
key: 'SortAs'
string: 'SGML'
key: 'GlossTerm'
string: 'Standard Generalized Markup Language'
key: 'Acronym'
string: 'SGML'
key: 'Abbrev'
string: 'ISO 8879:1986'
key: 'GlossDef'
string: 'A meta-markup language, used to create markup languages such as DocBook.'
key: 'GlossSeeAlso'
array open '['
string: 'GML'
string: 'XML'
string: 'markup'
array close ']'
map close '}'
array close ']'
map close '}'
map close '}'
map close '}'

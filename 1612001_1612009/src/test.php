<?php
/**
 * Created by PhpStorm.
 * User: Beo Hoang
 * Date: 4/19/2018
 * Time: 5:17 PM
 */

require "class.php";
header("Content-Type: application/json");

try {
    $config = [
        "client_id"=>"asd",
        "client_secret"=>"asd"
    ];
    $app = new Beo\Instagram($config);
}
catch (Exception $e) {
    echo json_encode([
        "res"=>"failed",
        "msg"=>$e->getMessage()
    ]);
}
?>
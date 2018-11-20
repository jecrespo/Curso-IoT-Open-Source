<?php
error_reporting(E_ALL);
ini_set('display_errors', 1);

//recojo variables
if(isset($_GET["arduino"]) && isset($_GET["IntensidadLuminosa"])){
	$arduino = $_GET["arduino"];
	$IntensidadLuminosa = $_GET["IntensidadLuminosa"];
}
else{
	echo("Faltan Datos");
	die();
}
//Crear conexión a la Base de Datos
$mysqli = new mysqli('localhost', 'pi', 'tu_contraseña', 'DatosArduino');


if ($mysqli->connect_errno) {
	echo "Lo sentimos, este sitio web está experimentando problemas.";
	echo "Error: Fallo al conectarse a MySQL debido a: \n";
    echo "Errno: " . $mysqli->connect_errno . "\n";
    echo "Error: " . $mysqli->connect_error . "\n";
    exit;
}

$sql = "INSERT INTO luminosidad (fecha, arduino, IntensidadLuminosa) VALUES (CURRENT_TIMESTAMP, {$arduino}, {$IntensidadLuminosa})";

if (!$resultado = $mysqli->query($sql)) {
	echo "Lo sentimos, este sitio web está experimentando problemas.";
	echo "Error: La ejecución de la consulta falló debido a: \n";
    echo "Query: " . $sql . "\n";
    echo "Errno: " . $mysqli->errno . "\n";
    echo "Error: " . $mysqli->error . "\n";
    exit;
}

$mysqli->close();
echo ("GRABADOS");
?>

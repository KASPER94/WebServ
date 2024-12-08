#!/usr/bin/php
<?php

// En-tête de la réponse HTTP
header("Content-Type: application/json");

// Vérifier si la méthode est POST
if ($_SERVER['REQUEST_METHOD'] !== 'POST') {
    echo json_encode([
        "status" => "error",
        "message" => "Invalid request method. Only POST is allowed."
    ]);
    exit;
}

// Récupérer les données POST
$postData = file_get_contents('php://input');

// Traiter les données du formulaire
$formData = [];
parse_str($postData, $formData); // Convertit les données URL-encodées en tableau

// Construire une réponse JSON
$response = [
    "status" => "success",
    "message" => "Form data received successfully",
    "data" => $formData,
    "env" => [
        "CONTENT_TYPE" => getenv("CONTENT_TYPE"),
        "CONTENT_LENGTH" => getenv("CONTENT_LENGTH"),
        "QUERY_STRING" => getenv("QUERY_STRING"),
        "REMOTE_ADDR" => getenv("REMOTE_ADDR"),
        "REQUEST_METHOD" => getenv("REQUEST_METHOD")
    ]
];

// Retourner la réponse JSON
echo json_encode($response);

?>

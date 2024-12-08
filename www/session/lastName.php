<?php
session_start();

// Supprimer la session si le paramètre 'deleteSession' est présent dans l'URL
if (isset($_GET['deleteSession']) && $_GET['deleteSession'] === 'true') {
    session_unset();
    session_destroy();
} else {
    // Enregistrer 'lName' dans la session si passé dans l'URL
    if (isset($_GET['lName']) && $_GET['lName'] !== '') {
        $_SESSION['lName'] = $_GET['lName'];
    }

    // Exemple d'accès à des variables d'environnement via $_SERVER
    if (isset($_SERVER['QUERY_STRING']) && $_SERVER['QUERY_STRING'] !== '') {
        // Stocker la chaîne de requête complète dans la session pour le débogage
        $_SESSION['query_string'] = $_SERVER['QUERY_STRING'];
    }

    if (isset($_SERVER['REQUEST_METHOD'])) {
        // Stocker la méthode HTTP utilisée pour cette requête
        $_SESSION['request_method'] = $_SERVER['REQUEST_METHOD'];
    }
}
?>
<!DOCTYPE html>
<html>

<head>
    <title>Session</title>
    <link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/css/bootstrap.min.css">
    <link rel="preconnect" href="https://fonts.googleapis.com">
    <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
    <link href="https://fonts.googleapis.com/css2?family=Roboto+Mono:ital,wght@0,100..700;1,100..700&display=swap" rel="stylesheet">
    <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.7.1/jquery.min.js"></script>
</head>

<body <?php if (isset($_SESSION['bg'])) { ?>style="background-color:#<?php echo htmlspecialchars($_SESSION['bg']); ?> !important;" <?php } ?>>
    <div class="container p-3">
        <div class="rounded-3 p-5" style="background-color:#fcfcfc; border:solid 1px #cdcdcd">
            <center>
                <h2>Session Management</h2>
            </center>
            <div class="d-flex flex-row justify-content-around p-3 rounded-3" style="background-color: #ededed;">
                <span>Your background color is: <b><?php echo isset($_SESSION['bg']) ? "#" . htmlspecialchars($_SESSION['bg']) : "undefined"; ?></b></span>
            </div>
            <br />
            <div class="d-flex flex-row justify-content-around p-3 rounded-3" style="background-color: #ededed;">
                <span>Your first name is: <b><?php echo isset($_SESSION['fName']) ? htmlspecialchars($_SESSION['fName']) : "undefined"; ?></b></span>
            </div>
            <br />
            <div class="d-flex flex-row justify-content-around align-items-center p-2 rounded-3" style="background-color: #ededed;">
                <input id="lastName" name="lastName" type="text" class="form-control" style="margin-right: 10px;" placeholder="Last Name: <?php echo isset($_SESSION['lName']) ? htmlspecialchars($_SESSION['lName']) : ''; ?>" />
                <button class="btn btn-primary" id="saveName">Save</button>
            </div>
            <br />
            <div class="d-flex flex-row justify-content-around p-2 rounded-3" style="background-color: #ededed;">
                <a class="btn btn-primary" href="/." style="width: 150px">Home</a>
                <a class="btn btn-secondary" href="/firstName" style="width: 150px">First Name</a>
                <a class="btn btn-success" href="/lastName" style="width: 150px">Last Name</a>
            </div>
            <br />
            <div class="d-flex flex-row justify-content-around p-2 rounded-3" style="background-color: #ededed;">
                <a class="btn btn-danger" href=".?deleteSession=true">Delete session</a>
            </div>
            <br />
            <!-- Debugging Section -->
            <div class="d-flex flex-column justify-content-start p-3 rounded-3" style="background-color: #f9f9f9; border:solid 1px #ddd">
                <h5>Debugging Information:</h5>
                <p><strong>Request Method:</strong> <?php echo htmlspecialchars($_SESSION['request_method'] ?? 'undefined'); ?></p>
                <p><strong>Query String:</strong> <?php echo htmlspecialchars($_SESSION['query_string'] ?? 'undefined'); ?></p>
            </div>
        </div>
    </div>
</body>
<script type="text/javascript">
    $(document).ready(function() {
        // Redirection après avoir saisi le nom de famille
        $("#saveName").on("click", function() {
            var lName = $("#lastName").val();
            location.href = "/lastName/?lName=" + encodeURIComponent(lName);
        });
    });
</script>

</html>

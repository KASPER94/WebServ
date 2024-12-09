console.log("js loaded");

$(document).ready(function() {
    // Intercepter la soumission du formulaire pour empêcher un rafraîchissement de page
    $('#uploadForm').on('submit', function(event) {
        event.preventDefault(); // Empêche la soumission normale du formulaire
        
        var formData = new FormData(this);  // Récupère les données du formulaire

        // Envoyer les données via AJAX
        $.ajax({
            url: '/upload-endpoint',  // L'URL de ton endpoint de téléchargement (côté serveur)
            type: 'POST',
            data: formData,
            processData: false,   // Ne pas traiter les données, nécessaire pour FormData
            contentType: false,   // Empêcher jQuery de définir le type de contenu
            success: function(response) {
                // Traiter la réponse JSON et l'afficher dans la div uploadStatus
                var jsonResponse = JSON.parse(response); // Parser la réponse JSON
                if (jsonResponse.status === "success") {
                    $('#uploadStatus').html("<div class='alert alert-success'>" + 
                        jsonResponse.message + "<br>Files uploaded: " + jsonResponse.files.join(', ') + 
                        "</div>");
                } else {
                    $('#uploadStatus').html("<div class='alert alert-danger'>Error: " + jsonResponse.message + "</div>");
                }
            },
            error: function(xhr, status, error) {
                // En cas d'erreur lors de l'upload
                $('#uploadStatus').html("<div class='alert alert-danger'>Upload failed: " + error + "</div>");
            }
        });
    });
});

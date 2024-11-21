import os
print("Content-type: text/html; charset=UTF-8\r\n\r\n<html><head><title>Python CGI</title><link rel=\"stylesheet\" href=\"https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/css/bootstrap.min.css\"><link rel=\"preconnect\" href=\"https://fonts.googleapis.com\"><link rel=\"preconnect\" href=\"https://fonts.gstatic.com\" crossorigin><link href=\"https://fonts.googleapis.com/css2?family=Roboto+Mono:ital,wght@0,100..700;1,100..700&display=swap\" rel=\"stylesheet\"></head><body class=\"m-3 mt-5\" style=\"background-color:#e5e5e5\"><div class=\"container p-3\"><div class=\"rounded-3 p-5\" style=\"background-color:#fcfcfc; border:solid 1px #cdcdcd\"><center><h2>Python CGI</h2></center><br/><ul class=\"list-group\">")

for name, value in os.environ.items():
    print("<li class=\"list-group-item\">{0}: {1}</li>".format(name, value))

print("<ul></div></div></body></html>")

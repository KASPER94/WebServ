#!/bin/bash

printf "Content-type: text/html; charset=UTF-8\r\n\r\n<html><head><title>Same ports</title><link rel=\"stylesheet\" href=\"https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/css/bootstrap.min.css\"><link rel=\"preconnect\" href=\"https://fonts.googleapis.com\"><link rel=\"preconnect\" href=\"https://fonts.gstatic.com\" crossorigin><link href=\"https://fonts.googleapis.com/css2?family=Roboto+Mono:ital,wght@0,100..700;1,100..700&display=swap\" rel=\"stylesheet\"></head><body class=\"m-3 mt-5\" style=\"background-color:#e5e5e5\"><div class=\"container p-3\"><div class=\"rounded-3 p-5\" style=\"background-color:#fcfcfc; border:solid 1px #cdcdcd\"><center><h2>Same ports</h2></center><br/>"
printf "static.fr<div class=\"rounded-2 p-2\" style=\"background-color:#cecece\"><code>"
printf "$> curl --resolve static.fr:8086:127.0.0.1 http://static.fr:8086<br />"
printf "</code></div>"
printf "<br />upload.fr<div class=\"rounded-2 p-2\" style=\"background-color:#cecece\"><code>"
printf "$> curl --resolve upload.fr:8086:127.0.0.1 http://upload.fr:8086<br />"
printf "</code></div>"
printf "</div></div></body></html>"

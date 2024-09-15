// webpage.h
#ifndef WEBPAGE_H
#define WEBPAGE_H

const char* webpage = R"rawliteral(
<!DOCTYPE html> <html>
<head><meta name="viewport" content="width=device-width initial-scale=1.0, user-scalable=no">
<title>ESP32 Temperature Monitor</title>
<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}
body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}
p {font-size: 24px;color: #444444;margin-bottom: 10px;}
</style>
<script>
setInterval(loadDoc,100);
function loadDoc() {
var xhttp = new XMLHttpRequest();
xhttp.onreadystatechange = function() {
if (this.readyState == 4 && this.status == 200) {
document.body.innerHTML =this.responseText}
};
xhttp.open("GET", "/", true);
xhttp.send();
}
</script>
</head>
<body>
<div id="webpage">
<h1>Temperature Monitor</h1>
<p>Temperature: %TEMP% &deg;C</p>
</div>
</body>
</html>
)rawliteral";

#endif
